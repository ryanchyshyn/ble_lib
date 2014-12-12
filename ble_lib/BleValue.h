#pragma once

#include "BleBase.h"
#include "ByteDataHelper.h"

class BleValue : public BleBase
{
public:
	virtual ByteData read() = 0;
	virtual void write(const ByteData& data) = 0;
};