#ifndef KVTRIE_HXX
#define KVTRIE_HXX

#include <fstream>
#include <iostream>
#include <stdexcept>

#include "mmap.hxx"
#include "datrie.hxx"

class KVTrie {
protected:
	DATrie _index;
	MMap _data;

public:
	KVTrie(const char *index, const char *data)
		:_index(index), _data(data)
	{
	}

	static void build_from_text(const char *index, const char *data, 
			const char *source, bool verbose = false)
	{
		std::ifstream ifs;
		std::ofstream ofs;
		std::string s;
		int value_start, offset;
		DATrie trie;

		if (verbose)
			std::cout << "building from " << source << std::endl;
		ifs.open(source);
		ofs.open(data);
		if (!ifs.is_open()) throw std::runtime_error("error on reading source");
		if (!ofs.is_open()) throw std::runtime_error("error on writting data");
		ofs << "\n";
		while(!ifs.eof()) {
			std::getline(ifs, s);
			if (s.empty()) continue;
			value_start = s.find(' ') + 1;
			trie.insert(s.substr(0, value_start - 1).c_str(), ofs.tellp());
			ofs << s.substr(value_start);
			ofs.write("", 1);
		}
		trie.save(index);
	}

	
	const char* operator[](const char * k)
	{
		int v = _index.search(k);
		if (v > 0)
			return (const char *)_data.start(_index.search(k));
		else
			return NULL;
	}
};


#endif // KVTRIE_HXX
