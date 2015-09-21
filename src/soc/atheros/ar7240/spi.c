/*
 * This file is part of the coreboot project.
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
 */

#include <soc/ar71xx_regs.h>
#include <soc/spi.h>
#include <spi_flash.h>
#include <spi-generic.h>
#include <stdlib.h>
#include <string.h>
#include <timer.h>


/* based on u-boot sources (GPL) */

#define AR7240_SPI_FS           0x1f000000
#define AR7240_SPI_CLOCK        0x1f000004
#define AR7240_SPI_WRITE        0x1f000008
#define AR7240_SPI_READ         0x1f000000
#define AR7240_SPI_RD_STATUS    0x1f00000c

#define AR7240_SPI_CS_DIS       0x70000
#define AR7240_SPI_CE_LOW       0x60000
#define AR7240_SPI_CE_HIGH      0x60100

#define AR7240_SPI_CMD_WREN         0x06
#define AR7240_SPI_CMD_RD_STATUS    0x05
#define AR7240_SPI_CMD_FAST_READ    0x0b
#define AR7240_SPI_CMD_PAGE_PROG    0x02
#define AR7240_SPI_CMD_SECTOR_ERASE 0xd8

#define AR7240_SPI_SECTOR_SIZE      (1024*64)
#define AR7240_SPI_PAGE_SIZE        256

#define ar7240_be_msb(_val, _i) (((_val) & (1 << (7 - _i))) >> (7 - _i))

#define ar7240_spi_bit_banger(_byte)  do {        \
    int i;                                      \
    for(i = 0; i < 8; i++) {                    \
        ar7240_reg_wr_nf(AR7240_SPI_WRITE,      \
                        AR7240_SPI_CE_LOW | ar7240_be_msb(_byte, i));  \
        ar7240_reg_wr_nf(AR7240_SPI_WRITE,      \
                        AR7240_SPI_CE_HIGH | ar7240_be_msb(_byte, i)); \
    }       \
}while(0);

#define ar7240_spi_go() do {        \
    ar7240_reg_wr_nf(AR7240_SPI_WRITE, AR7240_SPI_CE_LOW); \
    ar7240_reg_wr_nf(AR7240_SPI_WRITE, AR7240_SPI_CS_DIS); \
}while(0);

#define ar7240_spi_send_addr(_addr) do {                    \
    ar7240_spi_bit_banger(((addr & 0xff0000) >> 16));                 \
    ar7240_spi_bit_banger(((addr & 0x00ff00) >> 8));                 \
    ar7240_spi_bit_banger(addr & 0x0000ff);                 \
}while(0);

#define ar7240_spi_delay_8()    ar7240_spi_bit_banger(0)
#define ar7240_spi_done()       ar7240_reg_wr_nf(AR7240_SPI_FS, 0)

#define ATHEROS_MAX_SPI_SLAVE 3

static struct spi_slave slaves[ATHEROS_MAX_SPI_SLAVE];
static int spi_initialized;

static inline void ath79_spi_wr(unsigned long addr, uint32_t val)
{
	write32(AR71XX_SPI_BASE + addr, val);
}

static inline uint32_t ath79_spi_rr(unsigned long addr)
{
	return read32(AR71XX_SPI_BASE + addr);
}

void spi_init(void)
{
	spi_initialized = 0;
	memset(slaves, 0, sizeof(slaves));
}

struct spi_slave *spi_setup_slave(unsigned int bus, unsigned int cs)
{
	if (bus != 0 || cs >= ATHEROS_MAX_SPI_SLAVE) {
		printk(BIOS_ERR, "%s: Invalid spi settings bus 0x%x cs 0x%x\n",
				__func__, bus, cs);
		return NULL;
	}

	slaves[cs].bus = bus;
	slaves[cs].cs = cs;
	slaves[cs].rw = SPI_READ_FLAG & SPI_WRITE_FLAG;

	return &slaves[cs];
}

int spi_claim_bus(struct spi_slave *slave)
{
	
	if (!spi_initialized) {
		spi_initialized = 1;

		/* enable gpio mode */
		ath79_spi_wr(AR71XX_SPI_REG_FS, AR71XX_SPI_FS_GPIO);

		/* disable remap (0x40) + set up clock divider 0x3
		* freq = AHB_CLK / ((CLOCK_DIVIDE +1) * 2)
		*/
		ath79_spi_wr(AR71XX_SPI_REG_CTRL, 0x43);
	}

	return 0;
}

void spi_release_bus(struct spi_slave *slave)
{
}

int spi_xfer(struct spi_slave *slave, const void *dout, unsigned int bytesout,
		void *din, unsigned int bytesin)
{
	int csbit = slave->cs + 16;
	int transfer_max = max(bytesin, bytesout);
	int readed = 0;
	uint8_t data = 0;
	
	ath79_spi_wr(AR71XX_SPI_REG_IOC, csbit);

	for(int i=0; i<transfer_max; i++) {
		if(bytesout <= i)
			data = 0; /* dummy write to read data */
		else
			data = *(((uint8_t *)dout) + i);

		/* write data 8 bit data */
		for(int j=0; j<8; j++) {
			ath79_spi_wr(AR71XX_SPI_REG_IOC,
					csbit | ar7240_be_msb(data, j));
			ath79_spi_wr(AR71XX_SPI_REG_IOC,
					csbit | AR71XX_SPI_IOC_CLK | ar7240_be_msb(data, j));
		}

		/* read data in from data register */
		if(i % 4 == 0 && i > 0) {
			if(bytesin == readed) {
				/* do nothing */
			} else if(bytesin-readed >= 4) {
				*(((uint32_t *)din) + (readed % 4)) = ath79_spi_rr(AR71XX_SPI_REG_RDS);
				readed += 4;
			} else {
				/* partial read until full. max 3 bytes are read */
				for (int j=0; bytesin != readed; j++) {
					*(((uint32_t *)din) + readed) = (readed >> (24 - j*8));
					readed++;
				}
			}
		}
	}

	/* check if we have to read_data once more */
	if (bytesin != readed) {
		/* partial read until full. max 3 bytes are read */
		for (int j=0; bytesin != readed; j++) {
			*(((uint32_t *)din) + readed) = (readed >> (24 - j*8));
			readed++;
		}
	}


	return 0;
}

unsigned int spi_crop_chunk(unsigned int cmd_len, unsigned int buf_len)
{
	return buf_len;
}

