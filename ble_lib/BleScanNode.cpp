#include "BleScanNode.h"
#include <math.h>


void BleScanNode::setAddress(const bdaddr_t& address)
{
	bacpy(&_address, &address);
}

const bdaddr_t& BleScanNode::getAddress() const
{
	return _address;
}

void BleScanNode::setIsPublic(bool isPublic)
{
	_isPublic = isPublic;
}

bool BleScanNode::getIsPublic() const
{
	return _isPublic;
}

void BleScanNode::setRssi(int value)
{
	_rssi = value;
}

int BleScanNode::getRssi() const
{
	return _rssi;
}

void BleScanNode::setName(string name)
{
	_name = name;
}

string BleScanNode::getName() const
{
	return _name;
}

void BleScanNode::setTxPower(int value)
{
	txPower = value;
}

int BleScanNode::getTxPower() const
{
	return txPower;
}

void BleScanNode::setFlags(GapAdvFlag value)
{
	flags = value;
}

GapAdvFlag BleScanNode::getFlags() const
{
	return flags;
}

void BleScanNode::setManufacturerData(ByteData data)
{
	manufacturerData = data;
}

const ByteData& BleScanNode::getManufacturerData() const
{
	return manufacturerData;
}

void BleScanNode::addServiceUuid(const BleUuid& uuid)
{
	serviceUuids.push_back(uuid);
}

list<BleUuid> BleScanNode::getServiceUuidsList() const
{
	return serviceUuids;
}

void BleScanNode::addServiceData(const BleUuid& uuid, ByteData data)
{
	serviceData[uuid] = data;
}

const ByteData& BleScanNode::getServiceData(const BleUuid& uuid) const
{
	//return serviceData[uuid];
	return serviceData.find(uuid)->second;
}

bool BleScanNode::isIBeacon() const
{
	// INFO original iBeacon device has 25 bytes of manufacturer data
	if (manufacturerData.size() < 25) return false;
	return (manufacturerData[0] == 0x4C) &&
			(manufacturerData[1] == 0x00) &&
			(manufacturerData[2] == 0x02) &&
			(manufacturerData[3] == 0x15);
}

string BleScanNode::getIBeaconUuid() const
{
	const uuid_t* uuidPtr = (const uuid_t*)&manufacturerData[4];
	return formatUuid(uuidToString(*uuidPtr));
}

void BleScanNode::getIBeaconUuid(uuid_t& uuid) const
{
	uuid_t& t = *(uuid_t*)&manufacturerData[4];
	uuid_copy(uuid, t);
}

uint16_t BleScanNode::getMinor() const
{
	return (manufacturerData[22] << 8) | manufacturerData[23];
}

uint16_t BleScanNode::getMajor() const
{
	return (manufacturerData[20] << 8) | manufacturerData[21];
}

int BleScanNode::getIBeaconTxPower() const
{
	return (signed char)manufacturerData[24];
}

float BleScanNode::getAccuracy() const
{
	// http://stackoverflow.com/questions/20416218/understanding-ibeacon-distancing
	// TODO not checked
	if ((getRssi() == 0) || (getIBeaconTxPower() == 0)) return -1; // if we cannot determine accuracy, return -1.
	float ratio = getRssi() * 1.0f / getIBeaconTxPower();
	if (ratio < 1.0) return pow(ratio, 10);
	else return (0.89976f) * pow(ratio, 7.7095f) + 0.111f;
}
