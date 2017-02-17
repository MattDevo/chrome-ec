/* Copyright (c) 2012 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/* Thermal engine module for Chrome EC */

#include "chipset.h"
#include "common.h"
#include "console.h"
#include "gpio.h"
#include "hooks.h"
#include "host_command.h"
#include "pwm.h"
#include "task.h"
#include "temp_sensor.h"
#include "thermal.h"
#include "timer.h"
#include "util.h"

/* Console output macros */
#define CPUTS(outstr) cputs(CC_THERMAL, outstr)
#define CPRINTF(format, args...) cprintf(CC_THERMAL, format, ## args)

/*
 * Temperature threshold configuration. Must be in the same order as in enum
 * temp_sensor_type. Threshold values for overheated action first (warning,
 * prochot, power-down), followed by fan speed stepping thresholds.
 */

/* DANGER WILL ROBINSON: This is an ugly hack, just for Falco & Peppy.
 * DO NOT USE THIS AS AN EXAMPLE OF WHAT TO DO FOR ANY OTHER BOARD!!
 */
#ifdef BOARD_falco				/* DON'T DO THIS */
test_export_static struct thermal_config_t
		thermal_config[TEMP_SENSOR_TYPE_COUNT] = {
	/* TEMP_SENSOR_TYPE_CPU */
	{THERMAL_CONFIG_WARNING_ON_FAIL,
	 {368, 370, 372,
	  323, 328, 333, 338, 343, 353, 363} },
	/* TEMP_SENSOR_TYPE_BOARD */
	{THERMAL_CONFIG_NO_FLAG, {THERMAL_THRESHOLD_DISABLE_ALL} },
	/* TEMP_SENSOR_TYPE_CASE */
	{THERMAL_CONFIG_NO_FLAG, {THERMAL_THRESHOLD_DISABLE_ALL} },
};
test_export_static const int fan_speed[THERMAL_FAN_STEPS + 1] =
{1400, 1600, 1800, 2000, 2500, 3000, 3500, 5000};
#endif
#ifdef BOARD_peppy				/* DON'T DO THIS */
test_export_static struct thermal_config_t
		thermal_config[TEMP_SENSOR_TYPE_COUNT] = {
	/* TEMP_SENSOR_TYPE_CPU */
	{THERMAL_CONFIG_WARNING_ON_FAIL,
	 {368, 370, 372,
	  315, 325, 332, 339, 346, 353, 363} },
	/* TEMP_SENSOR_TYPE_BOARD */
	{THERMAL_CONFIG_NO_FLAG, {THERMAL_THRESHOLD_DISABLE_ALL} },
	/* TEMP_SENSOR_TYPE_CASE */
	{THERMAL_CONFIG_NO_FLAG, {THERMAL_THRESHOLD_DISABLE_ALL} },
};
test_export_static const int fan_speed[THERMAL_FAN_STEPS + 1] =
	{0, 3200, 3700, 4000, 4400, 4900, 5500, 6500};
#endif
#ifdef BOARD_host /* for testing */		/* DON'T DO THIS */
test_export_static struct thermal_config_t
		thermal_config[TEMP_SENSOR_TYPE_COUNT] = {
	/* TEMP_SENSOR_TYPE_CPU */
	{THERMAL_CONFIG_WARNING_ON_FAIL,
	 {373, 378, 383,
	  327, 335, 343, 351, 359} } ,
	/* TEMP_SENSOR_TYPE_BOARD */
	{THERMAL_CONFIG_NO_FLAG, {THERMAL_THRESHOLD_DISABLE_ALL} },
	/* TEMP_SENSOR_TYPE_CASE */
	{THERMAL_CONFIG_NO_FLAG, {THERMAL_THRESHOLD_DISABLE_ALL} },
};
test_export_static const int fan_speed[THERMAL_FAN_STEPS + 1] =
	{0, 3000, 4575, 6150, 7725, -1};
#endif


