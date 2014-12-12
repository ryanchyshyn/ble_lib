#pragma once
#include <stdint.h>
#include <vector>
#include <map>
#include <memory>
#include "Utils.h"
#include "BleService.h"
#include "BleBase.h"
#include "BleUuid.h"
#include "BLE.h"

using namespace ::std;

class BleAdvNode
{
private:
	map<BleUuid, shared_ptr<BleService>> services16;
	map<BleUuid, shared_ptr<BleService>> services128;

	string _name;
	GapAdvFlag _flags;
	ByteData manufacturerData;

	vector<shared_ptr<BleBase>> handles;

public:
	BleAdvNode();

	void setName(const string& name);
	string& getName();

	void setFlags(GapAdvFlag flags);
	GapAdvFlag getFlags();

	void setManufacturerData(const ByteData& data);
	const ByteData& getManufacturerData();

	void addService16(shared_ptr<BleService> service);
	void addService128(shared_ptr<BleService> service);

	ByteData getAdvertisingData();
	ByteData getScanningData();

	vector<shared_ptr<BleBase>>& getHandles();
	void fillHandles();
};