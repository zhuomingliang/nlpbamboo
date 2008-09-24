#include <stdexcept>
#include <cassert>
#include <string>
#include "lua_config.hxx"

LuaConfig::LuaConfig()
	:_lua(NULL), _throw(true)
{
	_lua = lua_open();
	luaL_openlibs(_lua);
}

LuaConfig::LuaConfig(const char *filename, bool can_throw)
	:_lua(NULL), _throw(can_throw)
{
	_lua = lua_open();
	luaL_openlibs(_lua);
	luaopen_table(_lua);

	if (luaL_loadfile(_lua, filename) || lua_pcall(_lua, 0, 0, 0)) {
		throw std::runtime_error("Can not load configuration " + std::string(filename));
	}
}

LuaConfig::~LuaConfig()
{
	lua_close(_lua);
}

bool LuaConfig::get_value(const char *key, double &val)
{
	lua_getglobal(_lua, key);
	if (!lua_isnumber(_lua, -1)) {
		if (_throw) throw std::runtime_error(
				std::string("Invalid type of value of key ").append(key).append(", should be double")); 
		return false;
	}
	assert(sizeof(double) == sizeof(lua_Number));
	val = lua_tonumber(_lua, -1);
	return true;
}

bool LuaConfig::get_value(const char *key, int &val)
{
	lua_getglobal(_lua, key);
	if (!lua_isnumber(_lua, -1)) {
		if (_throw) throw std::runtime_error(
				std::string("Invalid type of value of key ").append(key).append(", should be integer")); 
		return false;
	}
	val = lua_tointeger(_lua, -1);
	return true;
}

bool LuaConfig::get_value(const char *key, const char *&val)
{
	lua_getglobal(_lua, key);
	if (!lua_isstring(_lua, -1)) {
		if (_throw) throw std::runtime_error(
				std::string("Invalid type of value of key ").append(key).append(", should be string")); 
		return false;
	}
	val = lua_tolstring(_lua, -1, NULL);
	return true;
}
