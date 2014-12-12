#include "BleL2Cap.h"

#include <errno.h>
#include <signal.h>
#include <sys/prctl.h>
#include <unistd.h>
#include <stdlib.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/l2cap.h>
#include "BLE.h"

void* BleL2Cap::threadFunc(void *arg)
{
	BleL2Cap* This = (BleL2Cap*)arg;
	This->run();
	return NULL;
}

BleL2Cap::BleL2Cap() : threadId(-1), exitThread(false), listenSocket(-1), clientSocket(-1)
{
}

BleL2Cap::~BleL2Cap()
{
	cleanup();
}

int BleL2Cap::init()
{
	if (isInitialized()) return 0;

	// create socket
	listenSocket = socket(AF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);
	if (listenSocket == -1)
	{
		L2CAP_LOG_ERROR(errno, "socket failed");
		return errno;
	}

	bdaddr_t daddr;
	{
		int hciDeviceId = hci_get_route(NULL);
		if (hciDeviceId < 0) hciDeviceId = 0; // use device 0, if device id is invalid

		int hciSocket = hci_open_dev(hciDeviceId);
		if (hciSocket == -1)
		{
			int ret = errno;
			L2CAP_LOG_ERROR(ret, "hci_open_dev failed");
			close(listenSocket);
			return ret;
		}
		if (hci_read_bd_addr(hciSocket, &daddr, 1000) == -1)
		{
			//daddr = *BDADDR_ANY;
			memset(&daddr, 0, sizeof(daddr));
		}
		close(hciSocket);
	}

	// bind
	memset(&sockAddr, 0, sizeof(sockAddr));
	sockAddr.l2_family = AF_BLUETOOTH;
	sockAddr.l2_bdaddr = daddr;
	sockAddr.l2_cid = htobs(ATT_CID);

	if (bind(listenSocket, (struct sockaddr*)&sockAddr, sizeof(sockAddr)) == -1)
	{
		int ret = errno;
		L2CAP_LOG_ERROR(ret, "hci_open_dev failed");
		close(listenSocket);
		return ret;
	}

	if (listen(listenSocket, 1) == -1)
	{
		int ret = errno;
		L2CAP_LOG_ERROR(ret, "listen failed");
		close(listenSocket);
		return ret;
	}

	// start thread
	exitThread = false;

	if (pthread_create(&threadId, NULL, &threadFunc, this) != 0)
	{
		int ret = errno;
		L2CAP_LOG_ERROR(ret, "pthread_create failed");
		close(listenSocket);
		return ret;
	}

	return 0;
}

void BleL2Cap::cleanup()
{
	if (!isInitialized()) return;
	if (threadId != -1)
	{
		exitThread = true;
		pthread_join(threadId, NULL);
		threadId = -1;

		closeSockets();
	}
}

bool BleL2Cap::isInitialized()
{
	return threadId != -1;
}

void BleL2Cap::closeSockets()
{
	if (listenSocket != -1)
	{
		close(listenSocket);
		listenSocket = -1;
	}
	if (clientSocket != -1)
	{
		close(clientSocket);
		clientSocket = -1;
	}
}

