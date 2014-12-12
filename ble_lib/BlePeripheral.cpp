#include "BlePeripheral.h"
#include "ParseBuffer.h"
#include "OutOfRangeException.h"


BlePeripheral::BlePeripheral()
{
}

BlePeripheral::~BlePeripheral()
{
	cleanup();
}

int BlePeripheral::init()
{
	int ret = BleHci::init();
	if (ret != 0)
	{
		cleanup();
		return ret;
	}
	ret = BleL2Cap::init();
	if (ret != 0)
	{
		cleanup();
		return ret;
	}
	return 0;
}

void BlePeripheral::cleanup()
{
	BleHci::cleanup();
	BleL2Cap::cleanup();
}

void BlePeripheral::setAdvertisementNode(shared_ptr<BleAdvNode>& node)
{
	advNode = node;
}

int BlePeripheral::setAdvertisementParams()
{
	return setAdvertizingParams(advNode->getAdvertisingData(), advNode->getScanningData());
}

void BlePeripheral::onScanFinished(shared_ptr<BleScanNode> node)
{
	// dummy
}

void BlePeripheral::onAccept(const bdaddr_t& address)
{
	printf("L2CAP onAccept\n");
}

void BlePeripheral::onDisconnected(const bdaddr_t& address)
{
	printf("L2CAP onDisconnected\n");
}

void BlePeripheral::onWriteDone(const ByteData& data)
{
	// dummy
}

void BlePeripheral::handleMtuRequest(const ByteData& data)
{
	printf("handleMtuRequest\n");

	AttOp requestType;
	try
	{
		ParseBuffer buffer(data);

		requestType = (AttOp)(buffer.readUInt8());
		uint16_t mtu = buffer.readUInt16LE();

		if (mtu < 23) mtu = 23;
		else if (mtu > 256) mtu = 256;

		ByteData resp;
		ByteDataHelper::writeByte(resp, AttOp::ATT_OP_MTU_RESP);
		ByteDataHelper::writeUInt16BE(resp, mtu);
		writeL2CapData(resp);
	}
	catch (OutOfRangeException& e)
	{
		writeL2CapError(requestType, 0, ErrECode::ATT_ECODE_INSUFF_RESOURCES);
	}
}

void BlePeripheral::handleFindInfoRequest(const ByteData& data)
{
	printf("handleFindInfoRequest\n");

	AttOp requestType;
	try
	{
		ParseBuffer buffer(data);

		requestType = (AttOp)(buffer.readUInt8());
		uint16_t startHandle = buffer.readUInt16LE();
		uint16_t endHandle = buffer.readUInt16LE();

		if ((startHandle > endHandle) || (startHandle == 0))
		{
			writeL2CapError(requestType, startHandle, ErrECode::ATT_ECODE_INVALID_HANDLE);
			return;
		}

		vector<shared_ptr<BleBase>>& handles = advNode->getHandles();

		ByteData resp;
		bool is128uuid = false;
		for (uint16_t i = startHandle; (i <= endHandle) && (i <= handles.size()); ++i)
		{
			BleBase* base = handles[i - 1].get();

			BleUuid retUuid;

			if (dynamic_cast<BleService*>(base)) retUuid = GattSvc::GATT_PRIMARY_SVC_UUID;
			//else if ('includedService' == = handle.type) get16uuidBE(GattSvc::GATT_INCLUDE_UUID);
			else if (dynamic_cast<BleCharacteristics*>(base)) retUuid = GattSvc::GATT_CHARAC_UUID;
			else if (dynamic_cast<BleValue*>(base)) retUuid = handles[i - 2]->uuid;
			else if (dynamic_cast<BleDescription*>(base)) retUuid = base->uuid;

			if (i == startHandle)
			{
				is128uuid = retUuid.isUuid128();
				ByteDataHelper::writeByte(resp, AttOp::ATT_OP_FIND_INFO_RESP);
				ByteDataHelper::writeByte(resp, is128uuid ? 0x02 : 0x01);
			}

			if (retUuid.isUuid128() != is128uuid) break;

			ByteDataHelper::writeUInt16BE(resp, i);
			ByteDataHelper::writeUuidBE(resp, retUuid);
		}

		if (resp.size() != 0) writeL2CapData(resp);
		else writeL2CapError(requestType, startHandle, ErrECode::ATT_ECODE_ATTR_NOT_FOUND);
	}
	catch (OutOfRangeException& e)
	{
		writeL2CapError(requestType, 0, ErrECode::ATT_ECODE_INSUFF_RESOURCES);
	}
}