/* Number of consecutive overheated events for each temperature sensor. */
static int8_t ot_count[TEMP_SENSOR_COUNT][THRESHOLD_COUNT + THERMAL_FAN_STEPS];

/*
 * Flag that indicate if each threshold is reached.  Note that higher threshold
 * reached does not necessarily mean lower thresholds are reached (since we can
 * disable any threshold.)
 */
static int8_t overheated[THRESHOLD_COUNT + THERMAL_FAN_STEPS];
static int8_t *fan_threshold_reached = overheated + THRESHOLD_COUNT;

static int fan_ctrl_on = 1;

int thermal_set_threshold(enum temp_sensor_type type, int threshold_id,
			  int value)
{
	if (type < 0 || type >= TEMP_SENSOR_TYPE_COUNT)
		return EC_ERROR_INVAL;
	if (threshold_id < 0 ||
	    threshold_id >= THRESHOLD_COUNT + THERMAL_FAN_STEPS)
		return EC_ERROR_INVAL;
	if (value < 0)
		return EC_ERROR_INVAL;

	thermal_config[type].thresholds[threshold_id] = value;

	return EC_SUCCESS;
}

int thermal_get_threshold(enum temp_sensor_type type, int threshold_id)
{
	if (type < 0 || type >= TEMP_SENSOR_TYPE_COUNT)
		return -1;
	if (threshold_id < 0 ||
	    threshold_id >= THRESHOLD_COUNT + THERMAL_FAN_STEPS)
		return -1;

	return thermal_config[type].thresholds[threshold_id];
}

void thermal_control_fan(int enable)
{
	fan_ctrl_on = enable;

	/* If controlling the fan, need it in RPM-control mode */
	if (enable)
		pwm_set_fan_rpm_mode(1);
}

static void smi_overheated_warning(void)
{
	host_set_single_event(EC_HOST_EVENT_THERMAL_OVERLOAD);
}

static void smi_sensor_failure_warning(void)
{
	host_set_single_event(EC_HOST_EVENT_THERMAL);
}

/*
 * TODO: When we need different overheated action for different boards, move
 *       these actiona to a board-specific file. (e.g. board_thermal.c)
 */
static void overheated_action(void)
{
	static int cpu_down_count;

	if (overheated[THRESHOLD_POWER_DOWN]) {
		cprintf(CC_CHIPSET,
			"[%T critical temperature; shutting down]\n");
		chipset_force_shutdown();
		host_set_single_event(EC_HOST_EVENT_THERMAL_SHUTDOWN);
		return;
	}

	if (overheated[THRESHOLD_CPU_DOWN]) {
		cpu_down_count++;
		if (cpu_down_count > 3) {
			CPRINTF("[%T overheated; shutting down]\n");
			chipset_force_shutdown();
			host_set_single_event(EC_HOST_EVENT_THERMAL_SHUTDOWN);
		}
	} else {
		cpu_down_count = 0;
	}

	if (overheated[THRESHOLD_WARNING]) {
		smi_overheated_warning();
		chipset_throttle_cpu(1, THROTTLE_SRC_THERMAL);
	} else {
		chipset_throttle_cpu(0, THROTTLE_SRC_THERMAL);
	}

	if (fan_ctrl_on) {
		int i;

		for (i = THERMAL_FAN_STEPS - 1; i >= 0; --i)
			if (fan_threshold_reached[i])
				break;
		pwm_set_fan_target_rpm(fan_speed[i + 1]);
	}
}

/**
 * Update counter and check if the counter has reached delay limit.
 *
 * Note that we have various delay periods to prevent one error value
 * triggering an overheated action.
 */
