/* Copyright 2015 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 * Power and battery LED control for Buddy.
 */

#include "battery.h"
#include "charge_state.h"
#include "chipset.h"
#include "ec_commands.h"
#include "gpio.h"
#include "hooks.h"
#include "host_command.h"
#include "led_common.h"
#include "util.h"

#define LED_TOTAL_TICKS 16
#define LED_ON_TICKS 4

enum led_color {
	LED_OFF = 0,
	LED_BLUE,
	LED_COLOR_COUNT  /* Number of colors, not a color itself */
};

const enum ec_led_id supported_led_ids[] = {
	EC_LED_ID_POWER_LED};

const int supported_led_ids_count = ARRAY_SIZE(supported_led_ids);

static int buddy_led_set_gpio(enum led_color color,
			      enum gpio_signal gpio_led_blue_l)
{
	switch (color) {
	case LED_OFF:
		gpio_set_level(gpio_led_blue_l,  1);
		break;
	case LED_BLUE:
		gpio_set_level(gpio_led_blue_l,  0);
		break;
	default:
		return EC_ERROR_UNKNOWN;
	}
	return EC_SUCCESS;
}

static int buddy_led_set_color_power(enum led_color color)
{
	return buddy_led_set_gpio(color, GPIO_PWR_LED0_L);
}

static int buddy_led_set_color(enum ec_led_id led_id, enum led_color color)
{
	int rv;

	led_auto_control(led_id, 0);
	switch (led_id) {
	case EC_LED_ID_POWER_LED:
		rv = buddy_led_set_color_power(color);
		break;
	default:
		return EC_ERROR_UNKNOWN;
	}
	return rv;
}

int led_set_brightness(enum ec_led_id led_id, const uint8_t *brightness)
{
	if (brightness[EC_LED_COLOR_BLUE] != 0)
		buddy_led_set_color(led_id, LED_BLUE);
	else
		buddy_led_set_color(led_id, LED_OFF);

	return EC_SUCCESS;
}

void led_get_brightness_range(enum ec_led_id led_id, uint8_t *brightness_range)
{
	/* Ignoring led_id as both leds support the same colors */
	brightness_range[EC_LED_COLOR_BLUE] = 1;
}

static void buddy_led_set_power(void)
{
	static int power_ticks;
	static int previous_state_suspend;

	power_ticks++;

	if (chipset_in_state(CHIPSET_STATE_SUSPEND)) {
		/* Reset ticks if entering suspend so LED turns blue
		 * as soon as possible. */
		if (!previous_state_suspend)
			power_ticks = 0;

		/* Blink once every four seconds. */
		buddy_led_set_color_power(
			(power_ticks % LED_TOTAL_TICKS < LED_ON_TICKS) ?
			LED_BLUE : LED_OFF);

		previous_state_suspend = 1;
		return;
	}

	previous_state_suspend = 0;

	if (chipset_in_state(CHIPSET_STATE_ANY_OFF))
		buddy_led_set_color_power(LED_OFF);
	else if (chipset_in_state(CHIPSET_STATE_ON))
		buddy_led_set_color_power(LED_BLUE);
}

/* Called by hook task every 250mSec */
static void led_tick(void)
{
	if (led_auto_control_is_enabled(EC_LED_ID_POWER_LED))
		buddy_led_set_power();
}
DECLARE_HOOK(HOOK_TICK, led_tick, HOOK_PRIO_DEFAULT);
