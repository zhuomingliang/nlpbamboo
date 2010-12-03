#ifndef KEA_MMAP_HXX
#define KEA_MMAP_HXX

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/mman.h>
#include <unistd.h>

namespace bamboo { namespace kea {

class MMap {
protected:
	int _fd;
	size_t _size;
	const char *_filename;
	char *_ptr;
	int _oflag;

	int _fileresize(size_t size) {
		if(size==0 || _fd==-1 || _oflag==O_RDONLY) return -1;
		if(ftruncate(_fd, size)==-1) return -1;
		_size = size;
		return 0;
	}
	void _closefd() {
		if(_fd != -1)
			::close(_fd);
		_fd = -1;
		_filename = 0;
		_oflag = 0;
	}
	void _destroy() {
		if(_ptr != 0) {
			msync((caddr_t)_ptr, 0, MS_ASYNC);
			munmap((caddr_t)_ptr, _size);
			_ptr = 0;
			_size = 0;
		}
	}

public:
	MMap():_fd(-1),_size(0),_filename(0),_ptr(0),_oflag(0) {}
	~MMap() {this->close();}

	size_t getlen() {
		return _size;
	}
	char *ptr() {
		return _ptr;
	}
	int open(const char *file, bool write_able = false, size_t size = 0, const mode_t perm_mode = 0644) {
		this->close();
		this->_filename = file;

		if(write_able)
			_oflag = O_RDWR|O_CREAT;
		else
			_oflag = O_RDONLY;

		struct stat st;

		if((_fd=::open(file, _oflag, perm_mode))<0)
			return -1;

		if(fstat(_fd, &st) != 0)
			return -1;
		
		_size = st.st_size;
		if(size>0 && _size!=size && _fileresize(size)!=0) return -1;
		if(_size==0) return -1;

		int prot = PROT_READ;
		if(_oflag&O_RDWR) prot |= PROT_WRITE;
		_ptr = (char*) mmap(NULL, _size, prot, MAP_FILE|MAP_SHARED, _fd, 0);
		if(_ptr==MAP_FAILED) return -1;

		return 0;
	}
	int resize(size_t size) {
		if(size==0) return -1;
		this->_destroy();
		if(_fileresize(size)!=0) return -1;
		int prot = PROT_READ;
		if(_oflag==O_RDWR) prot |= PROT_WRITE;
		_ptr = (char*) mmap(NULL, _size, prot, MAP_SHARED, _fd, 0);
		if(_ptr==MAP_FAILED) return -1;
		return 0;
	}
	int close() {
		this->_destroy();
		this->_closefd();
		return 0;
	}
	int flush() {
		if(_ptr==0) return -1;
		if(msync(_ptr, _size, MS_ASYNC)==-1)
			return -1;
		return 0;
	}
};

}} //end of namespace bamboo:kea

#endif //end of KEA_MMAP_HXX
