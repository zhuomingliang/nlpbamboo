#include <cstring>
#include <cassert>
#include <cstdlib>
#include <iostream>

class utf8 {
protected:
	static const char _map[];
public:
	static size_t first(const char *s, char *utf8ch)
	{
		size_t i = 1;

		if (s == NULL) return 0; 
		i = (_map[(unsigned char)*s] > 0)?_map[(unsigned char)*s]:1;
		strncpy(utf8ch, s, i);
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
		size_t i, j, n;

		if (s == NULL || t == NULL || length < 1) return 0;
		for (i = 0, j = 0; s[i] && j < start;) {
			if (_map[(unsigned char)s[i]] > 0) {
				i += _map[(unsigned char)s[i]];
				j++;
			} else  {
				++i;
				continue;
			}
		}
		for (j = 0; s[i] && length;) {
			if (_map[(unsigned char)s[i]] > 0) {
				length--;
				n = _map[(unsigned char)s[i]];
				while(n--) t[j++] = s[i++];
			} else  {
				++i;
				continue;
			}
		}
		t[j] = '\0';

		return j;
	}
};
