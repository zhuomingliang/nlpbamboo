#ifndef TRIE_LEXICON_HXX
#define TRIE_LEXICON_HXX

#include <cstdio>

#include "datrie.hxx"

template<class TrieType>
class TrieLexicon:public ILexicon {
	friend class TrieDebugger;
protected:
	TrieLexicon();
public:
	TrieType *_trie;
	TrieLexicon(int size)
	{
		_trie = new TrieType(size);
	}
	
	TrieLexicon(const char *filename)
	{
		assert(filename);
		_trie = new TrieType(filename);
	}
	
	~TrieLexicon()
	{
		delete _trie;
	}
	
	void insert(const char *s, int val)
	{
		_trie->insert(s, val);
	}

	int search(const char *s)
	{
		return _trie->search(s);
	}
	
	int operator[](const char *s)
	{
		return search(s);
	}

	void save(const char *filename) 
	{
		assert(filename);
		_trie->save(filename);
	}	

	void explore(on_explore_finish_t cb, void *arg) {
		_trie->explore(cb, arg);
	}

	void read_from_text(const char *filename, bool verbose)
	{
		FILE *fp;
		char str[4096];
		int val;
		size_t i;

		assert(filename);
		fp = fopen(filename, "r");
		if (verbose)
			std::clog << "Making Index" << std::endl;
		for(i = 0; ; i++) {
			if (fscanf(fp, "%d %4096[^\n]", &val, str) == EOF) break;
			insert(str, val);
			if (verbose && i % 500 == 0) 
				std::clog << "\r\t\t" << i << " items processed.";
		}
		fclose(fp);
		if (verbose)
			std::clog << "\r\t\t" << i << " items processed." << std::endl;
	}

	void write_to_text(const char *filename)
	{
		FILE *fp;

		assert(filename);
		fp = fopen(filename, "w+");
		explore(_export, fp);		
		fclose(fp);
	}

	int max_value()
	{
		return _trie->max_value();
	}

	int min_value()
	{
		return _trie->min_value();
	}

	int sum_value()
	{
		return _trie->sum_value();
	}

	int num_insert()
	{
		return _trie->num_insert();
	}

	//void read_from_text(const char *filename, bool verbose=false);
	//void write_to_text(const char *filename);
};

#endif // TRIE_LEXICON_HXX
