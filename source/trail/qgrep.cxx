#include <cstdlib>
#include <cstdio>
#include <iostream>
#include "lexicon_factory.hxx"

static int _qgrep(const char *pat, const char *src)
{
	char str[4096];
	TrieLexicon<DATrie> dc(1024);
	FILE *fsrc = NULL, *fpat = NULL;
	int i = 0;

	/* load source */
	if ((fsrc = fopen(src, "r")) == NULL) {
		std::cerr << "can not open source file " << src << std::endl;
		return -1;
	}
	while(1) {
		int cnt;
	
		if (fscanf(fsrc, "%4096s", str) == EOF) break;
		cnt = dc.search(str) + 1;
		dc.insert(str, cnt);
		++i;
		if (i % 10000 == 0)
			std::clog << i << " items processed" << std::endl;
	}
	fclose(fsrc);

	if ((fpat = fopen(pat, "r")) == NULL) {
		std::cerr << "can not open pattern file " << pat << std::endl;
		return -1;
	}
	while(1) {
		int val;
	
		if (fscanf(fpat, "%4096s", str) == EOF) break;
		val = dc.search(str);
		std::cout << str << "\t" << val << std::endl;
	}
	fclose(fpat);

	return 0;
}

int main(int argc, char *argv[])
{
	if (argc < 2 || !argv[1] || !argv[2]) {
		std::cerr << "Usage: qgrep pattern log\n"
				     "\n"
				     "Report bug to jianing.yang@alibaba-inc.com\n"
				  << std::endl;
		return 0;
	}

	return _qgrep(argv[1], argv[2]);
}
