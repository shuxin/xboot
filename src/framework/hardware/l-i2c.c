/*
 * framework/hardware/l-i2c.c
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

#include <bus/i2c.h>
#include <framework/hardware/l-hardware.h>

struct li2c_t {
	struct i2c_client_t * client;
};

static int l_i2c_new(lua_State * L)
{
	const char * name = luaL_checkstring(L, 1);
	int addr = luaL_checkinteger(L, 2);
	int flags = luaL_optinteger(L, 3, 0);
	struct i2c_client_t * client = i2c_client_alloc(name, addr, flags);
	if(!client)
		return 0;
	struct li2c_t * i2c = lua_newuserdata(L, sizeof(struct li2c_t));
	i2c->client = client;
	luaL_setmetatable(L, MT_HARDWARE_I2C);
	return 1;
}

static const luaL_Reg l_i2c[] = {
	{"new",	l_i2c_new},
	{NULL,	NULL}
};

static int m_i2c_gc(lua_State * L)
{
	struct li2c_t * i2c = luaL_checkudata(L, 1, MT_HARDWARE_I2C);
	i2c_client_free(i2c->client);
	return 0;
}

static int m_i2c_read(lua_State * L)
{
	struct li2c_t * i2c = luaL_checkudata(L, 1, MT_HARDWARE_I2C);
	int count = luaL_checkinteger(L, 2);
	if(count <= 0)
	{
		lua_pushnil(L);
	}
	else if(count <= SZ_4K)
	{
		char buf[SZ_4K];
		if(i2c_master_recv(i2c->client, buf, count) == count)
			lua_pushlstring(L, buf, count);
		else
			lua_pushnil(L);
	}
	else
	{
		char * p = malloc(count);
		if(p && i2c_master_recv(i2c->client, p, count) == count)
			lua_pushlstring(L, p, count);
		else
			lua_pushnil(L);
		free(p);
	}
	return 1;
}

static int m_i2c_write(lua_State * L)
{
	struct li2c_t * i2c = luaL_checkudata(L, 1, MT_HARDWARE_I2C);
	int count;
	const char * buf = luaL_checklstring(L, 2, (size_t *)&count);
	if(count > 0)
		lua_pushboolean(L, (i2c_master_send(i2c->client, (void *)buf, count) == count));
	else
		lua_pushboolean(L, 0);
	return 1;
}

static const luaL_Reg m_i2c[] = {
	{"__gc",	m_i2c_gc},
	{"read",	m_i2c_read},
	{"write",	m_i2c_write},
	{NULL,	NULL}
};

int luaopen_hardware_i2c(lua_State * L)
{
	luaL_newlib(L, l_i2c);
	luahelper_create_metatable(L, MT_HARDWARE_I2C, m_i2c);
	return 1;
}
