/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Advanced Micro Devices, Inc.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <stdint.h>
#include <string.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
#include <arch/io.h>
#include <arch/stages.h>
#include <device/pnp_def.h>
#include <arch/cpu.h>
#include <cpu/x86/lapic.h>
#include <console/console.h>
#include <console/loglevel.h>
#include <cpu/x86/mtrr.h>
#include <cpu/amd/car.h>
#include <northbridge/amd/agesa/agesawrapper.h>
#include <cpu/x86/bist.h>
#include <superio/nuvoton/common/nuvoton.h>
#include <superio/nuvoton/nct5572d/nct5572d.h>
#include <cpu/x86/lapic.h>
#include <sb_cimx.h>
#include "SBPLATFORM.h"


#define SERIAL_DEV PNP_DEV(0x2e, NCT5572D_SP1)

void cache_as_ram_main(unsigned long bist, unsigned long cpu_init_detectedx)
{
	u32 val;

	/*
	 * All cores: allow caching of flash chip code and data
	 * (there are no cache-as-ram reliability concerns with family 14h)
	 */
	__writemsr(0x20c, (0x0100000000ull - CACHE_ROM_SIZE) | 5);
	__writemsr(0x20d, (0x1000000000ull - CACHE_ROM_SIZE) | 0x800);

	/* All cores: set pstate 0 (1600 MHz) early to save a few ms of boot time */
	__writemsr(0xc0010062, 0);

	amd_initmmio();

	if (!cpu_init_detectedx && boot_cpu()) {
		post_code(0x30);
		sb_Poweron_Init();

		post_code(0x31);
		console_init();
	}

	/* Halt if there was a built in self test failure */
	post_code(0x34);
	report_bist_failure(bist);

	/* Load MPB */
	val = cpuid_eax(1);
	printk(BIOS_DEBUG, "BSP Family_Model: %08x\n", val);
	printk(BIOS_DEBUG, "cpu_init_detectedx = %08lx\n", cpu_init_detectedx);

	post_code(0x37);
	agesawrapper_amdinitreset();

	post_code(0x39);
	agesawrapper_amdinitearly();

	post_code(0x40);
	agesawrapper_amdinitpost();

  for (;;)
    ;

	post_code(0x41);
	agesawrapper_amdinitenv();
	amd_initenv();

	post_code(0x50);
	copy_and_run();
	printk(BIOS_ERR, "Error: copy_and_run() returned!\n");

	post_code(0x54);	/* Should never see this post code. */
}
