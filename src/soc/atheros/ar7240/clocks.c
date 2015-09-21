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
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#include <arch/io.h>
#include <soc/ar71xx_regs.h>
#include <assert.h>
#include <delay.h>
#include <soc/clocks.h>
#include <timer.h>

#include <soc/clocks.h>

/* read, modify with mask + val and write it back */
static inline void rmw32(unsigned long addr, uint32_t mask, uint32_t val) {
	uint32_t value = read32(addr);
	value &= ~mask;
	value |= val;
	write32(addr, value);
}

int ath7240_set_pll(struct pll_parameters *params) {
	uint32_t cpu_config = 0;
	uint32_t bypass = AR724X_CPU_PLL_BYPASS | AR724X_CPU_PLL_RESET | AR724X_CPU_PLL_NOPWD;

	/* check if pll params are valid */
	if(params->div_multiplier >= (1 << 10))
		return 1;

	if(params->refdiv >= (1 << 4))
		return 1;

	/* check if we already initilized the pll
	 * the cpu will reset when we setup our plls and this code aren't allowed to run again
	 * otherwise we get into a endless loop
	 */
	//if (read32(AR71XX_PLL_BASE + AR724X_PLL_REG_CPU_CLOCK_CTRL)
	//		& AR724X_CPU_PLL_RESET_SWITCH) {
	//	return 0;
	//}

	/* bypass cpu pll, reset pll, pretend powerdown of pll */
	rmw32(AR71XX_PLL_BASE + AR724X_PLL_REG_CPU_CONFIG,
			bypass,
			bypass);

	/* set plls */
	cpu_config = (params->div_multiplier & AR724X_PLL_FB_MASK) << AR724X_PLL_FB_SHIFT;
	cpu_config |= (params->refdiv & AR724X_PLL_REF_DIV_MASK) << AR724X_PLL_REF_DIV_SHIFT;
	cpu_config |= (params->divisor_ahb & AR724X_AHB_DIV_MASK) << AR724X_AHB_DIV_SHIFT;
	cpu_config |= (params->divisor_ddr & AR724X_DDR_DIV_MASK) << AR724X_DDR_DIV_SHIFT;

	write32(AR724X_PLL_REG_CPU_CONFIG, cpu_config | bypass);

	/* take pll out of reset */
	bypass &= ~AR724X_CPU_PLL_RESET;
	write32(AR724X_PLL_REG_CPU_CONFIG, cpu_config | bypass);

	/* wait until pll is done updateing */
	while ((read32(AR724X_PLL_REG_CPU_CONFIG) & AR724X_CPU_PLL_UPDATING) == 0)
		;

	/* disable bypass */
	write32(AR724X_PLL_REG_CPU_CONFIG, cpu_config);

	/* setting and clearing reset clock switch is taken from u-boot source GPL */
	/* TODO: check if this is really needed. why do we need to reset the cpu ? */
	/* cause the reset of the cpu using reset switch */
/*	rmw32(AR71XX_PLL_BASE + AR724X_PLL_REG_CPU_CLOCK_CTRL,
			AR724X_CPU_PLL_CTRL_RESET_SWITCH,
			AR724X_CPU_PLL_CTRL_RESET_SWITCH);

	rmw32(AR71XX_PLL_BASE + AR724X_PLL_REG_CPU_CLOCK_CTRL,
			AR724X_CPU_PLL_CTRL_CLOCK_SWITCH,
			AR724X_CPU_PLL_CTRL_CLOCK_SWITCH); */

	/* clear AR724X_CPU_PLL_CTRL_RESET_SWITCH & AR724X_CPU_PLL_CTRL_CLOCK_SWITCH */
/*	rmw32(AR71XX_PLL_BASE + AR724X_PLL_REG_CPU_CLOCK_CTRL,
			AR724X_CPU_PLL_CTRL_RESET_SWITCH,,
			0);
	rmw32(AR71XX_PLL_BASE + AR724X_PLL_REG_CPU_CLOCK_CTRL,
			AR724X_CPU_PLL_CTRL_CLOCK_SWITCH,,
			0); */
	return 0;
}
