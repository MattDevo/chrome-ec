/* -*- mode:c -*-
 *
 * Copyright 2019 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/* Declare symbolic names for all the GPIOs that we care about.
 * Note: Those with interrupt handlers must be declared first. */

GPIO_INT(USB_C0_TCPC_INT_ODL,	PIN(3, 4), GPIO_INT_FALLING,			tcpc_alert_event)
GPIO_INT(USB_C1_TCPC_INT_ODL,	PIN(F, 1), GPIO_INT_FALLING,			tcpc_alert_event)
GPIO_INT(USB_C0_PPC_INT_ODL,	PIN(6, 3), GPIO_INT_FALLING,			ppc_interrupt)
GPIO_INT(USB_C1_PPC_INT_ODL,	PIN(D, 4), GPIO_INT_FALLING,			ppc_interrupt)
GPIO_INT(PCH_SLP_S3_L,		PIN(7, 4), GPIO_INT_BOTH,			power_signal_interrupt)
GPIO_INT(PCH_SLP_S5_L,		PIN(E, 0), GPIO_INT_BOTH,			power_signal_interrupt)
GPIO_INT(S0_PGOOD,		PIN(5, 6), GPIO_INT_BOTH,			power_signal_interrupt)
GPIO_INT(S5_PGOOD,		PIN(3, 7), GPIO_INT_BOTH,			power_signal_interrupt)
GPIO_INT(POWER_BUTTON_L,	PIN(0, 1), GPIO_INT_BOTH,			power_button_interrupt)
GPIO_INT(LID_OPEN,		PIN(D, 2), GPIO_INT_BOTH | GPIO_HIB_WAKE_HIGH,	lid_interrupt)
GPIO_INT(AC_PRESENT,		PIN(0, 0), GPIO_INT_BOTH | GPIO_HIB_WAKE_HIGH,	extpower_interrupt)
GPIO_INT(WP_L,			PIN(5, 0), GPIO_INT_BOTH,			switch_interrupt)
GPIO_INT(VOLUME_DOWN_L,		PIN(A, 6), GPIO_INT_BOTH,			button_interrupt)
GPIO_INT(VOLUME_UP_L,		PIN(9, 5), GPIO_INT_BOTH,			button_interrupt)
GPIO_INT(6AXIS_INT_L,		PIN(A, 0), GPIO_INT_FALLING,			bmi160_interrupt)

/* GPIO_INT_BOTH is required for PSL wake from hibernate, but we don't need an interrupt handler. */
GPIO(EC_RST_ODL,		PIN(0, 2), GPIO_INT_BOTH | GPIO_HIB_WAKE_HIGH)

GPIO(3AXIS_INT_L,		PIN(9, 6), GPIO_INPUT)				/* 3 Axis Accel */
GPIO(CCD_MODE_ODL,		PIN(C, 6), GPIO_INPUT)				/* Case Closed Debug Mode */
GPIO(CPU_PROCHOT,		PIN(D, 5), GPIO_INPUT)				/* PROCHOT to SOC */
GPIO(EC_BATT_PRES_L,		PIN(4, 1), GPIO_INPUT)				/* Battery Present */
GPIO(EC_INT_L,			PIN(A, 3), GPIO_ODR_HIGH)			/* Sensor MKBP event to SOC */
GPIO(EN_PWR_A,			PIN(B, 7), GPIO_OUT_LOW)			/* Enable Power */
GPIO(ENABLE_BACKLIGHT_L,	PIN(A, 2), GPIO_OUT_HIGH)			/* Enable Backlight */
GPIO(ENTERING_RW,		PIN(E, 5), GPIO_OUT_LOW)			/* EC Entering RW */
GPIO(PCH_PWRBTN_L,		PIN(8, 6), GPIO_OUT_HIGH)			/* Power Button to SOC */
GPIO(PCH_RSMRST_L,		PIN(A, 1), GPIO_OUT_LOW)			/* RSMRST# to SOC */
GPIO(PCH_SYS_PWROK,		PIN(7, 5), GPIO_OUT_LOW)			/* Power OK to SOC */
GPIO(PCH_WAKE_L,		PIN(0, 3), GPIO_OUT_HIGH)			/* Wake SOC */
GPIO(SYS_RESET_L,		PIN(C, 7), GPIO_ODR_HIGH)			/* Cold Reset to SOC */
GPIO(USB_C0_TCPC_RST_L,		PIN(E, 1), GPIO_OUT_HIGH)			/* C0 TCPC Reset */
GPIO(USB_C1_TCPC_RST_L,		PIN(F, 0), GPIO_OUT_HIGH)			/* C1 TCPC Reset */

GPIO(LED_FULL_L,		PIN(6, 0), GPIO_OUT_HIGH)
GPIO(LED_CHRG_L,		PIN(C, 0), GPIO_OUT_HIGH)

/* IOEX(EN_USB_A0_5V,		EXPIN(0, 1, 5), GPIO_OUT_LOW) */		/* Enable A0 5V Charging */
/* IOEX(EN_USB_A1_5V,		EXPIN(1, 1, 6), GPIO_OUT_LOW) */		/* Enable A1 5V Charging */
/* IOEX(KB_BL_EN,		EXPIN(0, 1, 3), GPIO_OUT_LOW) */		/* Enable KB Backlight */

/*
 * The NPCX LPC driver configures and controls SCI, so PCH_SCI_ODL [PIN(7, 6)]
 * is not defined here as GPIO.
 */

