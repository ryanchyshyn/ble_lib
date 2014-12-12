#include "BleAdvNode.h"
#include "BLE.h"


BleAdvNode::BleAdvNode()
{
}

void BleAdvNode::setName(const string& name)
{
	_name = name;
}

string& BleAdvNode::getName()
{
	return _name;
}

void BleAdvNode::setFlags(GapAdvFlag flags)
{
	_flags = flags;
}

GapAdvFlag BleAdvNode::getFlags()
{
	return _flags;
}

void BleAdvNode::setManufacturerData(const ByteData& data)
{
	manufacturerData = data;
}

const ByteData& BleAdvNode::getManufacturerData()
{
	return manufacturerData;
}

void BleAdvNode::addService16(shared_ptr<BleService> service)
{
	services16[service->uuid] = service;
}

void BleAdvNode::addService128(shared_ptr<BleService> service)
{
	services128[service->uuid] = service;
}

ByteData BleAdvNode::getAdvertisingData()
{
	ByteData ret;

	// flags
	ByteDataHelper::writeByte(ret, 2);
	ByteDataHelper::writeByte(ret, AdvCharacteristicType::Flags);
	ByteDataHelper::writeByte(ret, _flags);

	for (map<BleUuid, shared_ptr<BleService>>::iterator it = services16.begin(); it != services16.end(); ++it)
	{
		shared_ptr<BleService> service = it->second;

		ByteDataHelper::writeByte(ret, 1 + service->uuid.getLength());
		ByteDataHelper::writeByte(ret, AdvCharacteristicType::CompleteList16BitServiceClassUuids);
		ByteDataHelper::writeUInt16LE(ret, service->uuid.get16Uuid());
	}

	if (manufacturerData.size() != 0)
	{
		ByteDataHelper::writeByte(ret, 1 + manufacturerData.size());
		ByteDataHelper::writeByte(ret, AdvCharacteristicType::ManufacturerSpecificData);
		ret.insert(ret.end(), manufacturerData.begin(), manufacturerData.end());
	}

	return ret;
}

ByteData BleAdvNode::getScanningData()
{
	ByteData ret;

	for (map<BleUuid, shared_ptr<BleService>>::iterator it = services128.begin(); it != services128.end(); ++it)
	{
		shared_ptr<BleService> service = it->second;

		ByteDataHelper::writeByte(ret, 1 + service->uuid.getLength());
		ByteDataHelper::writeByte(ret, AdvCharacteristicType::CompleteList128BitServiceClassUuids);
		ByteDataHelper::writeUuidLE(ret, service->uuid.get128Uuid());
	}

	ByteDataHelper::writeByte(ret, _name.length() + 1);
	ByteDataHelper::writeByte(ret, AdvCharacteristicType::CompleteLocalName);
	for (size_t i = 0; i < _name.size(); ++i)
	{
		ByteDataHelper::writeByte(ret, _name[i]);
	}

	return ret;
}

vector<shared_ptr<BleBase>>& BleAdvNode::getHandles()
{
	return handles;
}

void BleAdvNode::fillHandles()
{
	for (map<BleUuid, shared_ptr<BleService>>::iterator it = services16.begin(); it != services16.end(); ++it)
	{
		shared_ptr<BleService> service = it->second;
		handles.push_back(service);
		service->startHandle = handles.size();

		for (map<BleUuid, shared_ptr<BleCharacteristics>>::iterator it2 = service->characteristicsList.begin(); it2 != service->characteristicsList.end(); ++it2)
		{
			shared_ptr<BleCharacteristics> characteristics = it2->second;
			handles.push_back(characteristics);
			characteristics->startHandle = handles.size();

			// put values
			if (characteristics->value)
			{
				handles.push_back(characteristics->value);
				characteristics->valueHandle = handles.size();
			}

			// put description
			if (characteristics->description)
			{
				handles.push_back(characteristics->description);
				characteristics->valueHandle = handles.size();
			}
		}

		service->endHandle = handles.size();
	}

	for (map<BleUuid, shared_ptr<BleService>>::iterator it = services128.begin(); it != services128.end(); ++it)
	{
		shared_ptr<BleService> service = it->second;
		handles.push_back(service);
		service->startHandle = handles.size();

		for (map<BleUuid, shared_ptr<BleCharacteristics>>::iterator it2 = service->characteristicsList.begin(); it2 != service->characteristicsList.end(); ++it2)
		{
			shared_ptr<BleCharacteristics> characteristics = it2->second;
			handles.push_back(characteristics);
			characteristics->startHandle = handles.size();

			// put values
			if (characteristics->value)
			{
				handles.push_back(characteristics->value);
				characteristics->valueHandle = handles.size();
			}

			// put description
			if (characteristics->description)
			{
				handles.push_back(characteristics->description);
				characteristics->valueHandle = handles.size();
			}
		}

		service->endHandle = handles.size();
	}
}
