/*
 * Copyright (c) 2008, detrox@gmail.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the <organization> nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY detrox@gmail.com ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL detrox@gmail.com BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */

#include <stdexcept>
#include <cassert>
#include <string>
#include <cstdlib>
#include "lua_config.hxx"

namespace bamboo {


LuaConfig::LuaConfig()
	:_lua(NULL)
{
	_lua = lua_open();
	luaL_openlibs(_lua);
}

LuaConfig::LuaConfig(const char *filename)
	:_lua(NULL)
{
	_lua = lua_open();
	luaL_openlibs(_lua);

	if (luaL_loadfile(_lua, filename)) {
		throw std::runtime_error(std::string("Can not load configuration ").append(filename));
	}

	/* Update root */
	lua_pushstring(_lua, getenv("PWD"));
	lua_setglobal(_lua, "root");
	
	if (lua_pcall(_lua, 0, 0, 0)) {
		throw std::runtime_error(std::string("Can not parse configuration ").append(filename));
	}
}

LuaConfig::~LuaConfig()
{
	lua_close(_lua);
}

void LuaConfig::get_value(const char *key, double &val)
{
	lua_getglobal(_lua, key);
	if (!lua_isnumber(_lua, -1)) {
		throw std::runtime_error(
				std::string("Invalid type of value of key ").append(key).append(", should be double")); 
	}
	assert(sizeof(double) == sizeof(lua_Number));
	val = lua_tonumber(_lua, -1);
}

void LuaConfig::get_value(const char *key, int &val)
{
	lua_getglobal(_lua, key);
	if (!lua_isnumber(_lua, -1)) {
		throw std::runtime_error(
				std::string("Invalid type of value of key ").append(key).append(", should be integer")); 
	}
	val = lua_tointeger(_lua, -1);
}

void LuaConfig::get_value(const char *key, const char *&val)
{
	lua_getglobal(_lua, key);
	if (!lua_isstring(_lua, -1)) {
		throw std::runtime_error(
				std::string("Invalid type of value of key ").append(key).append(", should be string")); 
	}
	val = lua_tolstring(_lua, -1, NULL);
}


} //namespace bamboo