/* I2C pins - these will be reconfigured for alternate function below */
GPIO(I2C0_SCL,			PIN(B, 5), GPIO_INPUT)				/* EC_I2C_USB_A0_C0_SCL */
GPIO(I2C0_SDA,			PIN(B, 4), GPIO_INPUT)				/* EC_I2C_USB_A0_C0_SDA */
GPIO(I2C1_SCL,			PIN(9, 0), GPIO_INPUT)				/* EC_I2C_USB_A1_C1_SCL */
GPIO(I2C1_SDA,			PIN(8, 7), GPIO_INPUT)				/* EC_I2C_USB_A1_C1_SDA */
GPIO(I2C2_SCL,			PIN(9, 2), GPIO_INPUT)				/* EC_I2C_POWER_CBI_SCL */
GPIO(I2C2_SDA,			PIN(9, 1), GPIO_INPUT)				/* EC_I2C_POWER_CBI_SDA */
GPIO(I2C3_SCL,			PIN(D, 1), GPIO_INPUT)				/* EC_I2C_USBC_AP_MUX_SCL */
GPIO(I2C3_SDA,			PIN(D, 0), GPIO_INPUT)				/* EC_I2C_USBC_AP_MUX_SDA */
GPIO(I2C4_SCL,			PIN(F, 3), GPIO_INPUT)				/* FCH_SIC */
GPIO(I2C4_SDA,			PIN(F, 2), GPIO_INPUT)				/* FCH_SID */
GPIO(I2C5_SCL,			PIN(3, 3), GPIO_INPUT)				/* EC_I2C_SENSOR_SCL */
GPIO(I2C5_SDA,			PIN(3, 6), GPIO_INPUT)				/* EC_I2C_SENSOR_SDA */
GPIO(I2C6_SCL,			PIN(E, 4), GPIO_INPUT)				/* FCH_I2C_AUDIO_SCL */
GPIO(I2C6_SDA,			PIN(E, 3), GPIO_INPUT)				/* FCH_I2C_AUDIO_SDA */
GPIO(I2C7_SCL,			PIN(B, 3), GPIO_INPUT)				/* FCH_I2C_HDMI_HUB_3V3_SCL */
GPIO(I2C7_SDA,			PIN(B, 2), GPIO_INPUT)				/* FCH_I2C_HDMI_HUB_3V3_SDA */

ALTERNATE(PIN_MASK(6, BIT(4) | BIT(5)), 0, MODULE_UART, 0) /* Cr50 requires no pullups. */

ALTERNATE(PIN_MASK(B, BIT(4) | BIT(5)), 0, MODULE_I2C, 0)			/* I2C0 */
ALTERNATE(PIN_MASK(9, BIT(0) | BIT(1) | BIT(2)), 0, MODULE_I2C, 0)		/* I2C1 SCL / I2C2 */
ALTERNATE(PIN_MASK(8, BIT(7)), 0, MODULE_I2C, 0)				/* I2C1 SDA */
ALTERNATE(PIN_MASK(D, BIT(0) | BIT(1)), 0, MODULE_I2C, 0)			/* I2C3 */
ALTERNATE(PIN_MASK(F, BIT(2) | BIT(3)), 0, MODULE_I2C, 0)			/* I2C4 */
ALTERNATE(PIN_MASK(3, BIT(3) | BIT(6)), 0, MODULE_I2C, 0)			/* I2C5 */
ALTERNATE(PIN_MASK(E, BIT(3) | BIT(4)), 0, MODULE_I2C, 0)			/* I2C6 */
ALTERNATE(PIN_MASK(B, BIT(2) | BIT(3)), 0, MODULE_I2C, 0)			/* I2C7 */

ALTERNATE(PIN_MASK(4, BIT(2) | BIT(3)), 0, MODULE_ADC, 0)			/* ADC2, ADC3 Temp Sensors */

ALTERNATE(PIN_MASK(C, BIT(3)), 0, MODULE_PWM, 0)				/* PWM0 LED */
ALTERNATE(PIN_MASK(8, BIT(0)), 0, MODULE_PWM, 0)				/* PWM3 KB Backlight */

ALTERNATE(PIN_MASK(3, 0x03), 0, MODULE_KEYBOARD_SCAN, GPIO_INPUT)		/* KSI_00-01 */
ALTERNATE(PIN_MASK(2, 0xFC), 0, MODULE_KEYBOARD_SCAN, GPIO_INPUT)		/* KSI_02-07 */
ALTERNATE(PIN_MASK(2, 0x03), 0, MODULE_KEYBOARD_SCAN, GPIO_ODR_HIGH)		/* KSO_00-01 */
GPIO(KBD_KSO2, PIN(1, 7), GPIO_OUT_LOW)						/* KSO_02 inverted */
ALTERNATE(PIN_MASK(1, 0x7F), 0, MODULE_KEYBOARD_SCAN, GPIO_ODR_HIGH)		/* KSO_03-09 */
ALTERNATE(PIN_MASK(0, 0xE0), 0, MODULE_KEYBOARD_SCAN, GPIO_ODR_HIGH)		/* KSO_10-12 */

/* Power Switch Logic (PSL) inputs */
ALTERNATE(PIN_MASK(0, BIT(0) | BIT(1) | BIT(2)), 0, MODULE_PMU, 0)		/* AC_PRESENT, POWER_BUTTON_L, EC_RST_ODL */
ALTERNATE(PIN_MASK(D, BIT(2)), 0, MODULE_PMU, 0)				/* LID_OPEN */