#include "BleCentral.h"


BleCentral::BleCentral()
{
}


BleCentral::~BleCentral()
{
}

void BleCentral::onAdapterStateChanged(AdapterState state)
{
	if (state == Enabled)
	{
		enableScanning(false);
	}
}
