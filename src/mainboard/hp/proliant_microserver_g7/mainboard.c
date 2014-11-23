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

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <arch/io.h>
#include <cpu/x86/msr.h>
#include <cpu/amd/mtrr.h>
#include <device/pci_def.h>
#include <southbridge/amd/sb800/sb800.h>


u8 is_dev3_present(void);
void set_pcie_dereset(void);
void set_pcie_reset(void);
void enable_int_gfx(void);

/* from amd bimini_fam10/mainboard.c */
/* GPIO6. */
void enable_int_gfx(void)
{
	u8 byte;

	volatile u8 *gpio_reg;

	pm_iowrite(0xEA, 0x01);	/* diable the PCIB */
	/* Disable Gec */
	byte = pm_ioread(0xF6);
	byte |= 1;
	pm_iowrite(0xF6, byte);
	/* make sure the fed80000 is accessible */
	byte = pm_ioread(0x24);
	byte |= 1;
	pm_iowrite(0x24, byte);

	gpio_reg = (volatile u8 *)0xFED80000 + 0xD00; /* IoMux Register */

	*(gpio_reg + 0x6) = 0x1; /* Int_vga_en */
	*(gpio_reg + 170) = 0x1; /* gpio_gate */

	gpio_reg = (volatile u8 *)0xFED80000 + 0x100; /* GPIO Registers */

	*(gpio_reg + 0x6) = 0x8;
	*(gpio_reg + 170) = 0x0;
}

/*
 * Bimini uses GPIO 6 as PCIe slot reset, GPIO4 as GFX slot reset. We need to
 * pull it up before training the slot.
 ***/
void set_pcie_dereset(void)
{
	/* GPIO 50h reset PCIe slot */

	u8 *addr = (u8 *)(0xFED80000 + 0x100 + 0x50);
	u8 byte = ~(1 << 5);
	byte |= ~(1 << 6);
	*addr = byte;

}

void set_pcie_reset(void)
{
	/* GPIO 50h reset PCIe slot */

	u8 *addr = (u8 *)(0xFED80000 + 0x100 + 0x50);
	u8 byte = ~((1 << 5) | (1 << 6));
	*addr = byte;

}
u8 is_dev3_present(void)
{
	return 0;
}


/*************************************************
* enable the dedicated function in A785E-I board.
* This function called early than rs780_enable.
*************************************************/
static void mainboard_enable(device_t dev)
{
	printk(BIOS_INFO, "Mainboard Enable HP ProLiant G7. dev=0x%p\n", dev);

	set_pcie_dereset();
	enable_int_gfx();
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
