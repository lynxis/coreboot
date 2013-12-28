/*
 * CBFS accessors for bootblock stage.
 *
 * Copyright (C) 2013  Alexandru Gagniuc <mr.nuke.me@gmail.com>
 * Subject to the GNU GPL v2, or (at your option) any later version.
 */

#include "ram_segs.h"

#include <cbfs.h>
#include <config.h>
#include <console/console.h>
#include <string.h>
#include <cpu/allwinner/a10/mmc.h>
#include <lib.h>
#include <cbmem.h>

struct cbfs_mmc_context {
	struct mmc *mmc;
	void *cbfs_base;
};

static int dummy_open(struct cbfs_media *media)
{
	printk(BIOS_DEBUG, "default_media->open()\n");
	return 0;
}

static int dummy_close(struct cbfs_media *media)
{
	printk(BIOS_DEBUG, "default_media->close()\n");
	return 0;
}

static void *on_chip_memory_map(struct cbfs_media *media, size_t offset,
				 size_t count)
{
	printk(BIOS_DEBUG, "default_media->map(0x%x, 0x%x)\n", offset, count);
	struct cbfs_mmc_context *ctx = (void *)media->context;

	return ctx->cbfs_base + offset;
}

static void * dummy_unmap(struct cbfs_media *media, const void *address)
{
	return NULL;
}

static size_t mmc_card_read(struct cbfs_media *media, void *dest,
			  size_t offset, size_t count)
{
	struct cbfs_mmc_context *ctx = (void *)media->context;

	memcpy(dest, ctx->cbfs_base + offset, count);

	return count;
}

int init_default_cbfs_media(struct cbfs_media *media)
{
	/* It's OK to have a static context. We're in the bootblock. */
	static struct cbfs_mmc_context context;

	struct cbfs_header *header = (void *)CONFIG_BOOTBLOCK_BASE +
					     CONFIG_CBFS_HEADER_ROM_OFFSET;

	if (CBFS_HEADER_MAGIC != ntohl(header->magic)) {
		printk(BIOS_ERR, "Invalid CBFS master header at %p\n", header);
		//return -1;
	}

	static struct mmc *mmc;

	media->open = dummy_open;
	media->close = dummy_close;
	media->map = on_chip_memory_map;
	media->unmap = dummy_unmap;
	media->read = mmc_card_read;

	size_t rdr;
	size_t bs = 512;
	size_t start_block = (8 << 10) / bs;
	size_t n_blocks = (CONFIG_ROM_SIZE + 32 + bs - 1) / bs;
	void * dest = a1x_get_cbfs_cache_top() - n_blocks * bs;
	media->context = (void *)&context;
	context.cbfs_base = dest + 32;

	header = (void *)context.cbfs_base + CONFIG_CBFS_HEADER_ROM_OFFSET;
	if (CBFS_HEADER_MAGIC == ntohl(header->magic)) {
		printk(BIOS_ERR, "Already have CBFS master header at %p\n", header);
		return 0;
	}
	printk(BIOS_ERR, "No valid CBFS master header at %p\n", header);

	print_debug("Calling sunxi to init out MMC\n");
	sunxi_mmc_init(0, &mmc);

	context.mmc = mmc;

	print_debug("Calling generic MMC init\n");
	mmc_init(mmc);

	print_debug("Copying coreboot.rom to RAM\n");

	rdr = context.mmc->block_dev.block_read(0, start_block, n_blocks, dest);

	media->context = (void*)&context;

	return 0;
}
