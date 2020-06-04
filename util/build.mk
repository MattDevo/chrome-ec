# -*- makefile -*-
# Copyright (c) 2011 The Chromium OS Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.
#
# Host tools build
#

host-util-bin= ectool burn_my_ec
ifeq ($(CONFIG_LPC),y)
host-util-common=comm-lpc
else
host-util-common=comm-i2c
endif
build-util-bin=
