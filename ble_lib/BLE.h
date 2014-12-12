#pragma once

#include <stdint.h>
#include <uuid/uuid.h>
#include <bluetooth/bluetooth.h>
#include "ByteDataHelper.h"

enum AdvCharacteristicType : uint8_t
{
	Flags = 0x01,
	IncompleteList16BitServiceClassUuids = 0x02,
	CompleteList16BitServiceClassUuids = 0x03,
	IncompleteList32BitServiceClassUuids = 0x04,
	CompleteList32BitServiceClassUuids = 0x05,
	IncompleteList128BitServiceClassUuids = 0x06,
	CompleteList128BitServiceClassUuids = 0x07,
	ShortenedLocalName = 0x08,
	CompleteLocalName = 0x09,
	TxPowerLevel = 0x0A,
	ClassOfDevice = 0x0D,
	SimplePairingHashC = 0x0E,
	SimplePairingHashC192 = 0x0E,
	SimplePairingRandomizerR = 0x0F,
	SimplePairingRandomizerR192 = 0x0F,
	DeviceID = 0x10,
	SecurityManagerTKValue = 0x10,
	SecurityManagerOutOfBandFlags = 0x11,
	SlaveConnectionIntervalRange = 0x12,
	ServiceSolicitationUuids16BitList = 0x14,
	ServiceSolicitationUuids32BitList = 0x1F,
	ServiceSolicitationUuids128BitList = 0x15,
	ServiceData = 0x16,
	ServiceData16BitUuid = 0x16,
	ServiceData32BitUuid = 0x20,
	ServiceData128BitUuid = 0x21,
	PublicTargetAddress = 0x17,
	RandomTargetAddress = 0x18,
	Appearance = 0x19,
	AdvertisingInterval = 0x1A,
	LEBluetoothDeviceAddress = 0x1B,
	LERole = 0x1C,
	SimplePairingHashC256 = 0x1D,
	SimplePairingRandomizerR256 = 0x1E,
	InformationData3D = 0x3D,
	ManufacturerSpecificData = 0xFF
};

enum AttOp : uint8_t
{
	ATT_OP_ERROR = 0x01,
	ATT_OP_MTU_REQ = 0x02,
	ATT_OP_MTU_RESP = 0x03,
	ATT_OP_FIND_INFO_REQ = 0x04,
	ATT_OP_FIND_INFO_RESP = 0x05,
	ATT_OP_FIND_BY_TYPE_REQ = 0x06,
	ATT_OP_FIND_BY_TYPE_RESP = 0x07,
	ATT_OP_READ_BY_TYPE_REQ = 0x08,
	ATT_OP_READ_BY_TYPE_RESP = 0x09,
	ATT_OP_READ_REQ = 0x0a,
	ATT_OP_READ_RESP = 0x0b,
	ATT_OP_READ_BLOB_REQ = 0x0c,
	ATT_OP_READ_BLOB_RESP = 0x0d,
	ATT_OP_READ_MULTI_REQ = 0x0e,
	ATT_OP_READ_MULTI_RESP = 0x0f,
	ATT_OP_READ_BY_GROUP_REQ = 0x10,
	ATT_OP_READ_BY_GROUP_RESP = 0x11,
	ATT_OP_WRITE_REQ = 0x12,
	ATT_OP_WRITE_RESP = 0x13,
	ATT_OP_WRITE_CMD = 0x52,
	ATT_OP_PREP_WRITE_REQ = 0x16,
	ATT_OP_PREP_WRITE_RESP = 0x17,
	ATT_OP_EXEC_WRITE_REQ = 0x18,
	ATT_OP_EXEC_WRITE_RESP = 0x19,
	ATT_OP_HANDLE_NOTIFY = 0x1b,
	ATT_OP_HANDLE_IND = 0x1d,
	ATT_OP_HANDLE_CNF = 0x1e,
	ATT_OP_SIGNED_WRITE_CMD = 0xd2
};

enum ErrECode : uint8_t
{
	ATT_ECODE_SUCCESS = 0x00,
	ATT_ECODE_INVALID_HANDLE = 0x01,
	ATT_ECODE_READ_NOT_PERM = 0x02,
	ATT_ECODE_WRITE_NOT_PERM = 0x03,
	ATT_ECODE_INVALID_PDU = 0x04,
	ATT_ECODE_AUTHENTICATION = 0x05,
	ATT_ECODE_REQ_NOT_SUPP = 0x06,
	ATT_ECODE_INVALID_OFFSET = 0x07,
	ATT_ECODE_AUTHORIZATION = 0x08,
	ATT_ECODE_PREP_QUEUE_FULL = 0x09,
	ATT_ECODE_ATTR_NOT_FOUND = 0x0a,
	ATT_ECODE_ATTR_NOT_LONG = 0x0b,
	ATT_ECODE_INSUFF_ENCR_KEY_SIZE = 0x0c,
	ATT_ECODE_INVAL_ATTR_VALUE_LEN = 0x0d,
	ATT_ECODE_UNLIKELY = 0x0e,
	ATT_ECODE_INSUFF_ENC = 0x0f,
	ATT_ECODE_UNSUPP_GRP_TYPE = 0x10,
	ATT_ECODE_INSUFF_RESOURCES = 0x11
};

