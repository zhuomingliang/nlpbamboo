#ifndef CONFIG_FACTORY_HXX
#define CONFIG_FACTORY_HXX

#include "iconfig.hxx"
#include "lua_config.hxx"

class ConfigFactory {
private:
	ConfigFactory();
public:
	static IConfig *create(const char *t, const char *filename)
	{
		if (strcmp(t, "lua_config") == 0) {
			return new LuaConfig(filename, true);
		} else {
			throw std::runtime_error("Unknow config type");
		}
	}
};

#endif //CONFIG_FACTORY_HXX
