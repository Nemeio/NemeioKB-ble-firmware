#ifndef DEVICE_LIST_H
#define DEVICE_LIST_H

#include "peer_manager.h"

#define DEVICE_NAME_MAX_LENGTH (32 + 1) // Device name maximum length including \0 char

typedef struct {
	char name_buff[DEVICE_NAME_MAX_LENGTH];
	uint8_t dummy[3]; // for device_info length 4-byte alignment
} __packed device_info_t;

typedef struct {
	pm_peer_id_t peer_id;
	uint16_t dummy; // for device_info address 4 byte-aligment
	device_info_t device_info;
} device_list_store_device_name_evt;

#define DEVICE_LIST_EVENT_DATA_SIZE sizeof(device_list_store_device_name_evt)

ret_code_t device_list_store_device_info(pm_peer_id_t peer_id, device_info_t *info_buff);
void device_list_store_device_info_sched(pm_peer_id_t peer_id, device_info_t *info_buff);
ret_code_t device_list_get_device_info(uint16_t peer_id, device_info_t *ret_buff);
ret_code_t device_list_delete_device(uint16_t peer_id);
uint32_t device_list_get_device_count();
void device_list_pm_evt_handler(pm_evt_t const *p_evt);

#endif // DEVICE_LIST_H
