#ifndef UTF8_XXX
#define UTF8_XXX
#include <cstring>
#include <cassert>
#include <cstdlib>
#include <iostream>

class utf8 {
protected:
	static const char _map[], _map_ignore[];
public:
	static size_t first(const char *s, char *utf8ch)
	{
		register size_t i;
		i = _map_ignore[(unsigned char)*s];
		memcpy(utf8ch, s, i);
		utf8ch[i] = '\0';
		return i;
	}

	static int locate(const char *s, size_t start)
	{
		size_t i, j;

		if (s == NULL) return 0; 
		for (i = 0, j = 0; j < start && s[i];) {
			if (_map[(unsigned char)s[i]] > 0) {
				i += _map[(unsigned char)s[i]];
				j++;
			} else { 
				++i;
			}
		}
		return i;
	}

	static int check(const char *s)
	{
		size_t i;

		if (s == NULL) return 0; 
		for (i = 0; s[i];) {
			if (_map[(unsigned char)s[i]] > 0) {
				i += _map[(unsigned char)s[i]];
			} else { 
				return -1;
			}
		}
		return 0;
	}

	static int strstr(const char *haystack, const char *needle)
	{
		size_t i, step, length;
		
		length = strlen(needle);
		if (haystack == NULL || needle == NULL || length == 0) return -1; 
		for (i = 0; haystack[i];) {
			step = (_map[(unsigned char)haystack[i]] > 0)?_map[(unsigned char)haystack[i]]:1;
			if (length == step && strncmp(haystack + i, needle, length) == 0) return i;
			i += step;
		}
		return -1;
	}

	static int index(const char *s, size_t *index)
	{
		size_t i, j;

		if (s == NULL) return 0; 
		for (i = 0, j = 0; s[i];) {
			index[j++] = i;
			if (_map[(unsigned char)s[i]] > 0) {
				i += _map[(unsigned char)s[i]];
			} else { 
				++i;
				continue;
			}
		}
		return j;
	}

	static size_t length(const char *s)
	{
		size_t i, j;

		if (s == NULL) return 0; 
		for (i = 0, j = 0; s[i];) {
			if (_map[(unsigned char)s[i]] > 0) {
				i += _map[(unsigned char)s[i]];
				++j;
			} else { 
				++i;
				continue;
			}
		}
		return j;
	}

	static size_t sub(char *t, const char *s, size_t start, size_t length)
	{
		register size_t i;
		register char *begin = t;

		for (i = 0; *s && i < start; s += _map_ignore[(unsigned char)*s], ++i) ;
		while(length--) {
			i = _map_ignore[(unsigned char)*s];
			while(i--) *(t++) = *(s++);
		}
		*t = '\0';
		return t - begin;
	}

	static char to_dbc(const char *str, size_t len) {
		if ((unsigned char)*str < 128) return *str;
		if (len != 3) return 0;
		
		unsigned char uch1, uch2, uch3, dbc;
		uch1 = str[0];
		uch2 = str[1];
		uch3 = str[2];

		if(uch1==0xE3 && uch2==0x80 && uch3==0x80)
			dbc = 0x20;
		else if(uch1==0xEF && uch2==0xBC && uch3>=0x81 && uch3 <=0xBF) {
			dbc = uch3 - 0x60;
		}
		else if(uch1==0xEF && uch2==0xBD && uch3>=0x80 && uch3 <=0x9E) {
			dbc = uch3 - 0x20;
		}
		else
			dbc = 0x0;

		return dbc;
	}
};
#endif