static inline void update_and_check_stat(int temp,
					 int sensor_id,
					 int threshold_id)
{
	enum temp_sensor_type type = temp_sensors[sensor_id].type;
	const struct thermal_config_t *config = thermal_config + type;
	const int16_t threshold = config->thresholds[threshold_id];
	const int delay = temp_sensors[sensor_id].action_delay_sec;

	if (threshold <= 0) {
		ot_count[sensor_id][threshold_id] = 0;
	} else if (temp >= threshold) {
		++ot_count[sensor_id][threshold_id];
		if (ot_count[sensor_id][threshold_id] >= delay) {
			ot_count[sensor_id][threshold_id] = delay;
			overheated[threshold_id] = 1;
		}
	} else if (ot_count[sensor_id][threshold_id] >= delay &&
		   temp >= threshold - 3) {
		/*
		 * Once the threshold is reached, only deassert overheated if
		 * the temperature drops to 3 degrees below threshold.  This
		 * hysteresis prevents a temperature oscillating around the
		 * threshold causing overheated actions to trigger repeatedly.
		 */
		overheated[threshold_id] = 1;
	}
}

static void thermal_process(void)
{
	int i, j;
	int cur_temp;
	int flag;
	int rv;

	for (i = 0; i < THRESHOLD_COUNT + THERMAL_FAN_STEPS; ++i)
		overheated[i] = 0;

	for (i = 0; i < TEMP_SENSOR_COUNT; ++i) {
		enum temp_sensor_type type = temp_sensors[i].type;

		if (type == TEMP_SENSOR_TYPE_IGNORED)
			continue;

		flag = thermal_config[type].config_flags;

		rv = temp_sensor_read(i, &cur_temp);
		if (rv == EC_ERROR_NOT_POWERED) {
			/* Sensor not powered; ignore it */
			continue;
		} else if (rv) {
			/* Other sensor failure */
			if (flag & THERMAL_CONFIG_WARNING_ON_FAIL)
				smi_sensor_failure_warning();
			continue;
		}

		for (j = 0; j < THRESHOLD_COUNT + THERMAL_FAN_STEPS; ++j)
			update_and_check_stat(cur_temp, i, j);
	}

	overheated_action();
}

void thermal_task(void)
{
	while (1) {
		thermal_process();
		usleep(SECOND);
	}
}

static void thermal_shutdown(void)
{
	/* Take back fan control when the processor shuts down */
	thermal_control_fan(1);
}
DECLARE_HOOK(HOOK_CHIPSET_SHUTDOWN, thermal_shutdown, HOOK_PRIO_DEFAULT);

/*****************************************************************************/
/* Console commands */

static void print_thermal_config(enum temp_sensor_type type)
{
	const struct thermal_config_t *config = thermal_config + type;
	ccprintf("Sensor Type %d:\n", type);
	ccprintf("\tWarning: %d K\n",
		 config->thresholds[THRESHOLD_WARNING]);
	ccprintf("\tCPU Down: %d K\n",
		 config->thresholds[THRESHOLD_CPU_DOWN]);
	ccprintf("\tPower Down: %d K\n",
		 config->thresholds[THRESHOLD_POWER_DOWN]);
}

static void print_fan_stepping(enum temp_sensor_type type)
{
	const struct thermal_config_t *config = thermal_config + type;
	int i;

	ccprintf("Sensor Type %d:\n", type);
	ccprintf("\tLowest speed: %d RPM\n", fan_speed[0]);
	for (i = 0; i < THERMAL_FAN_STEPS; ++i)
		ccprintf("\t%3d K:        %d RPM\n",
			 config->thresholds[THRESHOLD_COUNT + i],
			 fan_speed[i+1]);
}

