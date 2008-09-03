#ifndef DOUBLE_ARRAY_HXX
#define DOUBLE_ARRAY_HXX

#include <cstdlib>
#include <cassert>
#include <cstring>

#include <exception>
#include <stdexcept>
#include <iostream>
#include <cstdio>

#include "mmap.hxx"

typedef void (*on_explore_finish_t)(const char*, int, void *);
class DoubleArray {
	friend class TrieDebugger;

public:	
	static const int alphabet_size = 257;
	static const int endmark = 1;
	static const int magic_size = 32;

	DoubleArray(int num=_default_num_state);
	DoubleArray(const char *filename);
	virtual ~DoubleArray()
	{
		if (_mmap) {
			delete _mmap;
		} else {
			delete _header;
			free(_state);
		}
	}

	void explore(on_explore_finish_t cb, void *arg)
	{
		_explore(cb, arg, 1, 0);
	}

	int max_value()
	{
		return _header->max;
	}

	int min_value()
	{
		return _header->min;
	}

	int sum_value()
	{
		return _header->sum;
	}

	int num_insert()
	{
		return _header->num_insert;
	}

	void insert(const char *key, int val);
	int search(const char *key);
	void save(const char *filename);

protected:
	static const int _default_num_state = 1024;
	static const int _explore_buff_size = 1024;

	typedef struct {
		char magic[magic_size];
		int num;
		int max, min;
		long sum;
		int num_insert;
	} _header_t;

	typedef struct {
		int base;
		int check;
	} _state_t;

	_header_t *_header;
	_state_t *_state;
	MMap *_mmap;
	char _explore_buff[_explore_buff_size];

	int _key2state(int ch)
	{
		return (unsigned int)ch + 1;
	}

	int _state2key(int s)
	{
		assert(s > 0);
		return s - 1;
	}

	int _base(int s)
	{
		assert(s > 0 && s < _header->num);
		return _state[s].base;
	}

	int _check(int s)
	{
		assert(s > 0 && s < _header->num);
		return _state[s].check;
	}

	void _set_base(int s, int val)
	{
		assert(s > 0 && s < _header->num);
		_state[s].base = val;
	}

	void _set_check(int s, int val)
	{
		assert(s > 0 && s < _header->num);
		_state[s].check = val;
	}

	int _next(int s, int ch)
	{
		int in = _key2state(ch);

		assert(s > 0 && s < _header->num); 
		if (s + in >= _header->num) _inflate(in);
		return _base(s) + in;
	}

	int _forward(int s, int ch)
	{
		assert(s > 0 && s < _header->num); 
		int t = _next(s, ch);
		return (t > 0 && t < _header->num && _check(t) == s)?t:0;
	}

	void _inflate(int num)
	{
		int neo;

		assert(num > 0);
		neo = (((_header->num + num) >> 12) + 1) << 12; // align for 4096
		_state = (_state_t *)realloc(_state, neo * sizeof(_state_t));
		memset(_state + _header->num, 0, (neo - _header->num) * sizeof(_state_t));
		if (_state == NULL) throw std::bad_alloc();
		_header->num = neo;
	}

	size_t _find_accepts(int s, int *inputs, int *max, int *min)
	{
		int ch;
		int *p;

		assert(s > -1 && s < _header->num);
		assert(inputs);
		for (ch = 0, p = inputs; ch < alphabet_size; ch++) {
			if (_forward(s, ch)) {
				*(p++) = ch;
				if (max && (ch > *max || *max == 0)) *max = ch;
				if (min && (ch < *min || *min == 0)) *min = ch;
			}
		}
		*p = -1;
		return p - inputs;
	}

	void _update_header(int val)
	{
		_header->max = (val > _header->max)?val:_header->max;
		_header->min = (val < _header->min)?val:_header->min;
		_header->sum += val;
		_header->num_insert++;
	}

	virtual void _explore_finish(on_explore_finish_t cb, void *arg, int s, size_t off) 
	{
		cb(_explore_buff, _base(s), arg);
	}

	int _find_base(int *key, int max, int min);
	int _relocate(int stand, int s, int *key, int max, int min);
	int _create_transition(int s, int ch);
	void _explore(on_explore_finish_t cb, void *arg, int s, size_t off);
private:
	DoubleArray(DoubleArray &) {}

};

#endif // DOUBLE_ARRAY_HPP
