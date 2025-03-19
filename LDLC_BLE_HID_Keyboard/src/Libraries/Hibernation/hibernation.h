/* Copyright (c) 2016 Adeneo Embedded. All Rights Reserved.
 *
 */

/**@file
 *
 * @defgroup Hibernation Hibernation module
 * @{
 * @brief API to set the device to hibernation mode.
 *
 */

#ifndef HIBERNATION_H
#define HIBERNATION_H

/**@brief Function for initializing the device for hibernation.
 *
 * @details This function configures a button as wake-up source
 *
 */
void hibernation_init();

/**@brief Function for setting the device to hibernation mode.
 */
void hibernation_power_off();

/**@brief Function for handling a scheduled hibernate task.
 */
void hibernation_power_off_evt_handler(void *p_data, uint16_t length);

#endif // HIBERNATION_H

/**@} */