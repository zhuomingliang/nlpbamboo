#ifndef LUA_CONFIG_HXX
#define LUA_CONFIG_HXX

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include <vector>
#include "iconfig.hxx"
#include <stdexcept>

class LuaConfig: public IConfig {
protected:
	lua_State *_lua;
public:
	LuaConfig(const char *filename);
	LuaConfig();
	~LuaConfig();
	void get_value(const char *key, int &val);
	void get_value(const char *key, const char *&val);
	void get_value(const char *key, double &val);
	void get_value(const char *key, std::vector<std::string> &val) 
	{
		throw std::runtime_error("LuaConfig: method not implement");
	}
};

#endif
