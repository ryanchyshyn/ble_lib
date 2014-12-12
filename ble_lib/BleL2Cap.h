#pragma once
#include <list>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/l2cap.h>
#include <pthread.h>
#include "ByteDataHelper.h"
#include "BLE.h"

#ifdef LOG_L2CAP
#define L2CAP_LOG_ERROR(error, str) fprintf(stderr, "L2CAP error: %d (%s). %s\n", error, strerror(error), str)
#define L2CAP_LOG_DEBUG(fmt, ...) do { printf("L2CAP debug: " fmt "\n", __VA_ARGS__); } while (0)
#else 
#define L2CAP_LOG_ERROR(error, str)
#define L2CAP_LOG_DEBUG(fmt, ...)
#endif

#define L2CAP_READ_BUFFER_SIZE 1024

class BleL2Cap
{
private:
	pthread_t threadId;
	volatile bool exitThread;
	
	volatile int listenSocket;
	volatile int clientSocket;
	struct sockaddr_l2 sockAddr;
	list<ByteData> writeData; // TODO make thread safe

	static void* threadFunc(void *arg);

public:
	BleL2Cap();
	~BleL2Cap();

	int init();
	void cleanup();
	bool isInitialized();

private:
	void closeSockets();
	void run();
	void onDataReceived(const ByteData& data);

protected:
	void writeL2CapData(const ByteData& data);
	void writeL2CapError(AttOp opcode, uint16_t handle, ErrECode status);

	virtual void onL2CapError(int error) = 0;
	virtual void onAccept(const bdaddr_t& address) = 0;
	virtual void onDisconnected(const bdaddr_t& address) = 0;
	virtual void onSecurityLevelChanged(uint8_t securityLevel) = 0;
	virtual void onWriteDone(const ByteData& data) = 0;

	virtual void handleMtuRequest(const ByteData& data) = 0;
	virtual void handleFindInfoRequest(const ByteData& data) = 0;
	virtual void handleFindByTypeRequest(const ByteData& data) = 0;
	virtual void handleReadByTypeRequest(const ByteData& data) = 0;
	virtual void handleReadOrReadBlobRequest(const ByteData& data) = 0;
	virtual void handleReadByGroupRequest(const ByteData& data) = 0;
	virtual void handleWriteRequestOrCommand(const ByteData& data) = 0;
	virtual void handleOther(const ByteData& data) = 0;
};