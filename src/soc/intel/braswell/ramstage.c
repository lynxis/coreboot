/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 * Copyright (C) 2015 Intel Corp.
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

#include <arch/cpu.h>
#include <arch/acpi.h>
#include <bootstate.h>
#include <cbmem.h>
#include <console/console.h>
#include <cpu/intel/microcode.h>
#include <cpu/x86/cr.h>
#include <cpu/x86/msr.h>
#include <device/device.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <fsp_util.h>
#include <romstage_handoff.h>
#include <soc/gpio.h>
#include <soc/lpc.h>
#include <soc/msr.h>
#include <soc/nvs.h>
#include <soc/pattrs.h>
#include <soc/pci_devs.h>
#include <soc/pm.h>
#include <soc/ramstage.h>
#include <soc/intel/common/ramstage.h>
#include <boardid.h>
#include <stdlib.h>

#define SHOW_PATTRS 1

struct pattrs __global_pattrs;

static void detect_num_cpus(struct pattrs *attrs)
{
	int ecx = 0;

	while (1) {
		struct cpuid_result leaf_b;

		leaf_b = cpuid_ext(0xb, ecx);

		/*
		 * The SOC doesn't have hyperthreading so just determine the
		 * number of cores by from level type (ecx[15:8] == * 2).
		 */
		if ((leaf_b.ecx & 0xff00) == 0x0200) {
			attrs->num_cpus = leaf_b.ebx & 0xffff;
			break;
		}
		ecx++;
	}
}

static inline void fill_in_msr(msr_t *msr, int idx)
{
	*msr = rdmsr(idx);
	if (SHOW_PATTRS) {
		printk(BIOS_DEBUG, "msr(%x) = %08x%08x\n",
		       idx, msr->hi, msr->lo);
	}
}

static const char * const stepping_str[] = {
	"A0", "A1", "B0", "B1", "B2", "B3", "C0"
};

static void fill_in_pattrs(void)
{
	device_t dev;
	msr_t msr;
	struct pattrs *attrs = (struct pattrs *)pattrs_get();

	attrs->cpuid = cpuid_eax(1);
	dev = dev_find_slot(0, PCI_DEVFN(LPC_DEV, LPC_FUNC));
	attrs->revid = pci_read_config8(dev, REVID);
	/* The revision to stepping IDs have two values per metal stepping. */
	if (attrs->revid >= RID_C_STEPPING_START) {
		attrs->stepping = (attrs->revid - RID_C_STEPPING_START) / 2;
		attrs->stepping += STEP_C0;
	} else if (attrs->revid >= RID_B_STEPPING_START) {
		attrs->stepping = (attrs->revid - RID_B_STEPPING_START) / 2;
		attrs->stepping += STEP_B0;
	} else {
		attrs->stepping = (attrs->revid - RID_A_STEPPING_START) / 2;
		attrs->stepping += STEP_A0;
	}

	attrs->microcode_patch = intel_microcode_find();
	attrs->address_bits = cpuid_eax(0x80000008) & 0xff;
	detect_num_cpus(attrs);

	if (SHOW_PATTRS) {
		printk(BIOS_DEBUG, "Cpuid %08x cpus %d rid %02x step %s\n",
		       attrs->cpuid, attrs->num_cpus, attrs->revid,
		       (attrs->stepping >= ARRAY_SIZE(stepping_str)) ? "??" :
		       stepping_str[attrs->stepping]);
	}

	fill_in_msr(&attrs->platform_id, MSR_IA32_PLATFORM_ID);
	fill_in_msr(&attrs->platform_info, MSR_PLATFORM_INFO);

	/* Set IA core speed ratio and voltages */
	fill_in_msr(&msr, MSR_IACORE_RATIOS);
	attrs->iacore_ratios[IACORE_MIN] = msr.lo & 0x7f;
	attrs->iacore_ratios[IACORE_LFM] = (msr.lo >> 8) & 0x7f;
	attrs->iacore_ratios[IACORE_MAX] = (msr.lo >> 16) & 0x7f;
	fill_in_msr(&msr, MSR_IACORE_TURBO_RATIOS);
	attrs->iacore_ratios[IACORE_TURBO] = (msr.lo & 0xff); /* 1 core max */

	fill_in_msr(&msr, MSR_IACORE_VIDS);
	attrs->iacore_vids[IACORE_MIN] = msr.lo & 0x7f;
	attrs->iacore_vids[IACORE_LFM] = (msr.lo >> 8) & 0x7f;
	attrs->iacore_vids[IACORE_MAX] = (msr.lo >> 16) & 0x7f;
	fill_in_msr(&msr, MSR_IACORE_TURBO_VIDS);
	attrs->iacore_vids[IACORE_TURBO] = (msr.lo & 0xff); /* 1 core max */

	/* Set bus clock speed */
	attrs->bclk_khz = BUS_FREQ_KHZ;
}

static inline void set_acpi_sleep_type(int val)
{
#if IS_ENABLED(CONFIG_HAVE_ACPI_RESUME)
	acpi_slp_type = val;
#endif
}

/* Save bit index for first enabled event in PM1_STS for \_SB._SWS */
static void s3_save_acpi_wake_source(global_nvs_t *gnvs)
{
	struct chipset_power_state *ps = cbmem_find(CBMEM_ID_POWER_STATE);
	uint16_t pm1;

	if (!ps)
		return;

	pm1 = ps->pm1_sts & ps->pm1_en;

	/* Scan for first set bit in PM1 */
	for (gnvs->pm1i = 0; gnvs->pm1i < 16; gnvs->pm1i++) {
		if (pm1 & 1)
			break;
		pm1 >>= 1;
	}

	/* If unable to determine then return -1 */
	if (gnvs->pm1i >= 16)
		gnvs->pm1i = -1;

	printk(BIOS_DEBUG, "ACPI System Wake Source is PM1 Index %d\n",
	       gnvs->pm1i);
}

static void s3_resume_prepare(void)
{
	global_nvs_t *gnvs;
	struct romstage_handoff *romstage_handoff;

	gnvs = cbmem_add(CBMEM_ID_ACPI_GNVS, sizeof(global_nvs_t));

	romstage_handoff = cbmem_find(CBMEM_ID_ROMSTAGE_INFO);
	if (romstage_handoff == NULL || romstage_handoff->s3_resume == 0) {
		if (gnvs != NULL)
			memset(gnvs, 0, sizeof(global_nvs_t));
		set_acpi_sleep_type(0);
		return;
	}

	set_acpi_sleep_type(3);

	s3_save_acpi_wake_source(gnvs);
}

static void set_board_id(void)
{
	global_nvs_t *gnvs;

	gnvs = cbmem_find(CBMEM_ID_ACPI_GNVS);
	if (!gnvs) {
		printk(BIOS_ERR, "Unable to locate Global NVS\n");
		return;
	}
	gnvs->bdid = board_id();
}

void soc_init_pre_device(struct soc_intel_braswell_config *config)
{
	struct soc_gpio_config *gpio_config;

	fill_in_pattrs();

	/* Allow for SSE instructions to be executed. */
	write_cr4(read_cr4() | CR4_OSFXSR | CR4_OSXMMEXCPT);

	/* Indicate S3 resume to rest of ramstage. */
	s3_resume_prepare();

	/* Perform silicon specific init. */
	intel_silicon_init();

	set_board_id();
	/* Get GPIO initial states from mainboard */
	gpio_config = mainboard_get_gpios();
	setup_soc_gpios(gpio_config, config->enable_xdp_tap);
}
