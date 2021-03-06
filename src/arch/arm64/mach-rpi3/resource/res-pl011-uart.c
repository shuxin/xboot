/*
 * resource/res-pl011-uart.c
 *
 * Copyright(c) 2007-2016 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <xboot.h>
#include <pl011-uart.h>
#include <bcm2837-gpio.h>
#include <bcm2837-irq.h>
#include <bcm2837/reg-uart.h>

static struct pl011_uart_data_t uart_datas[] = {
	[0] = {
		.clk	= "uart-clk",
		.txdpin	= BCM2837_GPIO(32),
		.txdcfg	= 3,
		.rxdpin	= BCM2837_GPIO(33),
		.rxdcfg	= 3,
		.baud	= 115200,
		.data	= 8,
		.parity	= 0,
		.stop	= 1,
		.phys	= BCM2837_UART0_BASE,
	}
};

static struct resource_t res_uarts[] = {
	{
		.name	= "pl011-uart",
		.id		= 0,
		.data	= &uart_datas[0],
	}
};

static __init void resource_uart_init(void)
{
	int i;

	for(i = 0; i < ARRAY_SIZE(res_uarts); i++)
		register_resource(&res_uarts[i]);
}
resource_initcall(resource_uart_init);
