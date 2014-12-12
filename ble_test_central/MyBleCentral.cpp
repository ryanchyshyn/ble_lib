#include "MyBleCentral.h"


MyBleCentral::MyBleCentral()
{
}

MyBleCentral::~MyBleCentral()
{
}

void MyBleCentral::onHciError(int error)
{
	printf("onHciError: %d\n", error);
}

void MyBleCentral::onAdapterStateChanged(AdapterState state)
{
	BleCentral::onAdapterStateChanged(state);
	printf("onAdapterStateChanged: %d\n", state);
}

void MyBleCentral::onAdapterStateError(int error)
{
	printf("onAdapterStateError: %d\n", error);
}

void MyBleCentral::onScanFinished(shared_ptr<BleScanNode> node)
{
	string addrStr = btAddressToString(node->getAddress());
	printf("MAC: %s\n", addrStr.c_str());
	printf("is public: %s\n", node->getIsPublic() ? "true" : "false");
	printf("RSSI: %d\n", node->getRssi());
	printf("TX power: %d\n", node->getTxPower());
	printf("Name: %s\n", node->getName().c_str());
	if (node->isIBeacon())
	{
		printf("iBeacon UUID: %s\n", node->getIBeaconUuid().c_str());
		printf("iBeacon major:minor %d:%d\n", node->getMajor(), node->getMinor());
		printf("iBeacon TX power: %d\n", node->getIBeaconTxPower());
	}
}