void BlePeripheral::handleFindByTypeRequest(const ByteData& data)
{
	printf("handleFindByTypeRequest\n");

	AttOp requestType;
	try
	{
		ParseBuffer buffer(data);

		requestType = (AttOp)(buffer.readUInt8());
		uint16_t startHandle = buffer.readUInt16LE();
		uint16_t endHandle = buffer.readUInt16LE();

		if ((startHandle > endHandle) || (startHandle == 0))
		{
			writeL2CapError(requestType, startHandle, ErrECode::ATT_ECODE_INVALID_HANDLE);
			return;
		}

		uint16_t svcUuid = buffer.readUInt16LE();
		BleUuid uuidValue;
		if (buffer.bytesAvailable() == 2) uuidValue = buffer.readUInt16LE();
		else if (buffer.bytesAvailable() == 16) uuidValue = buffer.readUuidLE();
		else
		{
			writeL2CapError(requestType, startHandle, ErrECode::ATT_ECODE_INVALID_PDU);
			return;
		}

		if (svcUuid != GattSvc::GATT_PRIMARY_SVC_UUID)
		{
			writeL2CapError(requestType, startHandle, ErrECode::ATT_ECODE_ATTR_NOT_FOUND);
			return;
		}

		vector<shared_ptr<BleBase>>& handles = advNode->getHandles();

		ByteData resp;
		for (uint16_t i = startHandle; (i <= endHandle) && (i <= handles.size()); ++i)
		{
			BleBase* base = handles[i - 1].get();

			if (dynamic_cast<BleService*>(base))
			{
				BleService* base2 = dynamic_cast<BleService*>(base);
				if (i == startHandle)
				{
					ByteDataHelper::writeByte(resp, AttOp::ATT_OP_FIND_BY_TYPE_RESP);
				}

				if (base->uuid == uuidValue)
				{
					ByteDataHelper::writeUInt16BE(resp, base2->startHandle);
					ByteDataHelper::writeUInt16BE(resp, base2->endHandle);
				}
			}
		}

		if (resp.size() != 0) writeL2CapData(resp);
		else writeL2CapError(requestType, startHandle, ErrECode::ATT_ECODE_UNSUPP_GRP_TYPE);
	}
	catch (OutOfRangeException& e)
	{
		writeL2CapError(requestType, 0, ErrECode::ATT_ECODE_INSUFF_RESOURCES);
	}
}

void BlePeripheral::handleReadByTypeRequest(const ByteData& data)
{
	printf("handleReadByTypeRequest\n");

	AttOp requestType;
	try
	{
		ParseBuffer buffer(data);

		requestType = (AttOp)(buffer.readUInt8());
		uint16_t startHandle = buffer.readUInt16LE();
		uint16_t endHandle = buffer.readUInt16LE();

		if ((startHandle > endHandle) || (startHandle == 0))
		{
			writeL2CapError(requestType, startHandle, ErrECode::ATT_ECODE_INVALID_HANDLE);
			return;
		}

		vector<shared_ptr<BleBase>>& handles = advNode->getHandles();

		BleUuid svcUuid;
		if (buffer.bytesAvailable() == 2) svcUuid = buffer.readUInt16LE();
		else if (buffer.bytesAvailable() == 16) svcUuid = buffer.readUuidLE();
		else
		{
			writeL2CapError(requestType, startHandle, ErrECode::ATT_ECODE_INVALID_PDU);
			return;
		}

		if (svcUuid == GattSvc::GATT_CHARAC_UUID)
		{
			ByteData resp;

			bool first = true;
			bool is128uuid = false;
			for (uint16_t i = startHandle; (i <= endHandle) && (i <= handles.size()); ++i)
			{
				BleBase* base = handles[i - 1].get();

				if (dynamic_cast<BleCharacteristics*>(base))
				{
					BleCharacteristics* base2 = dynamic_cast<BleCharacteristics*>(base);
					if (first)
					{
						is128uuid = base->uuid.isUuid128();
						ByteDataHelper::writeByte(resp, AttOp::ATT_OP_READ_BY_TYPE_RESP);
						ByteDataHelper::writeByte(resp, is128uuid ? 21 : 7);
						first = false;
					}

					if (base->uuid.isUuid128() != is128uuid) break;

					ByteDataHelper::writeUInt16BE(resp, base2->startHandle);
					ByteDataHelper::writeByte(resp, base2->properties);
					ByteDataHelper::writeUInt16BE(resp, base2->valueHandle);
					ByteDataHelper::writeUuidBE(resp, base2->uuid);
				}
			}

			if (resp.size() != 0) writeL2CapData(resp);
			else writeL2CapError(requestType, startHandle, ErrECode::ATT_ECODE_ATTR_NOT_FOUND);
		}
		else //if (svcUuid != GattUuid::ATTR_DEVICE_NAME_UUID)
		{
			ByteData resp;

			for (uint16_t i = startHandle; (i <= endHandle) && (i <= handles.size()); ++i)
			{
				BleBase* base = handles[i - 1].get();

				if (base->uuid == svcUuid)
				{
					if (dynamic_cast<BleCharacteristics*>(base))
					{
						BleCharacteristics* base2 = dynamic_cast<BleCharacteristics*>(base);
						ByteData value = base2->value->read();

						ByteDataHelper::writeByte(resp, AttOp::ATT_OP_READ_BY_TYPE_RESP);
						ByteDataHelper::writeByte(resp, data.size() + 2);
						ByteDataHelper::writeUInt16BE(resp, base2->valueHandle);
						resp.insert(resp.end(), value.begin(), value.end());
					}
					else if (dynamic_cast<BleDescription*>(base))
					{
						BleDescription* base2 = dynamic_cast<BleDescription*>(base);
						const ByteData& value = base2->value;

						ByteDataHelper::writeByte(resp, AttOp::ATT_OP_READ_BY_TYPE_RESP);
						ByteDataHelper::writeByte(resp, data.size() + 2);
						ByteDataHelper::writeUInt16BE(resp, i);
						resp.insert(resp.end(), value.begin(), value.end());
					}
				}
			}

			if (resp.size() != 0) writeL2CapData(resp);
			else writeL2CapError(requestType, startHandle, ErrECode::ATT_ECODE_ATTR_NOT_FOUND);
		}
		//else
		//{
		//	writeL2CapError(requestType, startHandle, ErrECode::ATT_ECODE_ATTR_NOT_FOUND);
		//}
	}
	catch (OutOfRangeException& e)
	{
		writeL2CapError(requestType, 0, ErrECode::ATT_ECODE_INSUFF_RESOURCES);
	}
}

