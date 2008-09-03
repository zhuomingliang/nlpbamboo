#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

class utf8 {
protected:
	static const char _map[];
public:
	static int locate(const char *s, size_t start)
	{
		size_t i, j;

		if (s == NULL) return 0; 
		for (i = 0, j = 0; j < start && s[i];) {
			if (_map[(unsigned char)s[i]] > -1) {
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
			if (_map[(unsigned char)s[i]] > -1) {
				i += _map[(unsigned char)s[i]];
			} else { 
				return -1;
			}
		}
		return 0;
	}

	static int index(const char *s, size_t *index)
	{
		size_t i, j;

		if (s == NULL) return 0; 
		for (i = 0, j = 0; s[i];) {
			index[j++] = i;
			if (_map[(unsigned char)s[i]] > -1) {
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
			if (_map[(unsigned char)s[i]] > -1) {
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
			if (_map[(unsigned char)s[i]] > -1) {
				i += _map[(unsigned char)s[i]];
				j++;
			} else  {
				++i;
				continue;
			}
		}
		for (j = 0; s[i] && length;) {
			if (_map[(unsigned char)s[i]] > -1) {
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
