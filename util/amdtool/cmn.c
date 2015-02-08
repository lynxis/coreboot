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

#include <stdint.h>
#include <pci/pci.h>

#define NBMISC_INDEX    0x60
#define NBPCIE_INDEX    0xE0
#define NBMC_INDEX    0xE8
#define NBHTIU_INDEX  0x94

#define pci_read_config32 pci_read_long
#define pci_read_config8 pci_read_byte
#define pci_write_config32 pci_write_long
#define pci_write_config8 pci_write_byte

static u32 nb_read_index(struct pci_dev * dev, u32 index_reg, u32 index)
{
	pci_write_config32(dev, index_reg, index);
	return pci_read_config32(dev, index_reg + 0x4);
}

static void nb_write_index(struct pci_dev * dev, u32 index_reg, u32 index, u32 data)
{
	pci_write_config32(dev, index_reg, index);
	pci_write_config32(dev, index_reg + 0x4, data);
}


u32 nbmisc_read_index(struct pci_dev * nb_dev, u32 index)
{
	return nb_read_index((nb_dev), NBMISC_INDEX, (index));
}

void nbmisc_write_index(struct pci_dev * nb_dev, u32 index, u32 data)
{
	nb_write_index((nb_dev), NBMISC_INDEX, ((index) | 0x80), (data));
}

u32 nbpcie_p_read_index(struct pci_dev * dev, u32 index)
{
	return nb_read_index((dev), NBPCIE_INDEX, (index));
}

void nbpcie_p_write_index(struct pci_dev * dev, u32 index, u32 data)
{
	nb_write_index((dev), NBPCIE_INDEX, (index), (data));
}

u32 nbpcie_ind_read_index(struct pci_dev * nb_dev, u32 index)
{
	return nb_read_index((nb_dev), NBPCIE_INDEX, (index));
}

void nbpcie_ind_write_index(struct pci_dev * nb_dev, u32 index, u32 data)
{
	nb_write_index((nb_dev), NBPCIE_INDEX, (index), (data));
}

u32 htiu_read_index(struct pci_dev * nb_dev, u32 index)
{
	return nb_read_index((nb_dev), NBHTIU_INDEX, (index));
}

void htiu_write_index(struct pci_dev * nb_dev, u32 index, u32 data)
{
	nb_write_index((nb_dev), NBHTIU_INDEX, ((index) | 0x100), (data));
}

u32 nbmc_read_index(struct pci_dev * nb_dev, u32 index)
{
	return nb_read_index((nb_dev), NBMC_INDEX, (index));
}

void nbmc_write_index(struct pci_dev * nb_dev, u32 index, u32 data)
{
	nb_write_index((nb_dev), NBMC_INDEX, ((index) | 1 << 9), (data));
}

void set_nbcfg_enable_bits(struct pci_dev * nb_dev, u32 reg_pos, u32 mask, u32 val)
{
	u32 reg_old, reg;
	reg = reg_old = pci_read_config32(nb_dev, reg_pos);
	reg &= ~mask;
	reg |= val;
	if (reg != reg_old) {
		pci_write_config32(nb_dev, reg_pos, reg);
	}
}

void set_nbcfg_enable_bits_8(struct pci_dev * nb_dev, u32 reg_pos, u8 mask, u8 val)
{
	u8 reg_old, reg;
	reg = reg_old = pci_read_config8(nb_dev, reg_pos);
	reg &= ~mask;
	reg |= val;
	if (reg != reg_old) {
		pci_write_config8(nb_dev, reg_pos, reg);
	}
}

void set_nbmc_enable_bits(struct pci_dev * nb_dev, u32 reg_pos, u32 mask, u32 val)
{
	u32 reg_old, reg;
	reg = reg_old = nbmc_read_index(nb_dev, reg_pos);
	reg &= ~mask;
	reg |= val;
	if (reg != reg_old) {
		nbmc_write_index(nb_dev, reg_pos, reg);
	}
}

void set_htiu_enable_bits(struct pci_dev * nb_dev, u32 reg_pos, u32 mask, u32 val)
{
	u32 reg_old, reg;
	reg = reg_old = htiu_read_index(nb_dev, reg_pos);
	reg &= ~mask;
	reg |= val;
	if (reg != reg_old) {
		htiu_write_index(nb_dev, reg_pos, reg);
	}
}

void set_nbmisc_enable_bits(struct pci_dev * nb_dev, u32 reg_pos, u32 mask, u32 val)
{
	u32 reg_old, reg;
	reg = reg_old = nbmisc_read_index(nb_dev, reg_pos);
	reg &= ~mask;
	reg |= val;
	if (reg != reg_old) {
		nbmisc_write_index(nb_dev, reg_pos, reg);
	}
}

void set_pcie_enable_bits(struct pci_dev * dev, u32 reg_pos, u32 mask, u32 val)
{
	u32 reg_old, reg;
	reg = reg_old = nb_read_index(dev, NBPCIE_INDEX, reg_pos);
	reg &= ~mask;
	reg |= val;
	if (reg != reg_old) {
		nb_write_index(dev, NBPCIE_INDEX, reg_pos, reg);
	}
}
