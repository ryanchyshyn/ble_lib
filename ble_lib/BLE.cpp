#include "BLE.h"

ByteData generateIBeaconData(const uuid_t& uuid, uint16_t major, uint16_t minor, uint8_t txPower)
{
	ByteData ret;

	// prefix
	ByteDataHelper::writeByte(ret, 0x4C);
	ByteDataHelper::writeByte(ret, 0x00);
	ByteDataHelper::writeByte(ret, 0x02);
	ByteDataHelper::writeByte(ret, 0x15);

	// iBeacon uuid
	for (size_t i = 0; i < sizeof(uuid); ++i)
	{
		ByteDataHelper::writeByte(ret, uuid[i]);
	}

	// major
	ByteDataHelper::writeByte(ret, (major >> 8) & 0xFF);
	ByteDataHelper::writeByte(ret, major & 0xFF);

	// minor
	ByteDataHelper::writeByte(ret, (minor >> 8) & 0xFF);
	ByteDataHelper::writeByte(ret, minor & 0xFF);

	// tx power
	ByteDataHelper::writeByte(ret, txPower);

	return ret;
}

