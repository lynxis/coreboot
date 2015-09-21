/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 Alexander Couzens <lynxis@fe80.eu>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#include <arch/cpu.h>
#include <soc/clocks.h>
#include <soc/cpu.h>
#include <arch/io.h>
#include <stdint.h>
#include <timer.h>
#include <timestamp.h>

#include <soc/ar71xx_regs.h>

int get_count_mhz_freq(void)
{
	uint32_t cpu_config;

	uint32_t div_multiplier;
	uint32_t ref_div;
	
	cpu_config = read32(AR71XX_PLL_BASE + AR724X_PLL_REG_CPU_CONFIG);

	if (cpu_config & AR724X_CPU_PLL_BYPASS) {
		return ar71xx_get_ref_clock_mhz();
	}

	div_multiplier = cpu_config & AR724X_PLL_FB_MASK;
	ref_div = (cpu_config >> AR724X_PLL_REF_DIV_SHIFT) & AR724X_PLL_REF_DIV_MASK;

	return ((div_multiplier / ref_div) * ar71xx_get_ref_clock_mhz()) / 2;
}

void timer_monotonic_get(struct mono_time *mt)
{
	mono_time_set_usecs(mt, read_c0_count() / get_count_mhz_freq());
}
