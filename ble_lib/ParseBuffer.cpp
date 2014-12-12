#include "ParseBuffer.h"
#include <bluetooth/bluetooth.h>
#include "OutOfRangeException.h"

ParseBuffer::ParseBuffer(const uint8_t* data, size_t dataLen) : _data(data), _dataLen(dataLen), _index(0)
{
}

ParseBuffer::ParseBuffer(const ByteData& data) : _data(&data.front()), _dataLen(data.size()), _index(0)
{
}

void ParseBuffer::reset()
{
	_index = 0;
}

size_t ParseBuffer::getIndex() const
{
	return _index;
}

size_t ParseBuffer::bytesAvailable() const
{
	return _dataLen - _index;
}

const uint8_t* ParseBuffer::getCurrData() const
{
	return _data + _index;
}

bool ParseBuffer::canRead8() const
{
	return _index + sizeof(uint8_t) <= _dataLen;
}
bool ParseBuffer::canRead16() const
{
	return _index + sizeof(uint16_t) <= _dataLen;
}

bool ParseBuffer::canReadUuid() const
{
	return _index + sizeof(uuid_t) <= _dataLen;
}

void ParseBuffer::skip(size_t length)
{
	_index += length;
}

uint8_t ParseBuffer::readUInt8()
{
	if (!canRead8()) throw OutOfRangeException();
	uint8_t ret = _data[_index];
	_index += sizeof(uint8_t);
	return ret;
}

uint16_t ParseBuffer::readUInt16LE()
{
	if (!canRead16()) throw OutOfRangeException();
	uint16_t ret = bt_get_le16(_data + _index);
	_index += sizeof(uint16_t);
	return ret;
}

uint16_t ParseBuffer::readUInt16BE()
{
	if (!canRead16()) throw OutOfRangeException();
	uint16_t ret = bt_get_be16(_data + _index);
	_index += sizeof(uint16_t);
	return ret;
}

BleUuid ParseBuffer::readUuidLE()
{
	if (!canReadUuid()) throw OutOfRangeException();
	uint128_t ret;
	ntoh128((const uint128_t*)(_data + _index), &ret);
	_index += sizeof(uuid_t);
	return BleUuid(*((const uuid_t*)&ret));
}

BleUuid ParseBuffer::readUuidBE()
{
	if (!canReadUuid()) throw OutOfRangeException();
	uint128_t ret;
	btoh128((const uint128_t*)(_data + _index), &ret);
	_index += sizeof(uuid_t);
	return BleUuid(*((const uuid_t*)&ret));
}

ByteData ParseBuffer::readBytes()
{
	size_t len = bytesAvailable();
	ByteData ret;
	ret.resize(len, 0);
	memcpy(&ret.front(), _data + _index, len);
	_index += len;
	return ret;
}

ByteData ParseBuffer::readBytes(size_t length)
{
	if (length > bytesAvailable()) throw OutOfRangeException();
	ByteData ret;
	ret.resize(length, 0);
	memcpy(&ret.front(), _data + _index, length);
	_index += length;
	return ret;
}
