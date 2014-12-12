#include "ByteDataBleValue.h"


void ByteDataBleValue::setValue(const ByteData& value)
{
	_value = value;
}

ByteData& ByteDataBleValue::getValue()
{
	return _value;
}

ByteData ByteDataBleValue::read()
{
	return _value;
}

void ByteDataBleValue::write(const ByteData& data)
{
	_value = data;
}