void BleL2Cap::run()
{
	while (!exitThread)
	{
		fd_set fd1;
		FD_ZERO(&fd1);
		FD_SET(listenSocket, &fd1);

		timeval tv;
		tv.tv_sec = 1;
		tv.tv_usec = 0;

		int result = select(listenSocket + 1, &fd1, NULL, NULL, &tv);
		if (result == -1)
		{
			onL2CapError(errno);
			continue;
		}

		if (result && FD_ISSET(listenSocket, &fd1))
		{
			socklen_t sockAddrLen = sizeof(sockAddr);
			clientSocket = accept(listenSocket, (struct sockaddr *)&sockAddr, &sockAddrLen);

			onAccept(sockAddr.l2_bdaddr);

			uint8_t securityLevel = 0;

			ByteData l2capSockBuf;
			while (!exitThread)
			{
				fd_set fd2;
				FD_ZERO(&fd2);
				FD_SET(clientSocket, &fd2);

				tv.tv_sec = 1;
				tv.tv_usec = 0;

				result = select(clientSocket + 1, &fd2, NULL, NULL, &tv);

				if (result == -1)
				{
					onL2CapError(errno);
					break;
				}

				if (result)
				{
					if (FD_ISSET(clientSocket, &fd2))
					{
						l2capSockBuf.resize(L2CAP_READ_BUFFER_SIZE);
						int len = read(clientSocket, &l2capSockBuf.front(), l2capSockBuf.size());
						if (len <= 0)
						{
							onL2CapError(errno);
							break;
						}
						l2capSockBuf.resize(len);

						bt_security btSecurity;
						memset(&btSecurity, 0, sizeof(btSecurity));
						socklen_t btSecurityLen = sizeof(btSecurity);
						getsockopt(clientSocket, SOL_BLUETOOTH, BT_SECURITY, &btSecurity, &btSecurityLen);

						if (securityLevel != btSecurity.level)
						{
							securityLevel = btSecurity.level;
							onSecurityLevelChanged(securityLevel);
						}

						onDataReceived(l2capSockBuf);
					}
				}

				{
					// check for write
					if (writeData.size() != 0)
					{
						const ByteData& writeItem = writeData.front();
						int len = write(clientSocket, (const void*) &writeItem.front(), writeItem.size());
						if ((len < 0) || (len != writeItem.size()))
						{
							writeData.pop_front();
							onL2CapError(errno);
						}
						else
						{
							string hexData = ByteDataHelper::byteDataToHexString(writeItem);
							L2CAP_LOG_DEBUG("L2CAP write: %s", hexData.c_str());

							writeData.pop_front();
							onWriteDone(writeItem);
						}
					}
				}
			}

			onDisconnected(sockAddr.l2_bdaddr);
			close(clientSocket);
			clientSocket = -1;
			
			writeData.clear();
		}
	}
}

void BleL2Cap::writeL2CapData(const ByteData& data)
{
	writeData.push_back(data);
}

void BleL2Cap::writeL2CapError(AttOp opcode, uint16_t handle, ErrECode status)
{
	ByteData data;
	ByteDataHelper::writeByte(data, AttOp::ATT_OP_ERROR);
	ByteDataHelper::writeByte(data, opcode);
	ByteDataHelper::writeUInt16BE(data, handle);
	ByteDataHelper::writeByte(data, status);
	writeL2CapData(data);
}

void BleL2Cap::onDataReceived(const ByteData& data)
{
	string hexData = ByteDataHelper::byteDataToHexString(data);
	L2CAP_LOG_DEBUG("L2CAP read: %s", hexData.c_str());

	if (data.size() == 0)
	{
		writeL2CapError(ATT_OP_ERROR, 0, ATT_ECODE_REQ_NOT_SUPP);
		return;
	}

	uint8_t code = data[0];
	switch (code)
	{
	case AttOp::ATT_OP_MTU_REQ:
		handleMtuRequest(data);
		break;

	case AttOp::ATT_OP_FIND_INFO_REQ:
		handleFindInfoRequest(data);
		break;

	case AttOp::ATT_OP_FIND_BY_TYPE_REQ:
		handleFindByTypeRequest(data);
		break;

	case AttOp::ATT_OP_READ_BY_TYPE_REQ:
		handleReadByTypeRequest(data);
		break;

	case AttOp::ATT_OP_READ_REQ:
	case AttOp::ATT_OP_READ_BLOB_REQ:
		handleReadOrReadBlobRequest(data);
		break;

	case AttOp::ATT_OP_READ_BY_GROUP_REQ:
		handleReadByGroupRequest(data);
		break;

	case AttOp::ATT_OP_WRITE_REQ:
	case AttOp::ATT_OP_WRITE_CMD:
		handleWriteRequestOrCommand(data);
		break;

	case AttOp::ATT_OP_READ_MULTI_REQ:
	case AttOp::ATT_OP_PREP_WRITE_REQ:
	case AttOp::ATT_OP_EXEC_WRITE_REQ:
	case AttOp::ATT_OP_SIGNED_WRITE_CMD:
	default:
		handleOther(data);
		writeL2CapError(ATT_OP_ERROR, 0x0000, ATT_ECODE_REQ_NOT_SUPP);
		break;
	}
}
