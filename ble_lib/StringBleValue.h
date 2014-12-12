#pragma once

#include "BleValue.h"

class StringBleValue : public BleValue
{
private:
	string _value;

public:
	void setValue(string value);
	string getValue();

	virtual ByteData read();
	virtual void write(const ByteData& data);
};

