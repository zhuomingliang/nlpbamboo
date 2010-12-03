/*
 * Copyright (c) 2008, detrox@gmail.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the <organization> nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY detrox@gmail.com ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL detrox@gmail.com BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */

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
#include <cstring>

namespace bamboo {


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
		if (fd < 0) throw std::runtime_error(strerror(errno));
		if (fstat(fd, &sb) < 0) throw std::runtime_error(strerror(errno));
		_start = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
		if (_start == MAP_FAILED) throw std::runtime_error(strerror(errno));
		while (retval = close(fd), retval == -1 && errno == EINTR) ;
		_size = sb.st_size;
	}
	~MMap()
	{
		if (_start) {
			if (munmap(_start, _size) < 0) 
				throw std::runtime_error(strerror(errno));
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

} //namespace bamboo

#endif


