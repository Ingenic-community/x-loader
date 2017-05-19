/*
 * Jz4740 UART support
 * Copyright (c) 2011
 * Qi Hardware, Xiangfu Liu <xiangfu@sharism.cc>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <generated/uart_baudrate_reg_values.h>

struct jz_uart *uart;

#ifdef CONFIG_CONSOLE_ENABLE
static void uart_putc(const char c) {
    if (c == '\n')
        uart_putc('\r');

    writeb((uint8_t)c, &uart->rbr_thr_dllr);

    /* Wait for fifo to shift out some bytes */
    while (!((readb(&uart->lsr) & (UART_LSR_TDRQ | UART_LSR_TEMT)) == 0x60));
}
#endif

void uart_init(void) {
    uint8_t tmp;

    uart = (struct jz_uart *)(UART0_BASE + CONFIG_CONSOLE_INDEX * 0x1000);

    /* init uart gpio */
    console_set_gpio();

    /* open uart clk gate */
    enable_uart_clk();

    /* Disable port interrupts while changing hardware */
    writeb(0, &uart->dlhr_ier);

    /* Disable UART unit function */
    writeb(~UART_FCR_UUE, &uart->iir_fcr);

    /* Set both receiver and transmitter in UART mode (not SIR) */
    writeb(~(SIRCR_RSIRE | SIRCR_TSIRE), &uart->isr);

    /*
     * Set databits, stopbits and parity.
     * (8-bit data, 1 stopbit, no parity)
     */
    writeb(UART_LCR_WLEN_8 | UART_LCR_STOP_1, &uart->lcr);

    /* Set baud rate */
    tmp = readb(&uart->lcr);
    tmp |= UART_LCR_DLAB;
    writeb(tmp, &uart->lcr);

    writeb((UART_BAUDRATE_DIV_BEST >> 8) & 0xff, &uart->dlhr_ier);
    writeb(UART_BAUDRATE_DIV_BEST & 0xff, &uart->rbr_thr_dllr);
    writeb(UART_UMR_BEST, &uart->umr);
    writeb(UART_UACR_BEST, &uart->uacr);

    tmp &= ~UART_LCR_DLAB;
    writeb(tmp, &uart->lcr);

    /* Enable UART unit, enable and clear FIFO */
    writeb(UART_FCR_UUE | UART_FCR_FE | UART_FCR_TFLS | UART_FCR_RFLS,
           &uart->iir_fcr);
}

void uart_puts(const char* s) {
#ifdef CONFIG_CONSOLE_ENABLE
    while (*s)
        uart_putc(*s++);
#endif
}

void uart_put_hex_n(uint32_t d) {
        char c[9];
        uint32_t i;
        for (i = 0; i < 8; i++) {
                c[i] = (d >> ((7 - i) * 4)) & 0xf;
                if (c[i] < 10)
                        c[i] += 0x30;
                else
                        c[i] += (0x41 - 10);
        }
        c[8] = 0;
        uart_puts(c);
}

void uart_put_hex(uint32_t d) {
        uart_put_hex_n(d);
        uart_puts("\n");
}
