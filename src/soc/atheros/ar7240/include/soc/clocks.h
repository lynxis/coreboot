/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Imagination Technologies
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

#ifndef __SOC_ATHEROS_AR7240_CLOCKS_H__
#define __SOC_ATHEROS_AR7240_CLOCKS_H__

/* atheros has 4 main clock source
 * all clocks are derived from the external crystal (ref clock) via PLL.
 *
 * taken from Atheros Datasheet AR7242 April 2011, p 20, Figure 2-4 Basic Clocking Diagram
 *
 *   | Ref Clk
 * -----
 * |PLL|
 * -----
 *   |
 * ----------
 * \        /-- PLL Bypass
 *  \------/
 *    |
 *    |---------------------------------------------------
 *    |            |                               |
 *    |        |----------|                  |-----------|
 *    |        |  DIV     |                  |  DIV      |
 *    |        | by 1 or 2| Ref Clk          | by 2 or 4 | Ref Clk
 *    |        |----------|  |               |-----------|  |
 *    |           |          |                   |          |
 *    |        ----------------                ---------------
 *    |        \              /-- PLL Bypass   \             /-- PLL Bypass
 *    |         \------------/                  \-----------/
 *    |              |                               |
 *    |              |                               |
 *    |             DDR Clock                     AHB Clock
 *    |
 *    |
 *    CPU Clock
 *
 * Ref clock - the external crystal. 25mhz or 40mhz
 * CPU clock
 * DDR clock
 * AHB clock - from this clock most subsystem will derive it's clocksa
 *
 */

enum ref_clock {
	REF_CLOCK25MHZ = 0,
	REF_CLOCK40MHZ,
};

enum divisor_ddr {
	DIV_DDR_BY_1 = 0,
	DIV_DDR_BY_2 = 1,
};

enum divisor_ahb {
	DIV_AHB_BY_2 = 0,
	DIV_AHB_BY_4 = 1,
};

/* cpu clock freq calculation
 * freq = (div_multiplier / refclock_div) * ref_clock /2
 *
 */
struct pll_parameters {
	enum ref_clock refclock;
	enum divisor_ddr divisor_ddr;
	enum divisor_ahb divisor_ahb;
	int div_multiplier; /* primary multiplier - named DIV in datasheet. p51 CPU_PLL_CONFIG (AR71XX_PLL_REG_CPU_CONFIG) */
	int refdiv; /* reference clock divider */
};

int ath7240_set_pll(struct pll_parameters *params);

int get_count_mhz_freq(void);
int ar71xx_get_ref_clock_mhz(void);

#endif
