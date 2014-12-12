#pragma once
#include <map>
#include <memory>
#include "BleBase.h"
#include "BleCharacteristics.h"

using namespace ::std;

class BleService : public BleBase
{
public:
	map<BleUuid, shared_ptr<BleCharacteristics>> characteristicsList;

	uint16_t startHandle;
	uint16_t endHandle;

public:
	void addCharacteristics(shared_ptr<BleCharacteristics>& characteristics);
};