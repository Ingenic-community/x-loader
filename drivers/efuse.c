/*
 *  Copyright (C) 2016 Ingenic Semiconductor Co.,Ltd
 *
 *  X1000 series bootloader for u-boot/rtos/linux
 *
 *  Wang Qiuwei <qiuwei.wang@ingenic.com>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under  the terms of the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the License, or (at your
 *  option) any later version.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include <common.h>

#define EFUSE_CTRL		0x0
#define EFUSE_CFG		0x4
#define EFUSE_STATE		0x8
#define EFUSE_DATA(n)   (0xC + (n)*4)

struct efuse_cfg {
	uint32_t rd_adj;
	uint32_t wr_adj;
	uint32_t rd_strobe;
	uint32_t wr_strobe;
};

static void efuse_adjust_cfg(struct efuse_cfg *cfg) {
	int i;
	uint32_t val, ns;
	uint32_t h2clk;
	uint32_t pll_rate = CONFIG_DDR_SEL_PLL == APLL ? CONFIG_APLL_FREQ : CONFIG_MPLL_FREQ;

	h2clk = pll_rate / CONFIG_AHB_CLK_DIV * 1000000;
	ns = 1000000000 / h2clk;

	for(i = 0; i < 0x4; i++)
		if(((i + 1) * ns) > 2)
			break;

	//assert((i < 0x4));
	cfg->rd_adj = cfg->wr_adj = i;

	for(i = 0; i < 0x8; i++)
		if(((cfg->rd_adj + i + 3) * ns) > 15)
			break;

	//assert((i < 0x8));
	cfg->rd_strobe = i;

	/*
	 * X-loader efuse driver not support to write efuse,
	 * so, don't need to calculate wr_adj and wr_strobe.
	 */
#if 0
	cfg->wr_adj = cfg->rd_adj;

    for(i = 0; i < 0x1f; i += 100) {
        val = (cfg->wr_adj + i + 916) * ns;
        if( val > 4 * 1000 && val < 6 *1000)
            break;
    }

    assert((i < 0x1f));
    cfg->wr_strobe = i;
#endif
}

static int check_vaild(uint32_t addr, int length)
{
    uint32_t length_bits = length * 8;

    if (addr >= CHIP_ID_ADDR && addr <= CHIP_ID_END) {
        debug("chip id\n");
        if (length_bits > CHIP_ID_SIZE)
            goto error;
    } else if (addr >= CUT_ID_ADDR && addr <= CUT_ID_END) {
        debug("customer id\n");
        if (length_bits > CUT_ID_SIZE)
            goto error;
    } else {
        debug("Invalid addr\n");
        return -1;
    }

    return 0;

error:
    debug("length too max\n");
    return -1;
}

int efuse_read(void *buf, uint32_t addr, int length) {
    int i;
    uint32_t *pbuf = buf;
    uint32_t val, word_num;

    if (check_vaild(addr, length))
        return -1;

    addr -= EFU_ROM_BASE;
    word_num = (length + 3) / 4;

    struct efuse_cfg efuse_cfg;

    efuse_adjust_cfg(&efuse_cfg);

    /* set efuse configure resister */
    val = efuse_cfg.rd_adj << 20 | efuse_cfg.rd_strobe << 16;
    efuse_writel(val, EFUSE_CFG);

    /* clear read done status */
    efuse_writel(0, EFUSE_STATE);

    /*indicat addr and length, enable read */
    val = addr << 21 | length << 16 | 1;
    efuse_writel(val, EFUSE_CTRL);

    /* wait read done status */
    while(!(efuse_readl(EFUSE_STATE) & 1));

    for(i = 0; i < word_num; i ++) {
         val = efuse_readl(EFUSE_DATA(i));
        *(pbuf + i) = val;
        debug("word%d: 0x%x\n", i, val);
    }

    return word_num;
}