static int command_thermal_config(int argc, char **argv)
{
	char *e;
	int sensor_type, threshold_id, value;

	if (argc != 2 && argc != 4)
		return EC_ERROR_PARAM_COUNT;

	sensor_type = strtoi(argv[1], &e, 0);
	if (*e || sensor_type < 0 || sensor_type >= TEMP_SENSOR_TYPE_COUNT)
		return EC_ERROR_PARAM1;

	if (argc == 2) {
		print_thermal_config(sensor_type);
		return EC_SUCCESS;
	}

	threshold_id = strtoi(argv[2], &e, 0);
	if (*e || threshold_id < 0 || threshold_id >= THRESHOLD_COUNT)
		return EC_ERROR_PARAM2;

	value = strtoi(argv[3], &e, 0);
	if (*e || value < 0)
		return EC_ERROR_PARAM3;

	thermal_config[sensor_type].thresholds[threshold_id] = value;
	ccprintf("Setting threshold %d of sensor type %d to %d\n",
		 threshold_id, sensor_type, value);

	return EC_SUCCESS;
}
DECLARE_CONSOLE_COMMAND(thermalconf, command_thermal_config,
			"sensortype [threshold_id temp]",
			"Get/set thermal threshold temp",
			NULL);

static int command_fan_config(int argc, char **argv)
{
	char *e;
	int sensor_type, stepping_id, value;

	if (argc != 2 && argc != 4)
		return EC_ERROR_PARAM_COUNT;

	sensor_type = strtoi(argv[1], &e, 0);
	if ((e && *e) || sensor_type < 0 ||
	    sensor_type >= TEMP_SENSOR_TYPE_COUNT)
		return EC_ERROR_PARAM1;

	if (argc == 2) {
		print_fan_stepping(sensor_type);
		return EC_SUCCESS;
	}

	stepping_id = strtoi(argv[2], &e, 0);
	if ((e && *e) || stepping_id < 0 || stepping_id >= THERMAL_FAN_STEPS)
		return EC_ERROR_PARAM2;

	value = strtoi(argv[3], &e, 0);
	if (*e || value < 0)
		return EC_ERROR_PARAM3;

	thermal_config[sensor_type].thresholds[THRESHOLD_COUNT + stepping_id] =
		value;
	ccprintf("Setting fan step %d of sensor type %d to %d K\n",
		 stepping_id, sensor_type, value);

	return EC_SUCCESS;
}
DECLARE_CONSOLE_COMMAND(thermalfan, command_fan_config,
			"sensortype [threshold_id rpm]",
			"Get/set thermal threshold fan rpm",
			NULL);

static int command_thermal_auto_fan_ctrl(int argc, char **argv)
{
	thermal_control_fan(1);
	return EC_SUCCESS;
}
DECLARE_CONSOLE_COMMAND(autofan, command_thermal_auto_fan_ctrl,
			NULL,
			"Enable thermal fan control",
			NULL);

/*****************************************************************************/
/* Host commands */

static int thermal_command_set_threshold(struct host_cmd_handler_args *args)
{
	const struct ec_params_thermal_set_threshold *p = args->params;

	if (thermal_set_threshold(p->sensor_type, p->threshold_id, p->value))
		return EC_RES_ERROR;

	return EC_RES_SUCCESS;
}
DECLARE_HOST_COMMAND(EC_CMD_THERMAL_SET_THRESHOLD,
		     thermal_command_set_threshold,
		     EC_VER_MASK(0));

static int thermal_command_get_threshold(struct host_cmd_handler_args *args)
{
	const struct ec_params_thermal_get_threshold *p = args->params;
	struct ec_response_thermal_get_threshold *r = args->response;
	int value = thermal_get_threshold(p->sensor_type, p->threshold_id);

	if (value == -1)
		return EC_RES_ERROR;
	r->value = value;

	args->response_size = sizeof(*r);

	return EC_RES_SUCCESS;
}
DECLARE_HOST_COMMAND(EC_CMD_THERMAL_GET_THRESHOLD,
		     thermal_command_get_threshold,
		     EC_VER_MASK(0));

static int thermal_command_auto_fan_ctrl(struct host_cmd_handler_args *args)
{
	thermal_control_fan(1);
	return EC_RES_SUCCESS;
}
DECLARE_HOST_COMMAND(EC_CMD_THERMAL_AUTO_FAN_CTRL,
		     thermal_command_auto_fan_ctrl,
		     EC_VER_MASK(0));
