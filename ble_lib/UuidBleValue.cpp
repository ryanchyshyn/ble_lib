#include "UuidBleValue.h"
#include "ParseBuffer.h"

void UuidBleValue::setValue(const BleUuid& value)
{
	_value = value;
}

const BleUuid& UuidBleValue::getValue()
{
	return _value;
}

ByteData UuidBleValue::read()
{
	printf("read uuid\n");
	ByteData ret;
	ByteDataHelper::writeUuidBE(ret, _value);
	return ret;
}

void UuidBleValue::write(const ByteData& data)
{
	ParseBuffer buffer(data);
	if (buffer.bytesAvailable() == 16)
	{
		printf("write uuid 128\n");
		_value = buffer.readUuidBE();
	}
	else if (buffer.bytesAvailable() == 2)
	{
		printf("write uuid 16\n");
		_value = buffer.readUInt16BE();
	}
	else
	{
		printf("invalid write uuid\n");
		// TODO
	}
}
