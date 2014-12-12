#include "StringBleValue.h"
#include "Utils.h"

void StringBleValue::setValue(string value)
{
	_value = value;
}

string StringBleValue::getValue()
{
	return _value;
}

ByteData StringBleValue::read()
{
	ByteData ret;
	ByteDataHelper::writeStringToByteData(ret, _value);
	return ret;
}

void StringBleValue::write(const ByteData& data)
{
	_value = ByteDataHelper::byteDataToString(data);
}
