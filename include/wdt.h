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

#ifndef __WDT_H__
#define __WDT_H__

#define TCU_TSSR    0x2c
#define TCU_TSCR    0x3c

#define WDT_TCSR    0xc
#define WDT_TCER    0x4
#define WDT_TDR     0x0
#define WDT_TCNT    0x8

#define TSSR_WDTSS  (1 << 16)
#define TSCR_WDTSC  (1 << 16)

#define TCSR_PRESCALE_1     (0 << 3)
#define TCSR_PRESCALE_4     (1 << 3)
#define TCSR_PRESCALE_16    (2 << 3)
#define TCSR_PRESCALE_64    (3 << 3)
#define TCSR_PRESCALE_256   (4 << 3)
#define TCSR_PRESCALE_1024  (5 << 3)

#define TCSR_EXT_EN (1 << 2)
#define TCSR_RTC_EN (1 << 1)
#define TCSR_PCK_EN (1 << 0)

#define TCER_TCEN   (1 << 0)

void wdt_init(void);
void wdt_stop(void);
void wdt_feed(void);
void wdt_restart(void);

#endif /* __WDT_H__ */
