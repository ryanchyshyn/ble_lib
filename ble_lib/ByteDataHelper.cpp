#include "ByteDataHelper.h"


void ByteDataHelper::writeByte(ByteData& data, uint8_t value)
{
	data.push_back(value);
}

uint16_t ByteDataHelper::readUInt16LE(const ByteData& data, int pos)
{
	return (data[pos + 1] << 8) | data[pos];
}

void ByteDataHelper::writeUInt16LE(ByteData& data, uint16_t value)
{
	data.push_back((value >> 8) & 0xFF);
	data.push_back(value & 0xFF);
}

uint16_t ByteDataHelper::readUInt16BE(const ByteData& data, int pos)
{
	return (data[pos] << 8) | data[pos + 1];
}

void ByteDataHelper::writeUInt16BE(ByteData& data, uint16_t value)
{
	data.push_back(value & 0xFF);
	data.push_back((value >> 8) & 0xFF);
}

void ByteDataHelper::writeUuidBE(ByteData& data, const uuid_t& uuid)
{
	for (size_t i = 0; i < sizeof(uuid); ++i)
	{
		data.push_back(uuid[i]);
	}
}

void ByteDataHelper::writeUuidLE(ByteData& data, const uuid_t& uuid)
{
	for (size_t i = sizeof(uuid); i > 0; --i)
	{
		data.push_back(uuid[i - 1]);
	}
}

void ByteDataHelper::readUuidLE(const ByteData& data, uuid_t& ret)
{
	unsigned char* t = (unsigned char*) ret;
	if (data.size() == 16)
	{
		for (size_t i = 16; i > 0; --i)
		{
			t[16 - i] = data[i - 1];
		}
	}
}

void ByteDataHelper::readUuidBE(const ByteData& data, uuid_t& ret)
{
	if (data.size() == 16)
	{
		memcpy(ret, &data.front(), data.size());
	}
}

void ByteDataHelper::writeUuidLE(ByteData& data, const BleUuid& uuid)
{
	if (uuid.isUuid128()) writeUuidLE(data, uuid.get128Uuid());
	else writeUInt16LE(data, uuid.get16Uuid());
}

void ByteDataHelper::writeUuidBE(ByteData& data, const BleUuid& uuid)
{
	if (uuid.isUuid128()) writeUuidBE(data, uuid.get128Uuid());
	else writeUInt16BE(data, uuid.get16Uuid());
}

void ByteDataHelper::writeStringToByteData(ByteData& data, const string& str)
{
	for (size_t i = 0; i < str.length(); ++i)
	{
		data.push_back(str[i]);
	}
}

void ByteDataHelper::writeHexStringToByteDataBE(ByteData& data, const string& str)
{
	if (str.length() % 2 != 0) return;
	for (size_t i = 0; i < str.length(); i += 2)
	{
		string t = str.substr(i, 2);
		unsigned int byteVal = stoul(t, NULL, 16);
		data.push_back(byteVal);
	}
}

void ByteDataHelper::writeHexStringToByteDataLE(ByteData& data, const string& str)
{
	if (str.length() % 2 != 0) return;
	for (size_t i = 0; i < str.length(); i += 2)
	{
		string t = str.substr(str.length() - i - 2, 2);
		unsigned int byteVal = stoul(t, NULL, 16);
		data.push_back(byteVal);
	}
}

string ByteDataHelper::byteDataToString(const ByteData& data)
{
	string ret;
	for (size_t i = 0; i < data.size(); ++i)
	{
		ret += data[i];
	}
	return ret;
}

string ByteDataHelper::byteDataToHexStringReverse(const ByteData& data, int pos)
{
	string ret;
	for (size_t i = pos; i < data.size(); ++i)
	{
		char t[3];
		sprintf(t, "%02x", data[pos + data.size() - 1 - i]);
		ret += t;
	}
	return ret;
}

string ByteDataHelper::byteDataToHexString(const ByteData& data, int pos)
{
	string ret;
	for (size_t i = pos; i < data.size(); ++i)
	{
		char t[3];
		sprintf(t, "%02x", data[i]);
		ret += t;
	}
	return ret;
}
