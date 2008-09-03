#ifndef LEXICON_FACTORY_HXX
#define LEXICON_FACTORY_HXX

#include <cstring>
#include <exception>
#include <stdexcept>

#include "ilexicon.hxx"
#include "trie_lexicon.hxx"

class LexiconFactory {
private:
	LexiconFactory();
public:
	static ILexicon *create(const char *type)
	{
		ILexicon *dc = NULL;
		if (type == NULL) return NULL;
		if (strcmp(type, "datrie") == 0) {
			dc = new TrieLexicon<DATrie>(1024);
		} else if (strcmp(type, "double_array") == 0) {
			dc = new TrieLexicon<DoubleArray>(1024);
		} else {
			throw std::runtime_error("unknow lexicon file type: " + std::string(type));
		}

		return dc;
	}

	static ILexicon *load(const char *filename)
	{
		FILE *fp = NULL;
		char magic[32];

		if (filename == NULL) return NULL;
		fp = fopen(filename, "r");
		if (fp == NULL) throw std::runtime_error("can not open lexicon file: " + std::string(filename));
		fread(magic, sizeof(magic), 1, fp);
		fclose(fp);

		if (strcmp(magic, "datrie") == 0) {
			return new TrieLexicon<DATrie>(filename);
		} else if (strcmp(magic, "double_array") == 0) {
			return new TrieLexicon<DoubleArray>(filename);
		}
		return NULL;
	}
};

#endif // LEXICON_FACTORY_HXX
