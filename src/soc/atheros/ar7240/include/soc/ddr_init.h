/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Alexander Couzens <lynxis@fe80.eu>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef __SOC_IMGTEC_AR7240_DDR_INIT_H__
#define __SOC_IMGTEC_AR7240_DDR_INIT_H__

#define DDR_TIMEOUT			-1

int init_ddr1(void);
int init_ddr2(void);

#endif
