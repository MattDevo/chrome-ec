/* Copyright 2016 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/* Skylake IMVP8 / ROP PMIC chipset power control module for Chrome EC */

#ifndef __CROS_EC_SKYLAKE_H
#define __CROS_EC_SKYLAKE_H

/*
 * Input state flags.
 * TODO: Normalize the power signal masks from board defines to SoC headers.
 */
#define IN_PCH_SLP_S3_DEASSERTED  POWER_SIGNAL_MASK(X86_SLP_S3_DEASSERTED)
#define IN_PCH_SLP_S4_DEASSERTED  POWER_SIGNAL_MASK(X86_SLP_S4_DEASSERTED)
#define IN_PCH_SLP_SUS_DEASSERTED POWER_SIGNAL_MASK(X86_SLP_SUS_DEASSERTED)

#define IN_ALL_PM_SLP_DEASSERTED (IN_PCH_SLP_S3_DEASSERTED | \
				  IN_PCH_SLP_S4_DEASSERTED | \
				  IN_PCH_SLP_SUS_DEASSERTED)

/*
 * DPWROK is NC / stuffing option on initial boards.
 * TODO(shawnn): Figure out proper control signals.
 */
#define IN_PGOOD_ALL_CORE 0

#define IN_ALL_S0 (IN_PGOOD_ALL_CORE | IN_ALL_PM_SLP_DEASSERTED)

#define CHIPSET_G3S5_POWERUP_SIGNAL IN_PCH_SLP_SUS_DEASSERTED

#define CHARGER_INITIALIZED_DELAY_MS 100
#define CHARGER_INITIALIZED_TRIES 40

#endif /* __CROS_EC_SKYLAKE_H */
