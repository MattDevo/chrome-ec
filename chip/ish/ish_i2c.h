/* Copyright 2016 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef __CROS_EC_ISH_I2C_H
#define __CROS_EC_ISH_I2C_H

#include <stdint.h>
#include "task.h"

#define I2C_TSC_TIMEOUT			2000000
#define I2C_CALIB_ADDRESS		0x3
#define I2C_INTERRUPT_TIMEOUT		(TICKFREQ / 20)
#define NS_IN_SEC			1000
#define DEFAULT_SDA_HOLD		240
#define DEFAULT_SDA_HOLD_STD		2400
#define DEFAULT_SDA_HOLD_FAST		600
#define DEFAULT_SDA_HOLD_FAST_PLUS	300
#define DEFAULT_SDA_HOLD_HIGH		140
#define NS_2_COUNTERS(ns, clk)		((ns * clk)/NS_IN_SEC)
#define COUNTERS_2_NS(counters, clk)	(counters * (NANOSECONDS_IN_SEC / \
					(clk * HZ_IN_MEGAHZ)))
#define I2C_TX_FLUSH_TIMEOUT_USEC	200

#define ISH_I2C_FIFO_SIZE 64


enum {
	/* speed mode values */
	I2C_SPEED_STD = 0,
	I2C_SPEED_FAST = 1,
	I2C_SPEED_FAST_PLUS = 2,
	I2C_SPEED_HIGH = 3,
	/* freq mode values */
	I2C_FREQ_25 = 0,
	I2C_FREQ_50 = 1,
	I2C_FREQ_100 = 2,
	I2C_FREQ_120 = 3,
	I2C_FREQ_40 = 4,
	I2C_FREQ_20 = 5,
	I2C_FREQ_37 = 6
};

const unsigned int clk_in[] = {
	[I2C_FREQ_25] = 25,
	[I2C_FREQ_50] = 50,
	[I2C_FREQ_100] = 100,
	[I2C_FREQ_120] = 120,
	[I2C_FREQ_40] = 40,
	[I2C_FREQ_20] = 20,
	[I2C_FREQ_37] = 37,
};

const uint8_t spkln[] = {
	[I2C_FREQ_25] = 2,
	[I2C_FREQ_50] = 3,
	[I2C_FREQ_100] = 5,
	[I2C_FREQ_120] = 6,
	[I2C_FREQ_40] = 2,
	[I2C_FREQ_20] = 1,
	[I2C_FREQ_37] = 2,
};

enum {
	I2C_READ,
	I2C_WRITE
};

