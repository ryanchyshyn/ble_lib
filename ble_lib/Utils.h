#pragma once
#include <string>
#include <bluetooth/bluetooth.h>
#include <uuid/uuid.h>

using namespace ::std;

string btAddressToString(const bdaddr_t& address);
string uuidToString(const uuid_t& data);
string formatUuid(string uuid);
