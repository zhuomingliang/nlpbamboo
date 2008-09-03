#include "trie_debugger.hxx"

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
