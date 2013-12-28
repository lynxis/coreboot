static void dumpmmcreg(struct sunxi_mmc *reg)
{
	SPEW("dump mmc registers:\n");
	SPEW("gctrl     0x%08x\t", reg->gctrl);
	SPEW("clkcr     0x%08x\n", reg->clkcr);
	SPEW("timeout   0x%08x\t", reg->timeout);
	SPEW("width     0x%08x\n", reg->width);
	SPEW("blksz     0x%08x\t", reg->blksz);
	SPEW("bytecnt   0x%08x\n", reg->bytecnt);
	SPEW("cmd       0x%08x\t", reg->cmd);
	SPEW("arg       0x%08x\n", reg->arg);
	SPEW("resp0     0x%08x\t", reg->resp0);
	SPEW("resp1     0x%08x\n", reg->resp1);
	SPEW("resp2     0x%08x\t", reg->resp2);
	SPEW("resp3     0x%08x\n", reg->resp3);
	SPEW("imask     0x%08x\t", reg->imask);
	SPEW("mint      0x%08x\n", reg->mint);
	SPEW("rint      0x%08x\t", reg->rint);
	SPEW("status    0x%08x\n", reg->status);
	SPEW("ftrglevel 0x%08x\t", reg->ftrglevel);
	SPEW("funcsel   0x%08x\n", reg->funcsel);
	SPEW("dmac      0x%08x\t", reg->dmac);
	SPEW("dlba      0x%08x\n", reg->dlba);
	SPEW("idst      0x%08x\t", reg->idst);
	SPEW("idie      0x%08x\n", reg->idie);
}

static int gb_mmc_resource_init(int sdc_no)
{
	struct sunxi_mmc_host *mmchost = &mmc_host[sdc_no];
	struct a10_ccm *ccm = (void *)A1X_CCM_BASE;

	SPEW("init mmc %d resource\n", sdc_no);

	switch (sdc_no) {
		case 0:
			mmchost->reg = (struct sunxi_mmc *)A1X_MMC0_BASE;
			mmchost->mclkreg = &ccm->sd0_clk_cfg;
			break;
		default:
			DEBUG("Wrong mmc number %d\n", sdc_no);
			return -1;
	}
	mmchost->database = (unsigned int)mmchost->reg + 0x100;
	mmchost->mmc_no = sdc_no;

	return 0;
}

static void gb_mmc_ungate_clock(int sdc_no)
{
	a1x_periph_clock_enable(A1X_CLKEN_MMC0 + sdc_no);

}

static void gb_mmc_mux_pins(int sdc_no)
{
	struct a10_gpio *const pio = (void *)A1X_PIO_BASE;
	struct a10_gpio_port *gpio_f = &pio->port[GPF];

	/* config gpio */
	switch (sdc_no) {
		case 0:
			/* D1-PF0, D0-PF1, CLK-PF2, CMD-PF3, D3-PF4, D4-PF5 */
			write32(0x555, &gpio_f->pul[0]);
			write32(0xaaa, &gpio_f->drv[0]);
			break;
		default:
			return;
	}
}
