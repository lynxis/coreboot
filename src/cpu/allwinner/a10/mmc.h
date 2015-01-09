/*
 * MMC register definition for allwinner SOCs
 *
 * Copyright (C) 2007-2011 Allwinner Technology Co., Ltd.
 * Copyright (C) 2013  Alexandru Gagniuc <mr.nuke.me@gmail.com>
 * Subject to the GNU GPL v2, or (at your option) any later version.
 */

#ifndef CPU_ALLWINNER_A10_MMC_H
#define CPU_ALLWINNER_A10_MMC_H

#include <types.h>
#include <drivers/mmc/mmc.h>

struct sunxi_mmc {
	u32 gctrl;		/* (0x00) SMC Global Control Register */
	u32 clkcr;		/* (0x04) SMC Clock Control Register */
	u32 timeout;		/* (0x08) SMC Time Out Register */
	u32 width;		/* (0x0c) SMC Bus Width Register */
	u32 blksz;		/* (0x10) SMC Block Size Register */
	u32 bytecnt;		/* (0x14) SMC Byte Count Register */
	u32 cmd;		/* (0x18) SMC Command Register */
	u32 arg;		/* (0x1c) SMC Argument Register */
	u32 resp0;		/* (0x20) SMC Response Register 0 */
	u32 resp1;		/* (0x24) SMC Response Register 1 */
	u32 resp2;		/* (0x28) SMC Response Register 2 */
	u32 resp3;		/* (0x2c) SMC Response Register 3 */
	u32 imask;		/* (0x30) SMC Interrupt Mask Register */
	u32 mint;		/* (0x34) SMC Masked Interrupt Status Reg */
	u32 rint;		/* (0x38) SMC Raw Interrupt Status Register */
	u32 status;		/* (0x3c) SMC Status Register */
	u32 ftrglevel;		/* (0x40) SMC FIFO Threshold Watermark Reg */
	u32 funcsel;		/* (0x44) SMC Function Select Register */
	u32 cbcr;		/* (0x48) SMC CIU Byte Count Register */
	u32 bbcr;		/* (0x4c) SMC BIU Byte Count Register */
	u32 dbgc;		/* (0x50) SMC Debug Enable Register */
	u32 res0[11];		/* (0x54~0x7c) */
	u32 dmac;		/* (0x80) SMC IDMAC Control Register */
	u32 dlba;		/* (0x84) SMC IDMAC Descr List Base Addr Reg */
	u32 idst;		/* (0x88) SMC IDMAC Status Register */
	u32 idie;		/* (0x8c) SMC IDMAC Interrupt Enable Register */
	u32 chda;		/* (0x90) */
	u32 cbda;		/* (0x94) */
	u32 res1[26];		/* (0x98~0xff) */
	u32 fifo;		/* (0x100) SMC FIFO Access Address */
};

int sunxi_mmc_init(int sdc_no, struct mmc **mmcp);
#endif /* CPU_ALLWINNER_A10_MMC_H */
