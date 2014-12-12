#pragma once

#include "BleHci.h"

class BleCentral : public BleHci
{
public:
	BleCentral();
	~BleCentral();

protected:
	virtual void onAdapterStateChanged(AdapterState state);
};

