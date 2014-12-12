#define LOG_HCI

#include "BleHci.h"
#include <memory>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/prctl.h>
#include <unistd.h>
#include "BleScanNode.h"
#include "Utils.h"
#include "HciUtils.h"
#include "ParseBuffer.h"

using namespace ::std;

void* BleHci::threadFunc(void *arg)
{
	BleHci* This = (BleHci*)arg;
	This->run();
	return NULL;
}

BleHci::BleHci() : threadId(-1), exitThread(false), scanningEnabled(false), hciSocket(-1), orgFilterSaved(false), prevAdapterState(0), advertisingEnabled(false)
{
}

BleHci::~BleHci()
{
	cleanup();
}

int BleHci::init()
{
	if (isInitialized()) return 0;

	int res;
	if (res = createSocket()) return res;

	if (res = setFilter())
	{
		closeSocket();
		return res;
	}

	if (res = startThread())
	{
		closeSocket();
		return res;
	}

	return 0;
}

void BleHci::cleanup()
{
	if (!isInitialized()) return;
	disableScanning();
	disableAdvertising();
	stopThread();
}

bool BleHci::isInitialized()
{
	return threadId != -1;
}

int BleHci::createSocket()
{
	int res;
	if (res = selectDevice()) return res;
	hciSocket = hci_open_dev(hciDevInfo.dev_id);
	if (hciSocket < 0)
	{
		HCI_LOG_ERROR(errno, "hci_open_dev failed");
		return errno;
	}

	// HACK without this call all subsequent calls will fail
	hci_le_set_scan_enable(hciSocket, 0x00, 0, 1000);

	return 0;
}

int BleHci::setFilter()
{
	socklen_t orgFilterLen = sizeof(orgFilter);
	if (getsockopt(hciSocket, SOL_HCI, HCI_FILTER, &orgFilter, &orgFilterLen) < 0)
	{
		HCI_LOG_ERROR(errno, "getsockopt failed");
		return errno;
	}
	orgFilterSaved = true;

	hci_filter newFilter;
	hci_filter_clear(&newFilter);
	hci_filter_set_ptype(HCI_EVENT_PKT, &newFilter);
	hci_filter_set_event(EVT_LE_META_EVENT, &newFilter);

	if (setsockopt(hciSocket, SOL_HCI, HCI_FILTER, &newFilter, sizeof(newFilter)) < 0)
	{
		HCI_LOG_ERROR(errno, "setsockopt failed");
		return errno;
	}

	return 0;
}

int BleHci::startThread()
{
	prevAdapterState = 0;
	exitThread = false;

	if (pthread_create(&threadId, NULL, &threadFunc, this) != 0)
	{
		int ret = errno;
		HCI_LOG_ERROR(errno, "pthread_create failed");
		closeSocket();
		return ret;
	}

	return 0;
}

void BleHci::stopThread()
{
	if (isInitialized())
	{
		exitThread = true;
		pthread_join(threadId, NULL);
		threadId = -1;

		closeSocket();
	}
}

void BleHci::closeSocket()
{
	if (hciSocket != -1)
	{
		if (orgFilterSaved)
		{
			// restore original filter
			setsockopt(hciSocket, SOL_HCI, HCI_FILTER, &orgFilter, sizeof(orgFilter));
			orgFilterSaved = false;
		}

		hci_close_dev(hciSocket);
		hciSocket = -1;
	}
}

int BleHci::enableScanning(bool filterDuplicates)
{
	if (!isInitialized()) return EPERM;
	if (scanningEnabled) return 0;

	if (hci_le_set_scan_enable(hciSocket, 0x01, filterDuplicates ? 1 : 0, 1000) < 0)
	{
		HCI_LOG_ERROR(errno, "hci_le_set_scan_enable failed");
		return errno;
	}

	scanningEnabled = true;
	return 0;
}

int BleHci::disableScanning()
{
	if (!isInitialized()) return EPERM;
	if (scanningEnabled)
	{
		if (hci_le_set_scan_enable(hciSocket, 0x00, 0, 1000) < 0)
		{
			HCI_LOG_ERROR(errno, "hci_le_set_scan_enable failed");
			return errno;
		}
		scanningEnabled = false;
	}
	return 0;
}

bool BleHci::isScanningEnabled()
{
	return scanningEnabled;
}

int BleHci::enableAdvertising()
{
	if (!advertisingEnabled)
	{
		int ret;
		if (hci_le_set_advertising_parameters(hciSocket, 1000) < 0)
		{
			HCI_LOG_ERROR(errno, "hci_le_set_advertising_parameters failed");
			return errno;
		}
		if (hci_enable_advertising(hciSocket, 1000) < 0)
		{
			HCI_LOG_ERROR(errno, "hci_enable_advertising failed");
			return errno;
		}

		advertisingEnabled = true;
	}
	return 0;
}

int BleHci::setAdvertizingParams(const ByteData& advData, const ByteData& scanData)
{
	// set advertising data
	if (hci_le_set_advertising_data(hciSocket, (uint8_t*)&advData.front(), advData.size(), 1000) < 0)
	{
		HCI_LOG_ERROR(errno, "hci_le_set_advertising_data failed");
		return errno;
	}

	// set scan data
	if (hci_le_set_scan_response_data(hciSocket, (uint8_t*)&scanData.front(), scanData.size(), 1000) < 0)
	{
		HCI_LOG_ERROR(errno, "hci_le_set_scan_response_data failed");
		return errno;
	}

	return 0;
}

int BleHci::selectDevice()
{
	int devId = hci_get_route(NULL);
	bdaddr_t ba;
	if ((devId != -1) && (hci_devba(devId, &ba) < 0))
	{
		HCI_LOG_ERROR(errno, "hci_devba failed");
		return errno;
	}

	memset(&hciDevInfo, 0x00, sizeof(hciDevInfo));
	hciDevInfo.dev_id = devId;

	return 0;
}

