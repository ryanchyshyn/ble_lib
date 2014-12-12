#pragma once

#include <map>
#include <string>
#include <list>
#include <bluetooth/bluetooth.h>
#include "ByteDataHelper.h"
#include "BleUuid.h"
#include "BLE.h"

using namespace ::std;

class BleScanNode
{
private:
	bdaddr_t _address;
	bool _isPublic;
	int _rssi;

	string _name;
	int txPower;
	GapAdvFlag flags;
	ByteData manufacturerData;
	list<BleUuid> serviceUuids;
	map<BleUuid, ByteData> serviceData;

public:
	void setAddress(const bdaddr_t& address);
	const bdaddr_t& getAddress() const;

	void setIsPublic(bool isPublic);
	bool getIsPublic() const;

	void setRssi(int value);
	int getRssi() const;

	void setName(string name);
	string getName() const;

	void setTxPower(int value);
	int getTxPower() const;

	void setFlags(GapAdvFlag value);
	GapAdvFlag getFlags() const;

	void setManufacturerData(ByteData data);
	const ByteData& getManufacturerData() const;

	void addServiceUuid(const BleUuid& uuid);
	list<BleUuid> getServiceUuidsList() const;

	void addServiceData(const BleUuid& uuid, ByteData data);
	const ByteData& getServiceData(const BleUuid& uuid) const;

	// iBeacon stuff
	bool isIBeacon() const;

	string getIBeaconUuid() const;
	void getIBeaconUuid(uuid_t& uuid) const;
	uint16_t getMinor() const;
	uint16_t getMajor() const;
	int getIBeaconTxPower() const;

	float getAccuracy() const;
};