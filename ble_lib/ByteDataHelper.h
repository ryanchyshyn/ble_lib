#pragma once
#include <vector>
#include <string>
#include <bluetooth/bluetooth.h>
#include <uuid/uuid.h>
#include "Utils.h"
#include "BleUuid.h"

using namespace ::std;

typedef vector<uint8_t> ByteData;

class ByteDataHelper
{
public:
	static void writeByte(ByteData& data, uint8_t value);

	static uint16_t readUInt16LE(const ByteData& data, int pos);
	static void writeUInt16LE(ByteData& data, uint16_t value);

	static uint16_t readUInt16BE(const ByteData& data, int pos);
	static void writeUInt16BE(ByteData& data, uint16_t value);

	static void readUuidBE(const ByteData& data, uuid_t& ret);
	static void writeUuidBE(ByteData& data, const uuid_t& uuid);

	static void readUuidLE(const ByteData& data, uuid_t& ret);
	static void writeUuidLE(ByteData& data, const uuid_t& uuid);

	static void writeUuidLE(ByteData& data, const BleUuid& uuid);
	static void writeUuidBE(ByteData& data, const BleUuid& uuid);

	static void writeStringToByteData(ByteData& data, const string& str);
	static void writeHexStringToByteDataLE(ByteData& data, const string& str);
	static void writeHexStringToByteDataBE(ByteData& data, const string& str);

	static string byteDataToString(const ByteData& data);
	static string byteDataToHexStringReverse(const ByteData& data, int pos = 0);
	static string byteDataToHexString(const ByteData& data, int pos = 0);

};

