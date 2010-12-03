#ifndef KEA_HASH_HXX
#define KEA_HASH_HXX

#include <cstring>
#include <stdexcept>
#include "kea_mmap.hxx"

namespace bamboo { namespace kea {

enum status_t {
	EMPTY = 0,
	NORMAL = 1,
	TOMB = 2
};

template <class T>
class YCHashNode {
protected:
	long long _id;
	T _val;
	status_t _st;

public:
	YCHashNode() {
		_st = EMPTY;
	}
	void set_status(status_t s) {
		_st = s;
	}
	status_t get_status() {
		return _st;
	}
	long long get_id() {
		return _id;
	}
	void set_id(long long i) {
		_id = i;
	}
	T get_value() {
		return _val;
	}
	void set_value(T t) {
		_val = t;
	}
};

template <class T>
class YCHash {
protected:
	YCHashNode<T> * _ht;
	MMap * _mmap;
	int _hash_size;

	struct YCHashInfo {
		int hash_size;
		int file_size;
	};

private: //hash functions
	unsigned int _elf_hash(const char* key, int hash_size) {
		size_t h =0 ;
		while (*key){
			h = (h << 4) + *key++;
			size_t g = h & 0xF0000000L;
			if (g) h ^= g >> 24;
			h &= ~g;
		}
		return h % hash_size;
	}

	unsigned int _str_hash(const char *s, int hash_size) {
		unsigned int h;
		for(h=0; *s!='\0'; s++)
			h = (int) ((unsigned char) *s) + 31 * h;
		return h % hash_size;
	}

	unsigned int _hash_func1(const char * key, int hash_size) {
		if(!key) return 0;
		return _str_hash(key, hash_size);
	}

	unsigned int _hash_func2(const char * key, int hash_size) {
		if(!key) return 0;
		return _elf_hash(key, hash_size);
	}

	unsigned int _hash_func(const char * key1, const char * key2, int hash_size) {
		unsigned int h, home1, home2;
		home1 = _hash_func1(key1, hash_size);
		home2 = _hash_func2(key2, hash_size);
		h = ( home1 + home2 ) % hash_size;
		return h;
	}

	int _probe(int i) {
		return _square_probe(i);
	}

	int _linear_probe(int i) {
		return i;
	}

	int _square_probe(int i) {
		if (i%2==0)
			return -(i*i/4);
		else
			return (i+1)*(i+1)/4;
	}

public:
	//open a hash file
	YCHash(const char * hash_file) {
		_mmap = new MMap;
		if(_mmap->open(hash_file)!=0) {
			throw std::runtime_error(std::string("cannot mmap file ") + hash_file);
		}

		YCHashInfo * hash_info = reinterpret_cast<YCHashInfo*>(_mmap->ptr());
		_hash_size = hash_info->hash_size;
		int file_size = hash_info->file_size;
		int exp_file_size = sizeof(YCHashInfo) + _hash_size * sizeof(YCHashNode<T>);
		if(file_size != exp_file_size || file_size != int(_mmap->getlen())) {
			throw std::runtime_error(std::string("Hash dict ")
				+ hash_file + " is broken or not existed");
		}

		_ht = reinterpret_cast<YCHashNode<T>*>(hash_info+1);
	}

	//create a new hash file
	YCHash(const char * hash_file, int size) {
		_mmap = new MMap;
		_hash_size = size;
		int i, file_size = sizeof(YCHashInfo) + _hash_size * sizeof(YCHashNode<T>);

		if(_mmap->open(hash_file, true, file_size) != 0) {
			throw std::runtime_error(std::string("cannot mmap file ") + hash_file);
		}

		YCHashInfo * hash_info = reinterpret_cast<YCHashInfo*>(_mmap->ptr());
		hash_info -> hash_size = _hash_size;
		hash_info -> file_size = file_size;

		_ht = reinterpret_cast<YCHashNode<T>*>(hash_info+1);

		//init all hash nodes
		for(i=0; i<_hash_size; i++) {
			_ht[i].set_status(EMPTY);
		}
	}

	~YCHash() {
		if(_mmap) {
			delete _mmap;
			_mmap = NULL;
		}
	}

	int insert(const char * key, long long id, T value) {
		return insert(key, NULL, id, value);
	}

	int insert(const char * key1, const char * key2, long long id, T value) {
		unsigned int home = _hash_func(key1, key2, _hash_size);

		int i=0, insplace;
		bool tomb_pos=false;
		int pos = home;
		YCHashNode<T> *p;

		while(1) {
			p = _ht + pos;
			if(p->get_status() == EMPTY)
				break;
			if(p->get_status() == NORMAL && p->get_id()==id)
				return -1;
			if(p->get_status() == TOMB && !tomb_pos) {
				insplace = pos;
				tomb_pos = true;
				//cannot break, until find an empty space, otherwise may insert the same key
				//break;
			}
			
			if(i++>_hash_size) return -2; //hash may be full
			pos = (home + _probe(i)) % _hash_size;
			if(pos<0) pos += _hash_size;
		}
		if (!tomb_pos)
			insplace=pos;
		
		p = _ht + insplace;
		p->set_id(id);
		p->set_value(value);
		p->set_status(NORMAL);

		return 0;
	}

	int del(const char * key, long long id) {
		return del(key, NULL, id);
	}

	int del(const char * key1, const char * key2, long long id) {
		unsigned int home = _hash_func(key1, key2, _hash_size);
		int i=0;
		int pos = home;

		YCHashNode<T> *p;

		while(1) {
			p = _ht + pos;
			if(p->get_status() == EMPTY)
				break;
			if(p->get_status() == NORMAL && p->get_id() == id) {
				p->set_status(TOMB);
				break;
			}
			
			pos = (home + _probe(i)) % _hash_size;
			if(pos<0) pos += _hash_size;
		}

		return 0;
	}

	YCHashNode<T> * search(const char *key1, long long id) {
		return search(key1, NULL, id);
	}

	YCHashNode<T> * search(const char *key1, const char *key2, long long id) {
		int home = _hash_func(key1, key2, _hash_size);
		int i=0;
		int pos = home;

		YCHashNode<T> *p;

		while(1) {
			p = _ht + pos;
			if(p->get_status() == EMPTY)
				break;
			//key equal
			if(p->get_status() == NORMAL && p->get_id() == id) {
				return p;
			}

			if(i++>_hash_size) break; //hash may be full
			pos = (home + _probe(i)) % _hash_size;
			if(pos<0) pos += _hash_size;
		}
		return NULL;
	}
};

}} //end of namespace bamboo::kea

#endif //end of KEA_HASH_HXX
