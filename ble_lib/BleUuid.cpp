#include "BleUuid.h"
#include <stdio.h>

BleUuid::BleUuid() : _isUuid128(true)
{
	uuid_clear(_uuid.uuid128);
}

BleUuid::BleUuid(const BleUuid& other) : _isUuid128(other._isUuid128)
{
	uuid_copy(_uuid.uuid128, other._uuid.uuid128);
}

BleUuid::BleUuid(const uuid_t& uuid) : _isUuid128(true)
{
	uuid_copy(_uuid.uuid128, uuid);
}

BleUuid::BleUuid(const uint16_t& uuid) : _isUuid128(false)
{
	_uuid.uuid_16 = uuid;
}

bool BleUuid::operator<(const BleUuid& other) const
{
	if (_isUuid128 && other._isUuid128) return uuid_compare(_uuid.uuid128, other._uuid.uuid128) < 0;
	else if (_isUuid128 == other._isUuid128 /* both are false */) return (_uuid.uuid_16 - other._uuid.uuid_16) < 0;
	else if (!_isUuid128) return true;
	else return false;
}

bool BleUuid::operator==(const BleUuid& other) const
{
	if (_isUuid128 && other._isUuid128) return uuid_compare(_uuid.uuid128, other._uuid.uuid128) == 0;
	else if (_isUuid128 == other._isUuid128 /* both are false */) return _uuid.uuid_16 == other._uuid.uuid_16;
	return false;
}

bool BleUuid::operator==(const uuid_t& other) const
{
	if (!_isUuid128) return false;
	return uuid_compare(_uuid.uuid128, other) == 0;
}

bool BleUuid::operator==(const uint16_t& other) const
{
	if (_isUuid128) return false;
	return _uuid.uuid_16 == other;
}

bool BleUuid::operator!=(const uuid_t& other) const
{
	return !(operator==(other));
}

bool BleUuid::operator!=(const uint16_t& other) const
{
	return !(operator==(other));
}


BleUuid& BleUuid::operator=(const BleUuid& other)
{
	if (this != &other)
	{
		_isUuid128 = other._isUuid128;
		uuid_copy(_uuid.uuid128, other._uuid.uuid128);
	}
	return *this;
}

BleUuid& BleUuid::operator=(const uuid_t& other)
{
	_isUuid128 = true;
	uuid_copy(_uuid.uuid128, other);
	return *this;
}

BleUuid& BleUuid::operator=(const uint16_t& other)
{
	_isUuid128 = false;
	_uuid.uuid_16 = other;
	return *this;
}

void BleUuid::generateInplace()
{
	_isUuid128 = true;
	uuid_generate(_uuid.uuid128);
}

BleUuid BleUuid::generate()
{
	BleUuid ret;
	ret.generateInplace();
	return ret;
}

string BleUuid::toString()
{
	if (_isUuid128)
	{
		char ret[37];
		uuid_unparse(_uuid.uuid128, ret);
		return ret;
	}
	else
	{
		char ret[5];
		sprintf(ret, "%02x%02x", _uuid.uuid_16 & 0xFF, (_uuid.uuid_16 > 8) & 0xFF);
		return ret;
	}
}

size_t BleUuid::getLength() const
{
	if (_isUuid128) return 16;
	else return 2;
}

bool BleUuid::isUuid128() const
{
	return _isUuid128;
}

const uuid_t& BleUuid::get128Uuid() const
{
	return _uuid.uuid128;
}

const uint16_t& BleUuid::get16Uuid() const
{
	return _uuid.uuid_16;
}
