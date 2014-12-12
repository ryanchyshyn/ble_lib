#include "BleService.h"

void BleService::addCharacteristics(shared_ptr<BleCharacteristics>& characteristics)
{
	characteristicsList[characteristics->uuid] = characteristics;
}