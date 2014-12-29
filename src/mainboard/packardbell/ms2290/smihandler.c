/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#include <arch/io.h>
#include <console/console.h>
#include <cpu/x86/smm.h>
#include "southbridge/intel/ibexpeak/nvs.h"
#include "southbridge/intel/ibexpeak/pch.h"
#include "southbridge/intel/ibexpeak/me.h"
#include <northbridge/intel/sandybridge/sandybridge.h>
#include <cpu/intel/model_2065x/model_2065x.h>
#include <ec/acpi/ec.h>
#include <pc80/mc146818rtc.h>
#include <delay.h>

/* This GPI is not confirmed. */
#define MS2290_ENABLE_GPI12	1

/* The southbridge SMI handler checks whether gnvs has a
 * valid pointer before calling the trap handler
 */
extern global_nvs_t *gnvs;

static void mainboard_smm_init(void)
{
	printk(BIOS_DEBUG, "initializing SMI\n");
}

int mainboard_io_trap_handler(int smif)
{
	static int smm_initialized;

	if (!smm_initialized) {
		mainboard_smm_init();
		smm_initialized = 1;
	}

	switch (smif) {

	default:
		return 0;
	}

	/* On success, the IO Trap Handler returns 1
	 * On failure, the IO Trap Handler returns a value != 1 */
	return 1;
}

void mainboard_smi_gpi(u32 gpi_sts)
{
}

static int mainboard_finalized = 0;

int mainboard_smi_apmc(u8 data)
{
	switch (data) {
	case APM_CNT_FINALIZE:
		printk(BIOS_DEBUG, "APMC: FINALIZE\n");
		if (mainboard_finalized) {
			printk(BIOS_DEBUG, "APMC#: Already finalized\n");
			return 0;
		}

		intel_me_finalize_smm();
		intel_pch_finalize_smm();
		intel_sandybridge_finalize_smm();
		intel_model_2065x_finalize_smm();

		mainboard_finalized = 1;
		break;
	case APM_CNT_ACPI_ENABLE:
		if (MS2290_ENABLE_GPI12)
			gpi_route_interrupt(12, GPI_IS_SCI);
		break;
	case APM_CNT_ACPI_DISABLE:
		if (MS2290_ENABLE_GPI12)
			gpi_route_interrupt(12, GPI_IS_SMI);
		break;
	default:
		break;
	}
	return 0;
}
