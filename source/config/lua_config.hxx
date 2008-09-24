#ifndef LUA_CONFIG_HXX
#define LUA_CONFIG_HXX

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include <vector>
#include "iconfig.hxx"

class LuaConfig: public IConfig {
protected:
	lua_State *_lua;
	bool _throw;
	void _stack_pump(const char *key) 
	{
	}
public:
	LuaConfig(const char *filename, bool can_throw=true);
	LuaConfig();
	~LuaConfig();
	bool get_value(const char *key, int &val);
	bool get_value(const char *key, const char *&val);
	bool get_value(const char *key, double &val);
};

#endif
