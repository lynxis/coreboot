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

/* AMD 14h documentation based on
 * BIOS and Kernel Developer’s Guide (BKDG) for AMD Family 14h Models 00h-0Fh Processors
 * 43170 Rev 3.13 - February 17, 2012
 */

static const io_register_t amd_14_e4[] = {
  /* IO Link */
  { 0x01010002, 4, "IO Link Hardware Debug"},
  { 0x01010010, 4, "IO Link Control 1"},
  { 0x01010011, 4, "IO Link Config Control"},
  { 0x0101001C, 4, "IO Link Control 2"},
  { 0x01010020, 4, "IO Link Chip Interface Control"},
  { 0x01010040, 4, "IO Link Phy Control"},
  { 0x010100B0, 4, "IO Link Strap Control"},
  { 0x010100C0, 4, "IO Link Strap Miscellaneous"},
  { 0x010100C1, 4, "IO Link Strap Miscellaneous"},
  /* Impedance Controller Registers */
  { 0x01088071, 4, "Receiver Impedance Adjustment"},
  { 0x01088072, 4, "Transmitter Impedance Adjustment"},
  /* TODO: PIF Registers */
  /* Wrapper Registers */
  { 0x01300000, 4, "BIF Core Feature Enable"},
  { 0x01300002, 4, "Link Speed Control"},
  { 0x01300080, 4, "Link Configuration"},
  { 0x01300800, 4, "Device 8 Link Training Control"},
  { 0x01300803, 4, "Device 8 Link De-emphasis Control"},
  { 0x01300900, 4, "Device 4 Link Training Control"},
  { 0x01300903, 4, "Device 4 Link De-emphasis Control"},
  { 0x01300a00, 4, "Device 5 Link Training Control"},
  { 0x01300a03, 4, "Device 5 Link De-emphasis Control"},
  { 0x01300b00, 4, "Device 6 Link Training Control"},
  { 0x01300b03, 4, "Device 6 Link De-emphasis Control"},
  { 0x01300c00, 4, "Device 7 Link Training Control"},
  { 0x01300c03, 4, "Device 7 Link De-emphasis Control"},
  { 0x01308002, 4, "Subsystem and Subvendor ID Control"},
  { 0x01308011, 4, "Link Transmit Clock Gating Control"},
  { 0x01308012, 4, "Link Transmit Clock Gating Control 2"},
  { 0x01308013, 4, "Transmit Clock PLL Control"},
  { 0x01308016, 4, "Link Clock Switching Control"},
  { 0x01308021, 4, "Transmitter Lane Mux"},
  { 0x01308022, 4, "Receiver Lane Mux"},
  { 0x01308023, 4, "Lane Enable"},
  { 0x01308025, 4, "Lane Mux Power Sequence Control 0"},
  { 0x01308031, 4, "Lane Counter Status"},
  { 0x01308060, 4, "Soft Reset Command 0"},
  { 0x01308061, 4, "Soft Reset Command 1"},
  { 0x01308062, 4, "Soft Reset Control 0"},
  { 0x013080F0, 4, "BIOS Timer"},
  { 0x013080F1, 4, "BIOS Timer Control"},
};

void print_index(struct pci_dev *nb)
{
  int index_size = 0;
  const io_register_t *index;

  printf("=== Index Register Space ===\n");
  switch (nb->device_id) {
    case PCI_VENDOR_ID_AMD_NB_14H:
      index_size = ARRAY_SIZE(amd_14_e4);
      index = amd_14_e4;
      break;
    default:
      printf("Unsupported Northbridge\n");
      return;
      break;
  }

  for (int i=0; i < index_size; i++) {
    printf("0x%8x = %08x - %s\n", index[i].addr, nbpcie_p_read_index(nb, index[i].addr), index[i].name);
  }
}
