/*
 * resource/res-uart.c
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
#include <s5p6818-uart.h>

static struct s5p6818_uart_data_t s5p6818_uart_data[] = {
	[0] = {
		.baud		= 115200,
		.data		= 8,
		.parity		= 0,
		.stop		= 1,
		.regbase	= S5P6818_UART0_BASE,
	},
	[1] = {
		.baud		= 115200,
		.data		= 8,
		.parity		= 0,
		.stop		= 1,
		.regbase	= S5P6818_UART1_BASE,
	},
	[2] = {
		.baud		= 115200,
		.data		= 8,
		.parity		= 0,
		.stop		= 1,
		.regbase	= S5P6818_UART2_BASE,
	},
	[3] = {
		.baud		= 115200,
		.data		= 8,
		.parity		= 0,
		.stop		= 1,
		.regbase	= S5P6818_UART3_BASE,
	},
	[4] = {
		.baud		= 115200,
		.data		= 8,
		.parity		= 0,
		.stop		= 1,
		.regbase	= S5P6818_UART4_BASE,
	},
	[5] = {
		.baud		= 115200,
		.data		= 8,
		.parity		= 0,
		.stop		= 1,
		.regbase	= S5P6818_UART5_BASE,
	}
};

static struct resource_t res_uarts[] = {
	{
		.name		= "s5p6818-uart",
		.id			= 0,
		.data		= &s5p6818_uart_data[0],
	}, {
		.name		= "s5p6818-uart",
		.id			= 1,
		.data		= &s5p6818_uart_data[1],
	}, {
		.name		= "s5p6818-uart",
		.id			= 2,
		.data		= &s5p6818_uart_data[2],
	}, {
		.name		= "s5p6818-uart",
		.id			= 3,
		.data		= &s5p6818_uart_data[3],
	}, {
		.name		= "s5p6818-uart",
		.id			= 4,
		.data		= &s5p6818_uart_data[4],
	}, {
		.name		= "s5p6818-uart",
		.id			= 5,
		.data		= &s5p6818_uart_data[5],
	}
};

static __init void resource_uart_init(void)
{
	int i;

	for(i = 0; i < ARRAY_SIZE(res_uarts); i++)
	{
		if(register_resource(&res_uarts[i]))
			LOG("Register resource %s:'%s.%d'", res_uarts[i].mach, res_uarts[i].name, res_uarts[i].id);
		else
			LOG("Failed to register resource %s:'%s.%d'", res_uarts[i].mach, res_uarts[i].name, res_uarts[i].id);
	}
}

static __exit void resource_uart_exit(void)
{
	int i;

	for(i = 0; i < ARRAY_SIZE(res_uarts); i++)
	{
		if(unregister_resource(&res_uarts[i]))
			LOG("Unregister resource %s:'%s.%d'", res_uarts[i].mach, res_uarts[i].name, res_uarts[i].id);
		else
			LOG("Failed to unregister resource %s:'%s.%d'", res_uarts[i].mach, res_uarts[i].name, res_uarts[i].id);
	}
}

resource_initcall(resource_uart_init);
resource_exitcall(resource_uart_exit);
