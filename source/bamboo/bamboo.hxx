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

#ifndef LIB_BAMBOO_HXX
#define LIB_BAMBOO_HXX

#ifdef __cplusplus
#include <vector>
#include <string>

namespace bamboo {
class Token {
public:
	Token();
	~Token();
	Token(const TokenImpl &rhs);
	Token(const Token &rhs);
	Token& operator=(const TokenImpl &rhs);
	char *token;
	int pos;
};

class ParserImpl;
class Parser: public ParserImpl {

public:
	Parser (const char *s);

	void set(std::string s);
	void set(std::string key, std::string val);
	void reload();
	size_t parse(std::vector<Token> &vec, const char *s);
};


char *strfpos(unsigned short p);

}
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>

struct token_queue {
	size_t length;
	struct {
		char *token;
		char attr[4];
	} tokens[1];
};
void *bamboo_init(const char *s);
ssize_t bamboo_parse(void *handle, const char **t, const char *s);
ssize_t bamboo_parse_more(void *handle, struct token_queue **queue, const char *s);
ssize_t bamboo_parse_with_pos(void *handle, const char **t, const char *s);
void bamboo_set(void *handle, const char *s);
void bamboo_reload(void *handle);
void bamboo_clean(void *handle);
#ifdef __cplusplus
};
#endif

#endif //LIB_BAMBOO_HXX