void BlePeripheral::handleReadOrReadBlobRequest(const ByteData& data)
{
	printf("handleReadOrReadBlobRequest\n");

	AttOp requestType;
	try
	{
		ParseBuffer buffer(data);

		requestType = (AttOp)(buffer.readUInt8());
		uint16_t handle = buffer.readUInt16LE();
		int offset = (requestType == AttOp::ATT_OP_READ_BLOB_REQ) ? buffer.readUInt16LE() : 0;

		vector<shared_ptr<BleBase>>& handles = advNode->getHandles();

		if ((handle == 0) || (handle > handles.size()))
		{
			writeL2CapError(requestType, handle, ErrECode::ATT_ECODE_INVALID_HANDLE);
			return;
		}

		BleBase* base = handles[handle - 1].get();
		ByteData resp;
		if (dynamic_cast<BleService*>(base))
		{
			ByteDataHelper::writeByte(resp, (requestType == AttOp::ATT_OP_READ_BLOB_REQ) ? AttOp::ATT_OP_READ_BLOB_RESP : AttOp::ATT_OP_READ_RESP);
			ByteDataHelper::writeUuidBE(resp, base->uuid);
		}
		else if (dynamic_cast<BleCharacteristics*>(base))
		{
			BleCharacteristics* base2 = dynamic_cast<BleCharacteristics*>(base);
			ByteDataHelper::writeByte(resp, (requestType == AttOp::ATT_OP_READ_BLOB_REQ) ? AttOp::ATT_OP_READ_BLOB_RESP : AttOp::ATT_OP_READ_RESP);
			ByteDataHelper::writeByte(resp, base2->properties);
			ByteDataHelper::writeUInt16BE(resp, handle);
			ByteDataHelper::writeUuidBE(resp, base->uuid);
		}
		else if (dynamic_cast<BleValue*>(base))
		{
			BleValue* base2 = dynamic_cast<BleValue*>(base);
			ByteDataHelper::writeByte(resp, (requestType == AttOp::ATT_OP_READ_BLOB_REQ) ? AttOp::ATT_OP_READ_BLOB_RESP : AttOp::ATT_OP_READ_RESP);
			ByteData value = base2->read();
			resp.insert(resp.end(), value.begin(), value.end());
		}
		else if (dynamic_cast<BleDescription*>(base))
		{
			BleDescription* base2 = dynamic_cast<BleDescription*>(base);
			ByteDataHelper::writeByte(resp, (requestType == AttOp::ATT_OP_READ_BLOB_REQ) ? AttOp::ATT_OP_READ_BLOB_RESP : AttOp::ATT_OP_READ_RESP);
			resp.insert(resp.end(), base2->value.begin(), base2->value.end());
		}

		if (resp.size() != 0) writeL2CapData(resp);
		else writeL2CapError(requestType, handle, ErrECode::ATT_ECODE_INVALID_HANDLE);
	}
	catch (OutOfRangeException& e)
	{
		writeL2CapError(requestType, 0, ErrECode::ATT_ECODE_INSUFF_RESOURCES);
	}
}

