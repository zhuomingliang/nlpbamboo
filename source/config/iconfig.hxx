#ifndef ICONFIG_HXX
#define ICONFIG_HXX

#include <stdexcept>
#include <cstring>


class IConfig {
public:
	virtual bool get_value(const char *key, int &val) = 0;
	virtual bool get_value(const char *key, double &val) = 0;
	virtual bool get_value(const char *key, const char *&val) = 0;
	IConfig(const char *filename, bool can_throw=true);
	IConfig() {}
	virtual ~IConfig() {}
};
#endif // ICONFIG_HPP
