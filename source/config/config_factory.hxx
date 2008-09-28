#ifndef CONFIG_FACTORY_HXX
#define CONFIG_FACTORY_HXX

#include "iconfig.hxx"
#include "simple_config.hxx"

class ConfigFactory {
private:
	ConfigFactory();
public:
	static IConfig *create(const char *filename)
	{
		return new SimpleConfig(filename);
	}
};

#endif //CONFIG_FACTORY_HXX
