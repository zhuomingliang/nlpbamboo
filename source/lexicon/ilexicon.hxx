#ifndef ILEXICON_HXX
#define ILEXICON_HXX

#include <cassert>
#include <cstdio>

class ILexicon {
protected:
	static void _export(const char *s, int val, void *arg) 
	{
		FILE *fp = (FILE *)arg;
		fprintf(fp, "%d %s\n", val, s);
	}
public:
	ILexicon() {};
	ILexicon(int size) {};
	ILexicon(const char *filename) {};

	virtual void insert(const char*, int val) = 0;
	virtual int search(const char *) = 0;
	virtual int operator[](const char *) = 0;
	virtual void save(const char *filename) = 0;
	virtual void read_from_text(const char *filename, bool verbose) = 0;
	virtual void write_to_text(const char *filename) = 0;
	virtual int max_value() = 0;
	virtual int min_value() = 0;
	virtual int sum_value() = 0;
	virtual int num_insert() = 0;
	virtual ~ILexicon() {};
};

#endif // ILEXICON_HXX
