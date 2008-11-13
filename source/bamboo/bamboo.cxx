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
#include <cstdlib>

#include "token_impl.hxx"
#include "parser_impl.hxx"
#include "bamboo.hxx"

static std::vector<bamboo::Token> g_tokens;
static std::string g_str;
static struct token_queue *g_tq = NULL;

static void _strftoks(std::string &str, std::vector<bamboo::Token> &vec, const char *delim)
{
	size_t i, length;

	length = vec.size();
	for (i = 0; i < length; i++) {
		str += vec[i].token;
		if (i < length - 1) str += delim;
	}
}


void *bamboo_init(const char *cfg)
{
	try {
		g_tq = (struct token_queue *)realloc(g_tq, sizeof(struct token_queue));
		if (!g_tq) throw new std::bad_alloc();
		memset(g_tq, 0, sizeof(struct token_queue));
		return new bamboo::Parser(cfg);
	} catch(std::exception &e) {
		std::cerr << __FUNCTION__ << ": " << e.what();
		return NULL;
	}
}

void bamboo_clean(void *handle)
{
	size_t i;

	try {
		if (handle == NULL)
			throw new std::runtime_error("handle is null");
		for (i = 0; i < g_tq->length; i++) 
			free(g_tq->tokens[i].token);
		free(g_tq);
		delete static_cast<bamboo::Parser *>(handle);
	} catch(std::exception &e) {
		std::cerr << __FUNCTION__ << ": " << e.what();
	}
}

ssize_t bamboo_parse(void *handle, const char **t, const char *s)
{
	try {
		if (handle == NULL) throw new std::runtime_error("handle is null");
		if (s == NULL) return 0;
		if (t == NULL) return 0;

		if (*s == '\0') {
			*t = '\0';
			return 0;
		}
		
		g_tokens.clear();
		g_str.clear();
		bamboo::Parser *bamboo = static_cast<bamboo::Parser *>(handle);
		bamboo->parse(g_tokens, s);
		_strftoks(g_str, g_tokens, " ");
		*t = g_str.c_str();
		return g_str.length();
	} catch(std::exception &e) {
		std::cerr << __FUNCTION__ << ": " << e.what();
		return -1;
	}
}

ssize_t bamboo_parse_more(void *handle, struct token_queue **queue, const char *s)
{
	size_t i, length, new_size;

	try {
		if (handle == NULL) throw new std::runtime_error("handle is null");
		if (s == NULL) return 0;
		if (queue == NULL) return 0;

		if (*s == '\0') {
			*queue = NULL;
			return 0;
		}

		g_tokens.clear();
		bamboo::Parser *bamboo = static_cast<bamboo::Parser *>(handle);
		bamboo->parse(g_tokens, s);
		length = g_tokens.size();
		for (i = 0; g_tq->tokens[i].token; i++) {
			free(g_tq->tokens[i].token);
			g_tq->tokens[i].token = NULL;
		}
		if (g_tq == NULL || g_tq->length < length) {
			new_size = sizeof(struct token_queue)  + sizeof(g_tq->tokens) * length;
			g_tq = (struct token_queue *)realloc(g_tq, new_size);
			if (!g_tq) throw new std::bad_alloc();
			memset(g_tq, 0, new_size);
			g_tq->length = length;
		}
		for (i = 0; i < length; i++) {
			g_tq->tokens[i].token = strdup(g_tokens[i].token);
			strncpy(g_tq->tokens[i].attr, bamboo::strfpos(g_tokens[i].pos), sizeof(g_tq->tokens[i].attr));
		}
		*queue = g_tq;
		return length;
	} catch(std::exception &e) {
		std::cerr << __FUNCTION__ << ": " << e.what();
		return -1;
	}
}

/* This function is for test purpose */
ssize_t bamboo_parse_with_pos(void *handle, const char **t, const char *s)
{
	size_t i, length;

	try {
		if (handle == NULL) throw new std::runtime_error("handle is null");
		if (s == NULL) return 0;
		if (t == NULL) return 0;

		if (*s == '\0') {
			*t = '\0';
			return 0;
		}

		g_tokens.clear();
		g_str.clear();
		bamboo::Parser *bamboo = static_cast<bamboo::Parser *>(handle);
		bamboo->parse(g_tokens, s);
		length = g_tokens.size();
		for (i = 0; i < length; i++) {
			g_str += g_tokens[i].token;
			if (g_tokens[i].pos) {
				g_str += "/"; 
				g_str += bamboo::strfpos(g_tokens[i].pos);
			}
			if (i < length - 1) g_str += " ";
		}
		*t = g_str.c_str();
		return g_str.length();
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

		bamboo::Parser *bamboo = static_cast<bamboo::Parser *>(handle);
		bamboo->set(s);
	} catch(std::exception &e) {
		std::cerr << __FUNCTION__ << ": " << e.what();
	}
}

void bamboo_reload(void *handle)
{
	try {
		if (handle == NULL) throw new std::runtime_error("handle is null");

		bamboo::Parser *bamboo = static_cast<bamboo::Parser *>(handle);
		bamboo->reload();
	} catch(std::exception &e) {
		std::cerr << __FUNCTION__ << ": " << e.what();
	}
}


namespace bamboo {

	char *strfpos(unsigned short p) {
		static char pos_str[3] = {0};
		if(p > 256) {
			pos_str[0] = p >> 8;
			pos_str[1] = p % 256;
		} else {
			pos_str[0] = p % 256;
			pos_str[1] = '\0';
		}
		return pos_str;
	}

	Parser::Parser (const char *s): ParserImpl(s) 
	{
	}

	void Parser::set(std::string s) {(*_config) << s;}
	void Parser::set(std::string key, std::string val) {(*_config)[key] = val;}
	void Parser::reload() {_fini();_init();}

	size_t Parser::parse(std::vector<Token> &vec, const char *s)
	{
		size_t i, length;

		_parse(s);
		length = _in->size();
		vec.clear();
		vec.reserve(length);
		for (i = 0; i < length; i++) {
			if (*((*_in)[i]->get_orig_token()) != ' ') 
				vec.push_back(*(*_in)[i]);
			delete (*_in)[i];
		}

		return length;
	}

	Token::Token()
		:token(NULL)
	{
	}

	Token::Token(const Token &rhs)
	{
		token = strdup(rhs.token);
		pos = rhs.pos;
	}

	Token::Token(const TokenImpl &rhs)
	{
		token = strdup(rhs.get_orig_token());
		pos = rhs.get_pos();
	}

	Token& Token::operator=(const TokenImpl &rhs)
	{
		token = strdup(rhs.get_orig_token());
		pos = rhs.get_pos();

		return *this;
	}

	Token::~Token()
	{
		if (token) free(token);
		pos = 0;
	}
}
