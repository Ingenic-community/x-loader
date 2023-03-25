/*
 *  Copyright (C) 2016 Ingenic Semiconductor Co.,Ltd
 *
 *  X1000 series bootloader for u-boot/rtos/linux
 *
 *  Zhang YanMing <yanming.zhang@ingenic.com, jamincheung@126.com>
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

#define src_readl(offset)   readl(SFC_BASE + offset)
#define sfc_writel(value, offset)   writel(value, SFC_BASE + offset);

void sfc_set_mode(uint32_t channel, uint32_t value);
void sfc_dev_addr_dummy_bits(uint32_t channel, uint32_t value);
void sfc_transfer_direction(uint32_t value);
void sfc_set_length(uint32_t value);
void sfc_set_addr_length(uint32_t channel, uint32_t value);
void sfc_cmd_en(uint32_t channel, uint32_t value);
void sfc_data_en(uint32_t channel, uint32_t value);
void sfc_write_cmd(uint32_t channel, uint32_t value);
void sfc_dev_addr(uint32_t channel, uint32_t value);
void sfc_dev_addr_plus(uint32_t channel, uint32_t value);
void sfc_set_transfer(struct jz_sfc *sfc, uint32_t dir);
void sfc_send_cmd(struct jz_sfc *sfc, uint8_t dir);


inline void sfc_set_mode(uint32_t channel, uint32_t value) {
	uint32_t tmp;

	tmp = src_readl(SFC_TRAN_CONF(channel));
	tmp &= ~(TRAN_MODE_MSK);
	tmp |= (value << TRAN_MODE_OFFSET);
	sfc_writel(tmp, SFC_TRAN_CONF(channel));
}

inline void sfc_dev_addr_dummy_bits(uint32_t channel, uint32_t value) {
	uint32_t tmp;

	tmp = src_readl(SFC_TRAN_CONF(channel));
	tmp &= ~TRAN_CONF_DMYBITS_MSK;
	tmp |= (value << TRAN_CONF_DMYBITS_OFFSET);
	sfc_writel(tmp, SFC_TRAN_CONF(channel));
}

inline void sfc_transfer_direction(uint32_t value) {
	uint32_t tmp;

	tmp = src_readl(SFC_GLB);

	if(value == 0)
		tmp &= ~TRAN_DIR;
	else
		tmp |= TRAN_DIR;

	sfc_writel(tmp, SFC_GLB);
}

inline void sfc_set_length(uint32_t value) {
	sfc_writel(value, SFC_TRAN_LEN);
}

inline void sfc_set_addr_length(uint32_t channel, uint32_t value) {
	uint32_t tmp;

	tmp = src_readl(SFC_TRAN_CONF(channel));
	tmp &= ~(ADDR_WIDTH_MSK);
	tmp |= (value << ADDR_WIDTH_OFFSET);
	sfc_writel(tmp, SFC_TRAN_CONF(channel));
}

inline void sfc_cmd_en(uint32_t channel, uint32_t value) {
	uint32_t tmp;

	tmp = src_readl(SFC_TRAN_CONF(channel));

	if(value == 1)
		tmp |= CMDEN;
	else
		tmp &= ~CMDEN;

	sfc_writel(tmp, SFC_TRAN_CONF(channel));
}

inline void sfc_data_en(uint32_t channel, uint32_t value) {
	uint32_t tmp;

	tmp = src_readl(SFC_TRAN_CONF(channel));

	if(value == 1)
		tmp |= DATEEN;
	else
		tmp &= ~DATEEN;

	sfc_writel(tmp, SFC_TRAN_CONF(channel));
}

inline void sfc_write_cmd(uint32_t channel, uint32_t value) {
	uint32_t tmp;

	tmp = src_readl(SFC_TRAN_CONF(channel));
	tmp &= ~CMD_MSK;
	tmp |= value;
	sfc_writel(tmp, SFC_TRAN_CONF(channel));
}

inline void sfc_dev_addr(uint32_t channel, uint32_t value) {
	sfc_writel(value, SFC_DEV_ADDR(channel));
}

inline void sfc_dev_addr_plus(uint32_t channel, uint32_t value) {
	sfc_writel(value, SFC_DEV_ADDR_PLUS(channel));
}

inline void sfc_set_transfer(struct jz_sfc *sfc, uint32_t dir) {
	if(dir == 1)
		sfc_transfer_direction(GLB_TRAN_DIR_WRITE);
	else
		sfc_transfer_direction(GLB_TRAN_DIR_READ);

	sfc_set_length(sfc->len);
	sfc_set_addr_length(0, sfc->addr_len);
	sfc_cmd_en(0, 0x1);
	sfc_data_en(0, sfc->daten);
	sfc_write_cmd(0, sfc->cmd);
	sfc_dev_addr(0, sfc->addr);
	sfc_dev_addr_plus(0, sfc->addr_plus);
	sfc_dev_addr_dummy_bits(0, sfc->dummy_bits);
	sfc_set_mode(0, sfc->sfc_mode);
}

inline void sfc_send_cmd(struct jz_sfc *sfc, uint8_t dir) {
	uint32_t reg_tmp = 0;

	sfc_set_transfer(sfc, dir);
	sfc_writel(1 << 2, SFC_TRIG);
	sfc_writel(START, SFC_TRIG);

	/*this must judge the end status*/
	if((sfc->daten == 0)){
		reg_tmp = src_readl(SFC_SR);
		while (!(reg_tmp & END))
			reg_tmp = src_readl(SFC_SR);

		if ((src_readl(SFC_SR)) & END)
			sfc_writel(CLR_END, SFC_SCR);
	}
}

