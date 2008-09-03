#ifndef TRIE_DEBUGGER_HXX
#define TRIE_DEBUGGER_HXX

#include <iostream>
#include <vector>

#include "double_array.hxx"
#include "datrie.hxx"

class TrieDebugger {
protected:
	
	DoubleArray *_darray;
	DATrie *_datrie;
	std::vector<int> _states;

	TrieDebugger() {}
	void _trace_finish(); 

public:
	TrieDebugger(DoubleArray *trie):_darray(trie) {}
	TrieDebugger(DATrie *trie):_darray(trie), _datrie(trie) {}
	~TrieDebugger() {}
	void trace(int s);


};

#endif // TRIE_DEBUGGER_HPP
