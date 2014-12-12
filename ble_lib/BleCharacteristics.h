#pragma once
#include <memory>
#include "BleBase.h"
#include "BleValue.h"
#include "BleDescription.h"

using namespace ::std;

class BleCharacteristics : public BleBase
{
public:
	uint8_t properties;
	uint8_t secure;
	shared_ptr<BleValue> value;
	shared_ptr<BleDescription> description;

	uint16_t startHandle;
	uint16_t valueHandle;

};