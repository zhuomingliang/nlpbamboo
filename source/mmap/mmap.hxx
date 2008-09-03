#ifndef MMAP_HXX
#define MMAP_HXX

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <ctype.h>
#include <stdexcept>
#include <cassert>

class MMap {
protected:
	void *_start;
	size_t _size;

public:
	MMap():_start(NULL), _size(0) {}
	MMap(const char *filename): _start(NULL), _size(0)
	{
		struct stat sb;
		int fd, retval;

		assert(filename);
		fd = open(filename, O_RDONLY);
		if (fd < 0) throw std::runtime_error("Can not open file");
		if (fstat(fd, &sb) < 0) throw std::runtime_error("Can not stat file");
		_start = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
		if (_start == MAP_FAILED) throw std::runtime_error("Can not mmap");
		while (retval = close(fd), retval == -1 && errno == EINTR) ;
		_size = sb.st_size;
	}
	~MMap()
	{
		if (_start) {
			if (munmap(_start, _size) < 0) 
				throw std::runtime_error("Can not munmap");
		}
	}
	void *start(size_t off=0)
	{
		return (void *)((char *)_start + off);
	}
	bool is_mapped()
	{
		return !(_start == NULL);
	}
};

#endif