enum GattSvc : uint16_t
{
	GENERIC_ACCESS_SVC_UUID = 0x1800,
	GENERIC_ATTRIBUTE_SVC_UUID = 0x1801,
	GATT_PRIMARY_SVC_UUID = 0x2800,
	GATT_SECONDARY_SVC_UUID = 0x2800,
	GATT_INCLUDE_UUID = 0x2802,
	GATT_CHARAC_UUID = 0x2803
};

enum GattCfgUuid : uint16_t
{
	GATT_CLIENT_CHARAC_CFG_UUID = 0x2902,
	GATT_SERVER_CHARAC_CFG_UUID = 0x2903
};

enum GattUuid : uint16_t
{
	ATTR_DEVICE_NAME_UUID = 0x2A00,
	ATTR_APPEARANCE_UUID = 0x2A01
};

enum GattProperties : uint8_t
{
	GATT_PROP_READ = 0x02,
	GATT_PROP_WRITE_WITHOUT_RESPONSE = 0x04,
	GATT_PROP_WRITE = 0x08,
	GATT_PROP_NOTIFY = 0x10
};

enum GapAdvFlag : uint8_t
{
	ADV_FLAG_LE_LIMITED_DISC_MODE = 0x01,
	ADV_FLAG_LE_GENERAL_DISC_MODE = 0x02,
	ADV_FLAG_BR_EDR_NOT_SUPPORTED = 0x04,
	ADV_FLAG_LE_BR_EDR_CONTROLLER = 0x08,
	ADV_FLAG_LE_BR_EDR_HOST = 0x10,
	ADV_FLAGS_LE_ONLY_LIMITED_DISC_MODE = ADV_FLAG_LE_LIMITED_DISC_MODE | ADV_FLAG_BR_EDR_NOT_SUPPORTED,
	ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE = ADV_FLAG_LE_GENERAL_DISC_MODE | ADV_FLAG_BR_EDR_NOT_SUPPORTED
};

// see https://developer.bluetooth.org/gatt/characteristics/Pages/CharacteristicViewer.aspx?u=org.bluetooth.characteristic.gap.appearance.xml
enum GapAppearance : uint16_t
{
	Unknown = 0,
	Generic_Phone = 64,
	Generic_Computer = 128,
	Generic_Watch = 192,
	Sports_Watch = 193,
	Generic_Clock = 256,
	Generic_Display = 320,
	Generic_Remote_Control = 384,
	Generic_Eye_glasses = 448,
	Generic_Tag = 512,
	Generic_Keyring = 576,
	Generic_Media_Player = 640,
	Generic_Barcode_Scanner = 704,
	Generic_Thermometer = 768,
	Thermometer_Ear = 769,
	Generic_Heart_rate_Sensor = 832,
	Heart_Rate_Belt = 833,
	Generic_Blood_Pressure = 896,
	Blood_Pressure_Arm = 897,
	Blood_Pressure_Wrist = 898,
	Human_Interface_Device = 960,
	Keyboard = 961,
	Mouse = 962,
	Joystick = 963,
	Gamepad = 964,
	Digitizer_Tablet = 965,
	Card_Reader = 966,
	Digital_Pen = 967,
	Barcode_Scanner = 968,
	Generic_Glucose_Meter = 1024,
	Generic_Running_Walking_Sensor = 1088,
	Running_Walking_Sensor_In_Shoe = 1089,
	Running_Walking_Sensor_On_Shoe = 1090,
	Running_Walking_Sensor_On_Hip = 1091,
	Generic_Cycling = 1152,
	Cycling_Cycling_Computer = 1153,
	Cycling_Speed_Sensor = 1154,
	Cycling_Cadence_Sensor = 1155,
	Cycling_Power_Sensor = 1156,
	Cycling_Speed_and_Cadence_Sensor = 1157,
	Generic_Pulse_Oximeter = 3136,
	Fingertip = 3137,
	Wrist_Worn = 3138,
	Generic_Weight_Scale = 3200,
	Generic_Outdoor_Sports_Activity = 5184,
	Location_Display_Device = 5185,
	Location_and_Navigation_Display_Device = 5186,
	Location_Pod = 5187,
	Location_and_Navigation_Pod = 5188,
};

#define ATT_CID 4

ByteData generateIBeaconData(const uuid_t& uuid, uint16_t major, uint16_t minor, uint8_t txPower);
