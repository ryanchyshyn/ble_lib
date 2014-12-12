#include "HciUtils.h"

int hci_le_set_advertising_data(int dd, uint8_t* data, uint8_t length, int to)
{
	struct hci_request rq;
	le_set_advertising_data_cp data_cp;
	uint8_t status;

	memset(&data_cp, 0, sizeof(data_cp));
	data_cp.length = length;
	memcpy(&data_cp.data, data, sizeof(data_cp.data));

	memset(&rq, 0, sizeof(rq));
	rq.ogf = OGF_LE_CTL;
	rq.ocf = OCF_LE_SET_ADVERTISING_DATA;
	rq.cparam = &data_cp;
	rq.clen = LE_SET_ADVERTISING_DATA_CP_SIZE;
	rq.rparam = &status;
	rq.rlen = 1;

	return hci_send_req(dd, &rq, to);
}

int hci_le_set_scan_response_data(int dd, uint8_t* data, uint8_t length, int to)
{
	struct hci_request rq;
	le_set_scan_response_data_cp data_cp;
	uint8_t status;

	memset(&data_cp, 0, sizeof(data_cp));
	data_cp.length = length;
	memcpy(&data_cp.data, data, sizeof(data_cp.data));

	memset(&rq, 0, sizeof(rq));
	rq.ogf = OGF_LE_CTL;
	rq.ocf = OCF_LE_SET_SCAN_RESPONSE_DATA;
	rq.cparam = &data_cp;
	rq.clen = LE_SET_SCAN_RESPONSE_DATA_CP_SIZE;
	rq.rparam = &status;
	rq.rlen = 1;

	return hci_send_req(dd, &rq, to);
}

// Set advertising interval to 100 ms
// Note: 0x00A0 * 0.625ms = 100ms
int hci_le_set_advertising_parameters(int dd, int to)
{
	struct hci_request rq;
	uint8_t status;
	int ret;

	le_set_advertising_parameters_cp adv_params_cp;
	memset(&adv_params_cp, 0, sizeof(adv_params_cp));
	adv_params_cp.min_interval = htobs(0x00A0);
	adv_params_cp.max_interval = htobs(0x00A0);
	adv_params_cp.chan_map = 7;

	memset(&rq, 0, sizeof(rq));
	rq.ogf = OGF_LE_CTL;
	rq.ocf = OCF_LE_SET_ADVERTISING_PARAMETERS;
	rq.cparam = &adv_params_cp;
	rq.clen = LE_SET_ADVERTISING_PARAMETERS_CP_SIZE;
	rq.rparam = &status;
	rq.rlen = 1;

	ret = hci_send_req(dd, &rq, to);
	if (ret < 0) return ret;

	/*if (status)
	{
		errno = EIO;
		return -1;
	}*/

	return 0;
	
}

int hci_enable_advertising(int dd, int to)
{
	struct hci_request rq;
	uint8_t status;
	int ret;

	le_set_advertise_enable_cp advertise_cp;
	memset(&advertise_cp, 0, sizeof(advertise_cp));
	advertise_cp.enable = 0x01;

	memset(&rq, 0, sizeof(rq));
	rq.ogf = OGF_LE_CTL;
	rq.ocf = OCF_LE_SET_ADVERTISE_ENABLE;
	rq.cparam = &advertise_cp;
	rq.clen = LE_SET_ADVERTISE_ENABLE_CP_SIZE;
	rq.rparam = &status;
	rq.rlen = 1;

	ret = hci_send_req(dd, &rq, to);
	if (ret < 0) return ret;

	if (status)
	{
		errno = EIO;
		return -1;
	}
}

int hci_disable_advertising(int dd, int to)
{
	struct hci_request rq;
	le_set_advertise_enable_cp advertise_cp;
	uint8_t status;

	memset(&advertise_cp, 0, sizeof(advertise_cp));

	memset(&rq, 0, sizeof(rq));
	rq.ogf = OGF_LE_CTL;
	rq.ocf = OCF_LE_SET_ADVERTISE_ENABLE;
	rq.cparam = &advertise_cp;
	rq.clen = LE_SET_ADVERTISE_ENABLE_CP_SIZE;
	rq.rparam = &status;
	rq.rlen = 1;

	int ret = hci_send_req(dd, &rq, to);
	if (ret < 0) return ret;

	if (status)
	{
		errno = EIO;
		return -1;
	}
}
