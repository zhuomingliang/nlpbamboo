#ifndef DATRIE_HXX
#define DATRIE_HXX

#include "double_array.hxx"

class DATrie: public DoubleArray {
	friend class TrieDebugger;
private:
	DATrie(DATrie &trie) {}

protected:
	typedef struct {
		int num;
		int last;
	} _extra_t;

	typedef struct {
		int *s;
		int length;
	} _suffix_t;

	_extra_t *_extra;
	int *_tail;
	_suffix_t _suffix;

	void _inflate_tail(int num)
	{
		int neo;

		assert(num > 0);
		neo = (((_extra->num + num) >> 12) + 1) << 12; // align for 32 bits
		_tail = (int *)realloc(_tail, neo * sizeof(int));
		memset(_tail + _extra->num, 0, (neo - _extra->num) * sizeof(int));
		if (_tail == NULL) throw std::bad_alloc();
		_extra->num = neo;
	}

	virtual void _explore_finish(on_explore_finish_t cb, void *arg, int s, size_t off) 
	{
		int p, val;

		if (_explore_buff[off - 1] == '\0') {
			if (_base(s) < 0) {
				val = *(_tail - _base(s));
			} else {
				val = _base(s);
			}
		} else {	
			for (p = -_base(s); _tail[p]; p++) 
				_explore_buff[off++] = _tail[p];
			_explore_buff[off] = '\0';
			val = *(_tail + p + 1);
		}
		cb(_explore_buff, val, arg);
	}

	void _insert_tail(int s, const char *key, int val);
	void _branch(int s, const char *key, int val);

public:
	DATrie(int size=_default_num_state)
		: DoubleArray(size), _extra(NULL), _tail(NULL)
	{
		_extra = new _extra_t;
		_extra->num = 0;
		_extra->last = 1;
		_suffix.s = NULL;
		_suffix.length = 0;
		_inflate_tail(size);
		strcpy(_header->magic, "datrie");
	}

	DATrie(const char *filename)
		: DoubleArray(filename)
	{
		size_t start = sizeof(_header_t) + _header->num * sizeof(_state_t);
		_extra = (_extra_t *)_mmap->start(start);
		_tail = (int *)_mmap->start(start + sizeof(_extra_t));
		_suffix.s = NULL;
		_suffix.length = 0;
	}

	~DATrie()
	{
		if (!_mmap) {
			delete _extra;
			free(_tail);
			free(_suffix.s);
			_suffix.s = 0;
		}
	}
	void insert(const char *key, int val);
	int search(const char *key);
	void save(const char *filename);
};

#endif // DATRIE_HPP
