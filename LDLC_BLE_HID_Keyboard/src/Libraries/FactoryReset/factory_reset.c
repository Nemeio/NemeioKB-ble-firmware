#include "factory_reset.h"

#include "ble_management.h"

ret_code_t factory_reset()
{
	delete_whitelist();
	delete_bonds();

	return NRF_SUCCESS;
}