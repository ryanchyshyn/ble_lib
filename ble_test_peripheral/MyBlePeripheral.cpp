#include "MyBlePeripheral.h"
#include "ByteDataHelper.h"
#include "BleService.h"
#include "StringBleValue.h"
#include "ByteDataBleValue.h"


MyBlePeripheral::MyBlePeripheral()
{
}


MyBlePeripheral::~MyBlePeripheral()
{
	disableAdvertising();
}

void MyBlePeripheral::onHciError(int error)
{
	printf("onHciError: %d\n", error);
}

void MyBlePeripheral::onAdapterStateChanged(AdapterState state)
{
	printf("onAdapterStateChanged: %d\n", state);
	if (state == Enabled)
	{
		// create advertising node
		shared_ptr<BleAdvNode> node(new BleAdvNode());

		node->setFlags(GapAdvFlag::ADV_FLAG_BR_EDR_NOT_SUPPORTED);
		node->setName("ble_lib");

		ByteData d;
		ByteDataHelper::writeStringToByteData(d, "test");
		node->setManufacturerData(d);


		{
			shared_ptr<BleService> service(new BleService());
			service->uuid = GattSvc::GENERIC_ACCESS_SVC_UUID;

			{
				shared_ptr<BleCharacteristics> deviceNameChar(new BleCharacteristics());
				deviceNameChar->uuid = GattUuid::ATTR_DEVICE_NAME_UUID;
				shared_ptr<StringBleValue> value(new StringBleValue());
				value->setValue(node->getName());
				deviceNameChar->value = value;
				deviceNameChar->properties = GattProperties::GATT_PROP_READ | GattProperties::GATT_PROP_WRITE;

				service->addCharacteristics(deviceNameChar);
			}

			{
				shared_ptr<BleCharacteristics> appearanceChar(new BleCharacteristics());
				appearanceChar->uuid = GattUuid::ATTR_APPEARANCE_UUID;
				shared_ptr<ByteDataBleValue> value(new ByteDataBleValue());
				ByteData data;
				ByteDataHelper::writeUInt16BE(data, GapAppearance::Generic_Computer);
				value->setValue(data);
				appearanceChar->value = value;
				appearanceChar->properties = GattProperties::GATT_PROP_READ;

				service->addCharacteristics(appearanceChar);
			}

			node->addService16(service);
		}

		node->fillHandles();

		setAdvertisementNode(node);

		setAdvertisementParams();
		enableAdvertising();
	}
}

void MyBlePeripheral::onAdapterStateError(int error)
{
	printf("onAdapterStateError: %d\n", error);
}

void MyBlePeripheral::onL2CapError(int error)
{
	printf("onL2CapError: %d\n", error);
}

void MyBlePeripheral::onSecurityLevelChanged(uint8_t securityLevel)
{
	printf("onSecurityLevelChanged: %d\n", (int)securityLevel);
}
