/*
 * resource/res-pl031-rtc.c
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
#include <pl031-rtc.h>
#include <realview/reg-rtc.h>

static struct pl031_rtc_data_t rtc_datas[] = {
	{
		.phys	= REALVIEW_RTC0_BASE,
	}
};

static struct resource_t res_rtcs[] = {
	{
		.name	= "pl031-rtc",
		.id		= 0,
		.data	= &rtc_datas[0],
	}
};

static __init void resource_pl031_rtc_init(void)
{
	int i;

	for(i = 0; i < ARRAY_SIZE(res_rtcs); i++)
		register_resource(&res_rtcs[i]);
}
resource_initcall(resource_pl031_rtc_init);
