#include "Utils.h"
#include <string>

using namespace ::std;

string btAddressToString(const bdaddr_t& address)
{
	char buffer[18];
	ba2str(&address, buffer);
	return string(buffer);
}

string uuidToString(const uuid_t& data)
{
	const unsigned char* t = (const unsigned char*)data;
	char ret[33];
	sprintf(ret, "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x", t[0], t[1], t[2], t[3], t[4], t[5], t[6], t[7], t[8], t[9], t[10], t[11], t[12], t[13], t[14], t[15]);
	return ret;
}

string formatUuid(string uuid)
{
	if (uuid.length() != 32) return uuid;
	uuid.insert(8, "-");
	uuid.insert(13, "-");
	uuid.insert(18, "-");
	uuid.insert(23, "-");
	return uuid;
}