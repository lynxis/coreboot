/*
 * amdtool - dump all registers on an AMD CPU + chipset based system.
 *
 * Copyright (C) 2015 Alexander Couzens <lynxis@fe80.eu>
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef AMDTOOL_H_
#define AMDTOOL_H_

#include <stdint.h>

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#endif

#define PCI_VENDOR_ID_AMD 0x1022

#define PCI_VENDOR_ID_AMD_NB_14H 0x1510

typedef struct { uint32_t addr; int size; char *name; } io_register_t;

void print_index(struct pci_dev *nb);

#endif /* AMDTOOL_H_ */
