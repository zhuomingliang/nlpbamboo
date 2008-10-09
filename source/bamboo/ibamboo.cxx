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

#include <iostream>
#include <stdexcept>

#include "bamboo.hxx"
#include "ibamboo.hxx"

void *bamboo_init(const char *cfg)
{
	try {
		return new Bamboo(cfg);
	} catch(std::exception &e) {
		std::cerr << __FUNCTION__ << ": " << e.what();
		return NULL;
	}
}

void bamboo_clean(void *handle)
{
	try {
		if (handle == NULL)
			throw new std::runtime_error("handle is null");
		delete static_cast<Bamboo *>(handle);
	} catch(std::exception &e) {
		std::cerr << __FUNCTION__ << ": " << e.what();
	}
}

ssize_t bamboo_parse(void *handle, char *t, const char *s)
{
	try {
		if (handle == NULL) throw new std::runtime_error("handle is null");
		if (s == NULL) return 0;
		if (t == NULL) return 0;

		if (*s == '\0') {
			*t = '\0';
			return 0;
		}

		Bamboo *bamboo = static_cast<Bamboo *>(handle);
		return bamboo->parse(t, s);
	} catch(std::exception &e) {
		std::cerr << __FUNCTION__ << ": " << e.what();
		return -1;
	}
}
void bamboo_set(void *handle, const char *s)
{
	try {
		if (handle == NULL) throw new std::runtime_error("handle is null");
		if (s == NULL) return;

		Bamboo *bamboo = static_cast<Bamboo *>(handle);
		bamboo->set(s);
	} catch(std::exception &e) {
		std::cerr << __FUNCTION__ << ": " << e.what();
	}
}

void bamboo_reload(void *handle)
{
	try {
		if (handle == NULL) throw new std::runtime_error("handle is null");

		Bamboo *bamboo = static_cast<Bamboo *>(handle);
		bamboo->reload();
	} catch(std::exception &e) {
		std::cerr << __FUNCTION__ << ": " << e.what();
	}
}
