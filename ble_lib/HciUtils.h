#pragma once

#include <errno.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>


int hci_le_set_advertising_data(int dd, uint8_t* data, uint8_t length, int to);
int hci_le_set_scan_response_data(int dd, uint8_t* data, uint8_t length, int to);
int hci_le_set_advertising_parameters(int dd, int to);
int hci_enable_advertising(int dd, int to);
int hci_disable_advertising(int dd, int to);