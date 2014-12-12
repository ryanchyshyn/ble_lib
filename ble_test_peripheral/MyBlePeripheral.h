#pragma once

#include "BlePeripheral.h"

class MyBlePeripheral : public BlePeripheral
{
public:
	MyBlePeripheral();
	~MyBlePeripheral();

protected:
	virtual void onHciError(int error);
	virtual void onAdapterStateChanged(AdapterState state);
	virtual void onAdapterStateError(int error);


	virtual void onL2CapError(int error);
	virtual void onSecurityLevelChanged(uint8_t securityLevel);
};

