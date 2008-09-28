#include <stdexcept>
#include <cassert>
#include <string>
#include <cstdlib>
#include "lua_config.hxx"

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
