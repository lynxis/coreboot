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

#include <stdio.h>
#include <stdint.h>
#include <pci/pci.h>

#include "amdtool.h"
#include "cmn.h"

int main() {
  struct pci_access *pci_acc = pci_alloc();
  struct pci_dev *dev = NULL;

  pci_init(pci_acc);
  dev = pci_get_dev(pci_acc, 0, 0, 0, 0);

  print_index(dev);

  return 0;
}
