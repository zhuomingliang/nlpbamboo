#ifndef SIMPLE_CONFIG_HXX
#define SIMPLE_CONFIG_HXX

#include <map>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <stdexcept>
#include <vector>
#include <ctype.h>

#include "iconfig.hxx"

class SimpleConfig:public IConfig {
protected:
	size_t _deep_parse_level;
	std::map<std::string, std::string> _map;
	std::string _key, _val, _real, _reserve;
	std::vector<std::string> _storage;
	std::string& _trim(std::string &s)
	{
		static const char whitespace[] = " \t\r\n";
		std::string::size_type begin = s.find_first_not_of(whitespace);
		if (begin > 0 && begin < s.npos) s.erase(0, begin);
		std::string::size_type end = s.find_last_not_of(whitespace);
		if (end + 1 < s.npos) s.erase(end + 1);
		return s;
	}

	void _insert(std::string &s)
	{
		int split = s.find("=");
		_key = s.substr(0, split);
		_val = s.substr(split + 1);
		_trim(_key);
		_trim(_val);
		if (!_key.empty())
			_map[_key] = _val;
	}

	void _parse_value(std::string &str, std::string &real)
	{
		const char *pch;
		bool opencb = false;
		enum {
			PS_UNKNOW = 0,
			PS_ESCAPE,
			PS_DOLLAR,
			PS_FETCHKEY,
			PS_REPLACE,
		} state;

		real.clear();
		_key.clear();
		for (pch = str.c_str(), state = PS_UNKNOW; *pch; pch++) {
			if (*pch == '\\') state = PS_ESCAPE;
			else if (*pch == '$' && state != PS_ESCAPE) state = PS_DOLLAR;
			else if (state == PS_DOLLAR) state = PS_FETCHKEY;
			else if (state == PS_FETCHKEY && !isalnum(*pch) && *pch != '{') state = PS_REPLACE;
			else if (state != PS_FETCHKEY) state = PS_UNKNOW;

			if (state == PS_UNKNOW) real.append(pch, 1);
			else if (state == PS_FETCHKEY) {
				if (isalnum(*pch)) _key.append(pch, 1);
				if (*pch == '{') opencb = true;
			} else if (state == PS_REPLACE) {
				real.append(_map[_key]); 
				_key.clear(); 
				if (!(opencb && *pch == '}')) pch--;
			}
		}
	}

	std::string& _deep_parse(std::string &str, size_t level = 0)
	{
		size_t i;
		level = (level)?level:_deep_parse_level;
		for(i = 0; i < level && str.find('$') < str.npos; i++) {
			_parse_value(str, _real);
			str = _real;
		}
		return str;
	}

public:
	SimpleConfig():_deep_parse_level(3) {};
	SimpleConfig(const char *s):_deep_parse_level(3) {read_from_file(s);};
	SimpleConfig& operator<< (const char *s)
	{
		_reserve = s;
		_insert(_reserve);
		return *this;
	}
	SimpleConfig& operator<< (std::string &s)
	{
		_insert(s);
		return *this;
	}
	std::string &operator[] (std::string s)
	{
		return _map[s];
	}
	void get_value(const char *key, int &val) {get_value(std::string(key), val);}
	void get_value(const char *key, long &val) {get_value(std::string(key), val);}
	void get_value(const char *key, double &val) {get_value(std::string(key), val);}
	void get_value(const char *key, std::string &val) {get_value(std::string(key), val);}
	void get_value(const char *key, std::vector<std::string> &val) {get_value(std::string(key), val);}
	void get_value(const char *key, const char *&val) 
	{
		_storage.push_back(std::string(_deep_parse(_map[std::string(key)])));
		val = _storage.back().c_str();
	}
	void get_value(std::string key, int &val)  {val = atoi(_deep_parse(_map[key]).c_str());}
	void get_value(std::string key, long &val) {val = atol(_deep_parse(_map[key]).c_str());}
	void get_value(std::string key, double &val) {val = atof(_deep_parse(_map[key]).c_str());}
	void get_value(std::string key, std::string &val) {val = _deep_parse(_map[key]);}

	void get_value(std::string key, std::vector<std::string> &val)
	{
		std::string::size_type i, p;
		val.clear();
		_reserve = _deep_parse(_map[key]);
		for (p = 0, i = _reserve.find(","); i < _reserve.npos; i = _reserve.find(",", p), p = i + 1) {
			val.push_back(_reserve.substr(p, i));
			_trim(val.back());
		}
	}
	void dump(std::string &s)
	{
		std::map<std::string, std::string>::iterator it;
		std::ostringstream oss;

		for (it = _map.begin(); it != _map.end(); it++) {
			oss << it->first << " = " << it->second << std::endl;
		}
		s.assign(oss.str());
	}
	void read_from_file(const char *s)
	{
		std::ifstream ifs(s);
		if (ifs.is_open()) {
			while(!(std::getline(ifs, _reserve).eof())) {
				_trim(_reserve);
				if (!_reserve.empty() && _reserve[0] != '#') _insert(_reserve);
			}
		} else {
			throw std::runtime_error(std::string("can not open file ") + s);
		}
	}
};

#endif
