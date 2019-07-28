/* Copyright 2019 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/* Trembyle board configuration */

#include "button.h"
#include "driver/accelgyro_bmi160.h"
#include "extpower.h"
#include "gpio.h"
#include "lid_switch.h"
#include "power.h"
#include "power_button.h"
#include "switch.h"
#include "usb_charge.h"

static void ppc_interrupt(enum gpio_signal signal)
{
	/* TODO */
}

#include "gpio_list.h"

void board_update_sensor_config_from_sku(void)
{
	/* Enable Gyro interrupts */
	gpio_enable_interrupt(GPIO_6AXIS_INT_L);
}

int board_is_sourcing_vbus(int port)
{
	/* TODO */
	return 0;
}

void board_reset_pd_mcu(void)
{
	/* TODO */
}

uint32_t system_get_sku_id(void)
{
	/* TODO */
	return 0;
}

void usb_charger_set_switches(int port, enum usb_switch setting)
{
	/* TODO */
}

uint16_t tcpc_get_alert_status(void)
{
	/* TODO */
	return 0;
}

void usb_charger_task(void *u)
{
	/* TODO */
}

int usb_charger_ramp_allowed(int supplier)
{
	/* TODO */
	return 0;
}

int usb_charger_ramp_max(int supplier, int sup_curr)
{
	/* TODO */
	return 0;
}

void tcpc_alert_event(enum gpio_signal signal)
{
	/* TODO */
}

int charger_get_vbus_voltage(int port)
{
	/* TODO */
	return 0;
}

int board_set_active_charge_port(int port)
{
	/* TODO */
	return 0;
}