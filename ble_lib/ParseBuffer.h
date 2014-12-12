#pragma once
#include <stdint.h>
#include "BleUuid.h"
#include "ByteDataHelper.h"

class ParseBuffer
{
private:
	const uint8_t* _data;
	size_t _dataLen;
	size_t _index;

public:
	ParseBuffer(const uint8_t* data, size_t dataLen);
	ParseBuffer(const ByteData& data);

	void reset();
	size_t getIndex() const;
	size_t bytesAvailable() const;
	const uint8_t* getCurrData() const;
	bool canRead8() const;
	bool canRead16() const;
	bool canReadUuid() const;

	void skip(size_t length);
	uint8_t readUInt8();
	uint16_t readUInt16LE();
	uint16_t readUInt16BE();
	BleUuid readUuidLE();
	BleUuid readUuidBE();
	ByteData readBytes();
	ByteData readBytes(size_t length);
};