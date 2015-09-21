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
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#include <cbmem.h>
#include <stdlib.h>
#include <symbols.h>

void *cbmem_top(void)
{
	/* the stack lives on the end */
	return _dram + (CONFIG_DRAM_SIZE_MB << 20) - 8 * 1024;
}