int BleHci::disableAdvertising()
{
	if (advertisingEnabled)
	{
		//if (hci_le_set_advertise_enable(hciSocket, 0, 1000) < 0)
		if (hci_disable_advertising(hciSocket, 1000) < 0)
		{
			HCI_LOG_ERROR(errno, "hci_disable_advertising failed");
			return errno;
		}
		advertisingEnabled = false;
	}
	return 0;
}

bool BleHci::isAdvertisingEnabled()
{
	return advertisingEnabled;
}

void BleHci::checkAdapterState()
{
	ioctl(hciSocket, HCIGETDEVINFO, (void *)&hciDevInfo);
	int adapterState = hci_test_bit(HCI_UP, &hciDevInfo.flags);

	if (prevAdapterState != adapterState)
	{
		prevAdapterState = adapterState;

		if (!adapterState) onAdapterStateChanged(AdapterState::Disabled);
		else if (hci_le_set_scan_parameters(hciSocket, 0x01, htobs(0x0010), htobs(0x0010), 0x00, 0, 1000) < 0)
		{
			if (errno == EPERM) onAdapterStateChanged(AdapterState::Unauthorised);
			else if (errno == EIO) onAdapterStateChanged(AdapterState::Unsupported);
			else onAdapterStateError(errno);
		}
		else onAdapterStateChanged(AdapterState::Enabled);
	}
}

void BleHci::run()
{
	while (!exitThread)
	{
		checkAdapterState();

		fd_set fd;
		FD_ZERO(&fd);
		FD_SET(hciSocket, &fd);

		timeval tv;
		tv.tv_sec = 0;
		tv.tv_usec = SELECT_TIME;

		int result = select(hciSocket + 1, &fd, NULL, NULL, &tv);
		if (result == -1)
		{
			HCI_LOG_ERROR(errno, "select failed");
			onHciError(errno);
			continue;
		}
		else if (result)
		{
			unsigned char hciEventBuf[HCI_MAX_EVENT_SIZE];
			int	hciEventLen = read(hciSocket, hciEventBuf, sizeof(hciEventBuf));
			if (hciEventLen <= 0)
			{
				HCI_LOG_ERROR(errno, "read failed");
				onHciError(errno);
				continue;
			}

			if (!scanningEnabled) continue;

			/* point past the HCI Event header and adjust length */
			evt_le_meta_event *leMetaEvent = (evt_le_meta_event*)(hciEventBuf + (1 + HCI_EVENT_HDR_SIZE));
			hciEventLen -= (1 + HCI_EVENT_HDR_SIZE);

			if (leMetaEvent->subevent != EVT_LE_ADVERTISING_REPORT) continue;

			le_advertising_info *leAdvInfo = (le_advertising_info *)(leMetaEvent->data + 1);

			processLeAdvData(leAdvInfo->bdaddr, (leAdvInfo->bdaddr_type == LE_PUBLIC_ADDRESS), leAdvInfo->data, leAdvInfo->length, (signed char)*(leAdvInfo->data + leAdvInfo->length));
		}
	}
}

void BleHci::processLeAdvData(const bdaddr_t& btAddress, bool isPublic, uint8_t* eirData, int eirDataLen, int rssi)
{
	shared_ptr<BleScanNode> node(new BleScanNode());
	node->setAddress(btAddress);
	node->setIsPublic(isPublic);
	node->setRssi(rssi);

	ParseBuffer pb(eirData, eirDataLen);
	while (pb.bytesAvailable() != 0)
	{
		// https://www.bluetooth.org/en-us/specification/assigned-numbers/generic-access-profile
		int charLength = pb.readUInt8();
		if (pb.getIndex() + charLength > eirDataLen)
		{
			// length is out of buffer range
			HCI_LOG_ERROR(errno, "Invalid advertisement packet: length is out of buffer range");
			return;
		}
		AdvCharacteristicType type = (AdvCharacteristicType)pb.readUInt8();
		--charLength;

		// parse the rest
		switch (type)
		{
		case Flags:
			{
				GapAdvFlag flags = (GapAdvFlag) pb.readUInt8();
				node->setFlags(flags);
				break;
			}

		case IncompleteList16BitServiceClassUuids:
		case CompleteList16BitServiceClassUuids:
			for (; charLength > 0; charLength -= 2)
			{
				node->addServiceUuid(pb.readUInt16LE());
			}
			break;

		case IncompleteList128BitServiceClassUuids:
		case CompleteList128BitServiceClassUuids:
			for (; charLength > 0; charLength -= 16)
			{
				node->addServiceUuid(pb.readUuidLE());
			}
			break;

		case ShortenedLocalName:
		case CompleteLocalName:
			{
				// TODO UTF-8
				string name((char*)pb.getCurrData(), charLength);
				pb.skip(charLength);
				node->setName(name);
				break;
			}

		case TxPowerLevel:
			{
				uint8_t txPower = pb.readUInt8();
				node->setTxPower((signed char)txPower);
				break;
			}

		case ServiceData:
			{
				uint16_t uuid = pb.readUInt16LE();
				charLength -= sizeof(uint16_t);
				ByteData data = pb.readBytes(charLength);
				node->addServiceData(BleUuid(uuid), data);
				break;
			}

		case ManufacturerSpecificData:
			{
				ByteData data = pb.readBytes(charLength);
				node->setManufacturerData(data);
				break;
			}

		default:
			HCI_LOG_DEBUG("unprocessed AdvCharacteristicType type: 0x%0x", (int)type);
			// log unknown type
			pb.skip(charLength);
		}
	}

	onScanFinished(node);
}