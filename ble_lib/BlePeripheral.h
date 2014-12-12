#pragma once

#include <memory>
#include "BleHci.h"
#include "BleL2Cap.h"
#include "BleAdvNode.h"

class BlePeripheral : public BleHci, public BleL2Cap
{
private:
	shared_ptr<BleAdvNode> advNode;

public:
	BlePeripheral();
	~BlePeripheral();

	int init();
	void cleanup();

	void setAdvertisementNode(shared_ptr<BleAdvNode>& node);
	int setAdvertisementParams();

protected:
	virtual void onScanFinished(shared_ptr<BleScanNode> node);


	virtual void onAccept(const bdaddr_t& address);
	virtual void onDisconnected(const bdaddr_t& address);
	virtual void onWriteDone(const ByteData& data);

	virtual void handleMtuRequest(const ByteData& data);
	virtual void handleFindInfoRequest(const ByteData& data);
	virtual void handleFindByTypeRequest(const ByteData& data);
	virtual void handleReadByTypeRequest(const ByteData& data);
	virtual void handleReadOrReadBlobRequest(const ByteData& data);
	virtual void handleReadByGroupRequest(const ByteData& data);
	virtual void handleWriteRequestOrCommand(const ByteData& data);
	virtual void handleOther(const ByteData& data);
};

