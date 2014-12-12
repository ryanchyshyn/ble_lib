#pragma once
#include "BleCentral.h"

class MyBleCentral : public BleCentral
{
public:
	MyBleCentral();
	~MyBleCentral();

protected:
	virtual void onHciError(int error);
	virtual void onAdapterStateChanged(AdapterState state);
	virtual void onAdapterStateError(int error);
	virtual void onScanFinished(shared_ptr<BleScanNode> node);
};

