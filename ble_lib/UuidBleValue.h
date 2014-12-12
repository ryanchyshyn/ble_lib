#pragma once

#include "BleValue.h"
#include "BleUuid.h"

class UuidBleValue : public BleValue
{
private:
	BleUuid _value;

public:
	void setValue(const BleUuid& _value);
	const BleUuid& getValue();

	virtual ByteData read();
	virtual void write(const ByteData& data);
};

