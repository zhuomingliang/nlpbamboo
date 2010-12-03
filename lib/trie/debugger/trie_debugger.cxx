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

#include "trie_debugger.hxx"

namespace bamboo {


void TrieDebugger::trace(int s)
{
	int *p, key[_darray->alphabet_size + 1];
	int t, n;

	assert(s > 0 && s < _darray->_header->num);
	_states.push_back(s);
	n = _darray->_find_accepts(s, key, NULL, NULL);
	if (n) {
		for (p = key; *p > -1; p++) {
			t = _darray->_forward(s, *p);
			assert(t > 0 && t < _darray->_header->num);
			trace(t);
		}
	} else { 
		_trace_finish();
	}
	_states.pop_back();
}

void TrieDebugger::_trace_finish() 
{
	std::vector<int>::iterator it;
	int last, base=0;
	int ch = 0;

	std::clog << "transition => ";
	for (it = _states.begin(), last = 0; it != _states.end(); it++) {
		base = _darray->_base(*it);
		if (last) {
			ch = *it - last - 1; 
			if (isalnum(ch)) 
				std::clog << "-\033[1m\033[33m" << (unsigned char)ch << "\033[0m->";
			else
				std::clog << "-\033[1m\033[33m" << (int)ch << "\033[0m->";
		}
		std::clog << *it << "\033[37m[" << base << "]\033[0m";
		last = base;
	}

	if (base < 0 && _datrie) {
		std::clog << "->";
		if (ch == '\0') {
			std::clog << "!{" << _datrie->_tail[-base] << "}";
		} else {
			for (last = -base; _datrie->_tail[last]; last++) {
				std::clog << (char)_datrie->_tail[last];
			}
			std::clog << "{" << _datrie->_tail[last + 1] << "}";
		}
	} else {
		std::clog << "->x{" << base << "}";
	}
	
	std::clog << std::endl;
}


} //namespace bamboo