void BlePeripheral::handleReadByGroupRequest(const ByteData& data)
{
	printf("handleReadByGroupRequest\n");

	AttOp requestType;
	try
	{
		ParseBuffer buffer(data);

		requestType = (AttOp)(buffer.readUInt8());
		uint16_t startHandle = buffer.readUInt16LE();
		uint16_t endHandle = buffer.readUInt16LE();

		if (buffer.bytesAvailable() != 2)
		{
			writeL2CapError(requestType, startHandle, ErrECode::ATT_ECODE_INVALID_PDU);
			return;
		}

		BleUuid uuid = buffer.readUInt16LE();
		if ((uuid != GattSvc::GATT_PRIMARY_SVC_UUID) && (uuid != GattSvc::GATT_INCLUDE_UUID))
		{
			writeL2CapError(requestType, startHandle, ErrECode::ATT_ECODE_ATTR_NOT_FOUND);
			return;
		}

		if ((startHandle > endHandle) || (startHandle == 0))
		{
			writeL2CapError(requestType, startHandle, ErrECode::ATT_ECODE_INVALID_HANDLE);
			return;
		}

		vector<shared_ptr<BleBase>>& handles = advNode->getHandles();

		ByteData resp;
		bool is128uuid = false;
		for (uint16_t i = startHandle; (i <= endHandle) && (i <= handles.size()); ++i)
		{
			BleBase* base = handles[i - 1].get();

			if (dynamic_cast<BleService*>(base))
			{
				BleService* base2 = dynamic_cast<BleService*>(base);
				if (i == startHandle)
				{
					is128uuid = base->uuid.isUuid128();
					ByteDataHelper::writeByte(resp, AttOp::ATT_OP_READ_BY_GROUP_RESP);
					ByteDataHelper::writeByte(resp, is128uuid ? 20 : 6);
				}

				if (base->uuid.isUuid128() != is128uuid) break;

				ByteDataHelper::writeUInt16BE(resp, base2->startHandle);
				ByteDataHelper::writeUInt16BE(resp, base2->endHandle);
				ByteDataHelper::writeUuidBE(resp, base2->uuid);
			}
		}

		if (resp.size() != 0) writeL2CapData(resp);
		else writeL2CapError(requestType, startHandle, ErrECode::ATT_ECODE_ATTR_NOT_FOUND);
	}
	catch (OutOfRangeException& e)
	{
		writeL2CapError(requestType, 0, ErrECode::ATT_ECODE_INSUFF_RESOURCES);
	}
}

void BlePeripheral::handleWriteRequestOrCommand(const ByteData& data)
{
	printf("handleWriteRequestOrCommand\n");

	AttOp requestType;
	try
	{
		ParseBuffer buffer(data);

		requestType = (AttOp)(buffer.readUInt8());
		uint16_t handle = buffer.readUInt16LE();

		bool withoutResponse = (requestType == AttOp::ATT_OP_WRITE_CMD);

		vector<shared_ptr<BleBase>>& handles = advNode->getHandles();

		if ((handle == 0) || (handle > handles.size()))
		{
			writeL2CapError(requestType, handle, ErrECode::ATT_ECODE_INVALID_HANDLE);
			return;
		}

		BleBase* base = handles[handle - 1].get();
		ByteData resp;
		if (dynamic_cast<BleService*>(base))
		{
		}
		else if (dynamic_cast<BleCharacteristics*>(base))
		{
		}
		else if (dynamic_cast<BleValue*>(base))
		{
			BleValue* base2 = dynamic_cast<BleValue*>(base);
			ByteData opData = buffer.readBytes();
			base2->write(opData);
			ByteDataHelper::writeByte(resp, AttOp::ATT_OP_WRITE_RESP);
		}
		else if (dynamic_cast<BleDescription*>(base))
		{
		}

		if (resp.size() != 0) writeL2CapData(resp);
		else writeL2CapError(requestType, handle, ErrECode::ATT_ECODE_INVALID_HANDLE);
	}
	catch (OutOfRangeException& e)
	{
		writeL2CapError(requestType, 0, ErrECode::ATT_ECODE_INSUFF_RESOURCES);
	}
}

void BlePeripheral::handleOther(const ByteData& data)
{
	printf("handleOther\n");
}
