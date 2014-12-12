#pragma once

#include <stdint.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <memory>
#include <string>

#include "BLE.h"
#include "BleScanNode.h"

#ifdef LOG_HCI
#define HCI_LOG_ERROR(error, str) fprintf(stderr, "HCI error: %d (%s). %s\n", error, strerror(error), str)
#define HCI_LOG_DEBUG(fmt, ...) do { printf("HCI debug: " fmt "\n", __VA_ARGS__); } while (0)
#else 
#define HCI_LOG_ERROR(error, str)
#define HCI_LOG_DEBUG(fmt, ...)
#endif

using namespace std;

enum AdapterState
{
	Disabled,
	Enabled,
	Unauthorised,
	Unsupported
};


class BleHci
{
private:
	static const __suseconds_t SELECT_TIME = 200000; // in microseconds

	pthread_t threadId;
	volatile bool exitThread;

	volatile bool scanningEnabled;

	volatile int hciSocket;
	hci_filter orgFilter;
	bool orgFilterSaved;

	hci_dev_info hciDevInfo;
	volatile int prevAdapterState;

	bool advertisingEnabled;

	static void* threadFunc(void *arg);

public:
	BleHci();
	virtual ~BleHci();

	int init();
	void cleanup();
	bool isInitialized();

	int enableScanning(bool filterDuplicates);
	int disableScanning();
	bool isScanningEnabled();

	int enableAdvertising();
	int setAdvertizingParams(const ByteData& advData, const ByteData& scanData);
	int disableAdvertising();
	bool isAdvertisingEnabled();

private:
	int createSocket();
	void stopThread();
	int setFilter();
	int startThread();
	int selectDevice();
	void closeSocket();
	void checkAdapterState();
	void run();
	void processLeAdvData(const bdaddr_t& btAddress, bool isPublic, uint8_t* eirData, int eirDataLen, int rssi);
	
protected:
	virtual void onHciError(int error) = 0;
	virtual void onAdapterStateChanged(AdapterState state) = 0;
	virtual void onAdapterStateError(int error) = 0;
	virtual void onScanFinished(shared_ptr<BleScanNode> node) = 0;
};

