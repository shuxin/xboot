/*
 * resource/res-key-gpio.c
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
#include <input/key-gpio.h>
#include <s5p6818-gpio.h>

static struct gpio_button_t buttons[] = {
	{
		.key = KEY_MENU,
		.gpio = S5P6818_GPIOB(9),
		.active_low = 1,
	}, {
		.key = KEY_UP,
		.gpio = S5P6818_GPIOB(31),
		.active_low = 1,
	}, {
		.key = KEY_DOWN,
		.gpio = S5P6818_GPIOB(30),
		.active_low = 1,
	}, {
		.key = KEY_ENTER,
		.gpio = S5P6818_GPIOA(28),
		.active_low = 1,
	}, {
		.key = KEY_POWER,
		.gpio = S5P6818_GPIOALV(0),
		.active_low = 1,
	},
};

static struct key_gpio_data_t key_gpio_data = {
	.buttons	= buttons,
	.nbutton	= ARRAY_SIZE(buttons),
};

static struct resource_t res_key_gpio = {
	.name		= "key-gpio",
	.id			= -1,
	.data		= &key_gpio_data,
};

static __init void resource_key_gpio_init(void)
{
	register_resource(&res_key_gpio);
}
resource_initcall(resource_key_gpio_init);
