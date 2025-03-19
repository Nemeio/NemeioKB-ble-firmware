#include "device_list.h"
#include "app_scheduler.h"
#include "app_trace.h"

static bool m_b_device_data_store_done = false;
static pm_store_token_t m_device_data_store_token = 0;
static ret_code_t m_device_data_store_status = NRF_SUCCESS;

static void wait_for_store_done(void)
{
	for (;;) {
		// Wait in low power state for any events.
		uint32_t err_code = sd_app_evt_wait();
		APP_ERROR_CHECK(err_code);

		if (m_b_device_data_store_done) {
			// When operation has completed.
			return;
		}
	}
}

ret_code_t device_list_store_device_info(pm_peer_id_t peer_id, device_info_t *info_buff)
{
	ret_code_t err_code = NRF_SUCCESS;

	if (ALIGN_NUM(4, sizeof(device_info_t)) != sizeof(device_info_t)) {
		return NRF_ERROR_INVALID_PARAM;
	}

	m_device_data_store_token = 0;

	m_b_device_data_store_done = false;
	err_code = pm_peer_data_app_data_store(peer_id, (uint8_t *)info_buff, sizeof(device_info_t),
					       &m_device_data_store_token);

	return err_code;
}

static void store_device_name_scheduled_exec(void *p_event_data, uint16_t event_size)
{
	APP_ERROR_CHECK_BOOL(event_size == sizeof(device_list_store_device_name_evt));
	device_list_store_device_name_evt *p_evt =
		(device_list_store_device_name_evt *)p_event_data;

	device_list_store_device_info(p_evt->peer_id, &p_evt->device_info);
}

void device_list_store_device_info_sched(pm_peer_id_t peer_id, device_info_t *info_buff)
{
	device_list_store_device_name_evt evt = { .peer_id = peer_id };

	memcpy(&evt.device_info, info_buff, sizeof(device_info_t));
	app_sched_event_put(&evt, sizeof(evt), store_device_name_scheduled_exec);
}

uint32_t device_list_get_device_count()
{
	return pm_peer_count();
}

static pm_peer_id_t get_pm_peer_id(uint16_t peer_id)
{
	pm_peer_id_t pm_peer_id = PM_PEER_ID_INVALID;
	uint16_t nb_iter = peer_id;

	pm_peer_id = pm_next_peer_id_get(pm_peer_id);

	while ((pm_peer_id != PM_PEER_ID_INVALID) && (nb_iter > 0)) {
		pm_peer_id = pm_next_peer_id_get(pm_peer_id);
		nb_iter--;
	}

	return pm_peer_id;
}
ret_code_t device_list_get_device_info(uint16_t peer_id, device_info_t *ret_buff)
{
	ret_code_t err_code = NRF_SUCCESS;
	size_t len = sizeof(device_info_t);

	pm_peer_id_t pm_peer_id = get_pm_peer_id(peer_id);

	if (pm_peer_id != PM_PEER_ID_INVALID) {
		err_code = pm_peer_data_app_data_load(pm_peer_id, (uint8_t *)ret_buff, &len);
		if (err_code != NRF_SUCCESS || len != sizeof(device_info_t)) {
			err_code = NRF_ERROR_INVALID_DATA;
		}
	} else {
		err_code = NRF_ERROR_NOT_FOUND;
	}

	return err_code;
}

void device_list_pm_evt_handler(pm_evt_t const *p_evt)
{
	switch (p_evt->evt_id) {
	case PM_EVT_PEER_DATA_UPDATE_SUCCEEDED:
		if (!m_b_device_data_store_done &&
		    p_evt->params.peer_data_update_succeeded.token == m_device_data_store_token) {
			m_b_device_data_store_done = true;
			m_device_data_store_status = NRF_SUCCESS;
		}
		break;
	case PM_EVT_PEER_DATA_UPDATE_FAILED:
		if (p_evt->params.peer_data_update_failed.token == m_device_data_store_token) {
			m_b_device_data_store_done = true;
			m_device_data_store_status = p_evt->params.peer_data_update_failed.error;
		}
		break;
	default:
		break;
	}
}
ret_code_t device_list_delete_device(uint16_t peer_id)
{
	ret_code_t err_code = NRF_SUCCESS;

	pm_peer_id_t pm_peer_id = get_pm_peer_id(peer_id);

	if (pm_peer_id != PM_PEER_ID_INVALID) {
		err_code = pm_peer_delete(pm_peer_id);
	} else {
		err_code = NRF_ERROR_NOT_FOUND;
	}

	return err_code;
}