enum {
	/* REGISTERS */
	IC_ENABLE = 0x6c,
	IC_STATUS = 0x70,
	IC_ENABLE_STATUS = 0x9c,
	IC_CON = 0x00,
	IC_TAR = 0x04,
	IC_DATA_CMD = 0x10,
	IC_RX_TL = 0x38,
	IC_TX_TL = 0x3c,
	IC_COMP_PARAM_1 = 0xf4,
	IC_INTR_MASK = 0x30,
	IC_RAW_INTR_STAT = 0x34,
	IC_INTR_STAT = 0x2c,
	IC_CLR_TX_ABRT = 0x54,
	IC_TX_ABRT_SOURCE = 0x80,
	IC_SS_SCL_HCNT = 0x14,
	IC_SS_SCL_LCNT = 0x18,
	IC_FS_SCL_HCNT = 0x1c,
	IC_FS_SCL_LCNT = 0x20,
	IC_HS_SCL_HCNT = 0x24,
	IC_HS_SCL_LCNT = 0x28,
	IC_CLR_STOP_DET = 0x60,
	IC_CLR_START_DET = 0x64,
	IC_TXFLR = 0x74,
	IC_SDA_HOLD = 0x7c,
	IC_FS_SPKLEN = 0xA0,
	IC_HS_SPKLEN = 0xA4,
	/* IC_ENABLE VALUES */
	IC_ENABLE_ENABLE = 1,
	IC_ENABLE_DISABLE = 0,
	/* IC_STATUS OFFSETS */
	IC_STATUS_MASTER_ACTIVITY = 5,
	IC_STATUS_TFE = 2,
	/* IC_CON OFFSETS */
	MASTER_MODE_OFFSET = 0,
	SPEED_OFFSET = 1,
	IC_RESTART_EN_OFFSET = 5,
	IC_SLAVE_DISABLE_OFFSET = 6,
	/* IC_CON VALUES */
	MASTER_MODE = 1,
	STD_SPEED = 1,
	FAST_SPEED = 2,
	HIGH_SPEED = 3,
	IC_RESTART_EN = 1,
	IC_SLAVE_DISABLE = 1,
	/* IC_CON WRITE VALUES */
	MASTER_MODE_VAL = (MASTER_MODE << MASTER_MODE_OFFSET),
	STD_SPEED_VAL = (STD_SPEED << SPEED_OFFSET),
	FAST_SPEED_VAL = (FAST_SPEED << SPEED_OFFSET),
	FAST_PLUS_SPEED_VAL = (FAST_SPEED << SPEED_OFFSET),
	HIGH_SPEED_VAL = (HIGH_SPEED << SPEED_OFFSET),
	SPEED_MASK = (0x3 << SPEED_OFFSET),
	IC_RESTART_EN_VAL = (IC_RESTART_EN << IC_RESTART_EN_OFFSET),
	IC_SLAVE_DISABLE_VAL = (IC_SLAVE_DISABLE << IC_SLAVE_DISABLE_OFFSET),
	/* IC_TAR OFFSETS */
	IC_TAR_OFFSET = 0,
	SPECIAL_OFFSET = 11,
	IC_10BITADDR_MASTER_OFFSET = 12,
	/* IC_TAR VALUES */
	TAR_SPECIAL = 0,
	IC_10BITADDR_MASTER = 0,
	/* IC_TAR WRITE VALUES */
	IC_10BITADDR_MASTER_VAL =
	    (IC_10BITADDR_MASTER << IC_10BITADDR_MASTER_OFFSET),
	TAR_SPECIAL_VAL = (TAR_SPECIAL << SPECIAL_OFFSET),
	/* IC_DATA_CMD OFFSETS */
	DATA_CMD_DAT_OFFSET = 0,
	DATA_CMD_CMD_OFFSET = 8,
	DATA_CMD_STOP_OFFSET = 9,
	DATA_CMD_RESTART_OFFSET = 10,
	/* IC_DATA_CMD VALUES */
	DATA_CMD_READ = 1,
	DATA_CMD_WRITE = 0,
	DATA_CMD_STOP = 1,
	DATA_CMD_RESTART = 1,
	/* IC_DATA_CMD WRITE VALUES */
	DATA_CMD_WRITE_VAL = (DATA_CMD_WRITE << DATA_CMD_CMD_OFFSET),
	DATA_CMD_READ_VAL = (DATA_CMD_READ << DATA_CMD_CMD_OFFSET),
	DATA_CMD_STOP_VAL = (DATA_CMD_STOP << DATA_CMD_STOP_OFFSET),
	DATA_CMD_RESTART_VAL = (DATA_CMD_RESTART << DATA_CMD_RESTART_OFFSET),
	/* IC_TX_TL */
	IC_TX_TL_VAL = 0,
	/* IC_COM_PARAM_OFFSETS */
	TX_BUFFER_DEPTH_OFFSET = 16,
	RX_BUFFER_DEPTH_OFFSET = 8,
	/* IC_INTR_MASK VALUES */
	M_RX_FULL = (1 << 2),
	M_TX_EMPTY = (1 << 4),
	M_TX_ABRT = (1 << 6),
	M_STOP_DET = (1 << 9),
	M_START_DET = (1 << 10),
	IC_INTR_WRITE_MASK_VAL = (M_STOP_DET | M_TX_ABRT),
	IC_INTR_READ_MASK_VAL = (M_RX_FULL | M_TX_ABRT),
	DISABLE_INT = 0,
	ENABLE_WRITE_INT = 1,
	ENABLE_READ_INT = 2,
	/* IC_ENABLE_STATUS_OFFSETS */
	IC_EN_OFFSET = 0,
	/* IC_ENABLE_STATUS_VALUES */
	IC_EN_DISABLED_VAL = 0,
	IC_EN_DISABLED = (IC_EN_DISABLED_VAL << IC_EN_OFFSET),
	IC_EN_MASK = (1 << IC_EN_OFFSET),
	/* IC_TX_ABRT_SOURCE bits */
	ABRT_7B_ADDR_NOACK = 1,
};

struct i2c_bus_data {
	uint16_t hcnt;
	uint16_t lcnt;
	uint16_t sda_hold;
};

struct i2c_bus_info {
	uint8_t bus_id;
	struct i2c_bus_data std_speed;
	struct i2c_bus_data fast_speed;
	struct i2c_bus_data fast_plus_speed;
	struct i2c_bus_data high_speed;
} __attribute__ ((__packed__));

struct i2c_context {
	uint32_t *base;
	uint8_t max_rx_depth;
	uint8_t max_tx_depth;
	uint8_t bus;
	uint8_t speed;
	uint32_t interrupts;
	uint32_t reason;
	uint32_t int_pin;
	uint8_t error_flag;
	task_id_t wait_task_id;
};

#endif /* __CROS_EC_ISH_I2C_H */
