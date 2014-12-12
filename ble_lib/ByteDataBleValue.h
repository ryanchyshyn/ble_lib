#pragma once

#include "BleValue.h"

class ByteDataBleValue : public BleValue
{
private:
	ByteData _value;

public:
	void setValue(const ByteData& value);
	ByteData& getValue();

	virtual ByteData read();
	virtual void write(const ByteData& data);
};

