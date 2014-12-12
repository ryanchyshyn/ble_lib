#pragma once

#include <uuid/uuid.h>
#include <stdint.h>
#include <string>

using namespace ::std;

union Uuid16_128
{
	uuid_t uuid128;
	uint16_t uuid_16;
};

class BleUuid
{
private:
	bool _isUuid128;
	Uuid16_128 _uuid;

public:
	BleUuid();
	BleUuid(const BleUuid& other);
	BleUuid(const uuid_t& uuid);
	BleUuid(const uint16_t& uuid);

	bool operator<(const BleUuid& other) const;
	bool operator==(const BleUuid& other) const;
	bool operator==(const uuid_t& other) const;
	bool operator==(const uint16_t& other) const;
	bool operator!=(const uuid_t& other) const;
	bool operator!=(const uint16_t& other) const;

	BleUuid& operator=(const BleUuid& other);
	BleUuid& operator=(const uuid_t& other);
	BleUuid& operator=(const uint16_t& other);

	void generateInplace();
	static BleUuid generate();
	string toString();
	size_t getLength() const;
	bool isUuid128() const;
	const uuid_t& get128Uuid() const;
	const uint16_t& get16Uuid() const;
};