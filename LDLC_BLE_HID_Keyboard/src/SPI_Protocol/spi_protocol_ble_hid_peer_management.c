#include "spi_protocol_ble_hid_peer_management.h"
#include "peer_manager.h"
#include "ble_management.h"

ret_code_t delete_whitelist_action(size_t data_size)
{
	delete_whitelist();
	//delete_bonds();
	//Clear the list of all stored address
	//pm_whitelist_set(NULL,0);

	return NRF_SUCCESS;
}

ret_code_t delete_bonds_action(size_t data_size)
{
	//delete_whitelist();
	delete_bonds();
	return NRF_SUCCESS;
}
