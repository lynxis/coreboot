/*
 * amdtool - dump all registers on an AMD CPU + chipset based system.
 *
 * Copyright (C) 2008 Advanced Micro Devices, Inc.
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

#ifndef CMN_H__H_
#define CMN_H__H_

struct pci_dev;

u32 nbmisc_read_index(struct pci_dev * nb_dev, u32 index);
void nbmisc_write_index(struct pci_dev * nb_dev, u32 index, u32 data);
u32 nbpcie_p_read_index(struct pci_dev * dev, u32 index);
void nbpcie_p_write_index(struct pci_dev * dev, u32 index, u32 data);
u32 nbpcie_ind_read_index(struct pci_dev * nb_dev, u32 index);
void nbpcie_ind_write_index(struct pci_dev * nb_dev, u32 index, u32 data);
u32 htiu_read_index(struct pci_dev * nb_dev, u32 index);
void htiu_write_index(struct pci_dev * nb_dev, u32 index, u32 data);
u32 nbmc_read_index(struct pci_dev * nb_dev, u32 index);
void nbmc_write_index(struct pci_dev * nb_dev, u32 index, u32 data);
void set_nbcfg_enable_bits(struct pci_dev * nb_dev, u32 reg_pos, u32 mask, u32 val);
void set_nbcfg_enable_bits_8(struct pci_dev * nb_dev, u32 reg_pos, u8 mask, u8 val);
void set_nbmc_enable_bits(struct pci_dev * nb_dev, u32 reg_pos, u32 mask, u32 val);
void set_htiu_enable_bits(struct pci_dev * nb_dev, u32 reg_pos, u32 mask, u32 val);
void set_nbmisc_enable_bits(struct pci_dev * nb_dev, u32 reg_pos, u32 mask, u32 val);
void set_pcie_enable_bits(struct pci_dev * dev, u32 reg_pos, u32 mask, u32 val);

#endif /* CMN_H__H_ */