int sfc_write_data(uint32_t *data, uint32_t length) {
	uint32_t tmp_len = 0;
	uint32_t fifo_num = 0;
	uint32_t reg_tmp = 0;
	uint32_t len = (length + 3) / 4 ;

	while(1) {
		reg_tmp = src_readl(SFC_SR);
		if (reg_tmp & TRAN_REQ) {
			sfc_writel(CLR_TREQ,SFC_SCR);
			if ((len - tmp_len) > THRESHOLD)
				fifo_num = THRESHOLD;
			else
				fifo_num = len - tmp_len;

			for (int i = 0; i < fifo_num; i++) {
				sfc_writel(*data, SFC_DR);
				data++;
				tmp_len++;
			}
		}

		if (tmp_len == len)
			break;
	}

	reg_tmp = src_readl(SFC_SR);
	while (!(reg_tmp & END))
		reg_tmp = src_readl(SFC_SR);

	if ((src_readl(SFC_SR)) & END)
		sfc_writel(CLR_END, SFC_SCR);

	return 0;
}

int sfc_read_data(uint32_t *data, uint32_t length) {
	uint32_t tmp_len = 0;
	uint32_t fifo_num = 0;
	uint32_t reg_tmp = 0;
	uint32_t len = (length + 3) / 4 ;

	while(1){
		reg_tmp = src_readl(SFC_SR);
		if (reg_tmp & RECE_REQ) {
			sfc_writel(CLR_RREQ, SFC_SCR);
			if ((len - tmp_len) > THRESHOLD)
				fifo_num = THRESHOLD;
			else
				fifo_num = len - tmp_len;

			for (int i = 0; i < fifo_num; i++) {
				*data = src_readl(SFC_DR);
				data++;
				tmp_len++;
			}
		}
		if (tmp_len == len)
			break;
	}

	reg_tmp = src_readl(SFC_SR);
	while (!(reg_tmp & END))
		reg_tmp = src_readl(SFC_SR);

	if ((src_readl(SFC_SR)) & END)
		sfc_writel(CLR_END, SFC_SCR);

	return 0;
}

static unsigned int sfc_timing_calc() {
	unsigned int c_hold;
	unsigned int c_setup;
	unsigned int t_in, c_in, val = 0;
	unsigned long cycle;
	unsigned int tmp = 0x7;
	unsigned int rate =  CONFIG_SFC_FREQ;

	cycle = 1000 / rate;

	c_hold = DEF_TCHSH / cycle;
	if(c_hold > 0)
		val = c_hold - 1;
	tmp &= ~THOLD_MSK;
	tmp |= val << THOLD_OFFSET;

	c_setup = DEF_TSLCH / cycle;
	if(c_setup > 0)
		val = c_setup - 1;
	tmp &= ~TSETUP_MSK;
	tmp |= val << TSETUP_OFFSET;

	if (DEF_TSHSL_R >= DEF_TSHSL_W)
		t_in = DEF_TSHSL_R;
	else
		t_in = DEF_TSHSL_W;
	c_in = t_in / cycle;
	if(c_in > 0)
		val = c_in - 1;
	tmp &= ~TSH_MSK;
	tmp |= val << TSH_OFFSET;

	if(rate >= 100){
		val = 1;
		tmp &= ~SMP_DELAY_MSK;
		tmp |= val << SMP_DELAY_OFFSET;
	}

	return tmp;
}

void sfc_init(void) {
	uint32_t tmp;

	sfc_set_gpio_pa_as_6bit();

	set_sfc_freq(CONFIG_SFC_FREQ * 1000 * 1000);

	tmp = src_readl(SFC_GLB);
	tmp &= ~(TRAN_DIR | OP_MODE );
	tmp |= WP_EN;
	sfc_writel(tmp, SFC_GLB);

	/*
	 * Set sfc timing
	 */
	sfc_writel(sfc_timing_calc(), SFC_DEV_CONF);

	for (int i = 0; i < 6; i++)
		sfc_writel((src_readl(SFC_TRAN_CONF(i)) & (~(TRAN_MODE_MSK | FMAT))),
			   SFC_TRAN_CONF(i));

	sfc_writel((CLR_END | CLR_TREQ | CLR_RREQ | CLR_OVER | CLR_UNDER),
		   SFC_INTC);

	sfc_writel(0, SFC_CGE);

	tmp = src_readl(SFC_GLB);
	tmp &= ~(THRESHOLD_MSK);
	tmp |= (THRESHOLD << THRESHOLD_OFFSET);
	sfc_writel(tmp, SFC_GLB);
}
