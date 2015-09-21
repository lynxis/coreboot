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

#include <memlayout.h>
#include <vendorcode/google/chromeos/memlayout.h>

#include <arch/header.ld>

SECTIONS
{
	/*
	 * All of DRAM (other than the DMA coherent area) is accessed through
	 * the identity mapping.
	 */
	DRAM_START(0x00000000)
	/* DMA coherent area: accessed via KSEG1. */
	DMA_COHERENT(0x00100000, 1M)
	POSTRAM_CBFS_CACHE(0x00200000, 192K)
	PRERAM_CBFS_CACHE(0x00230000, 56K)
	RAMSTAGE(0x0023e000, 128K)
	STACK(0x00ffd000, 8K)

	/*
	 * GRAM becomes the SRAM.  Accessed through KSEG0 in the bootblock
	 * and then through the identity mapping in ROM stage.
	 */
/*	SRAM_START(0x1a000000)
	ROMSTAGE(0x1a005000, 40K)
	PRERAM_CBFS_CACHE(0x1a012000, 56K)
	SRAM_END(0x1a020000) */

	/* Bootblock executes out of KSEG0 and sets up the identity mapping.
	 * This is identical to SRAM above, and thus also limited 64K and
	 * needs to avoid conflicts with items set up above.
	 */
	BOOTBLOCK(0xbfc00000, 32K)
	ROMSTAGE(0xbfc08000, 32K)

	/*
	 * Let's use SRAM for stack and CBMEM console.  Always accessed
	 * through KSEG0.
	 */
/*	PRERAM_CBMEM_CONSOLE(0x9b002000, 8K) */
}
