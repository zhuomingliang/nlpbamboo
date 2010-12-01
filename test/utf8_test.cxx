#include <iostream>
#include <string>
#include <cstddef>
#include "utf8.hxx"
using namespace std;
using namespace bamboo;

bool test_dbc2sbc() {
    const char *dbcs = "　！＂＃＄％＆＇（）＊＋，－．／０１２３４５６７８９：；＜＝＞？＠ＡＢＣＤＥＦＧＨＩＪＫＬＭＮＯＰＱＲＳＴＵＶＷＸＹＺ［＼］＾＿｀ａｂｃｄｅｆｇｈｉｊｋｌｍｎｏｐｑｒｓｔｕｖｗｘｙｚ｛｜｝～";
    const char *sbcs = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";

	char uch[8];
    size_t step;
	for (const char *dbc_ptr = dbcs, *sbc_ptr = sbcs; ; dbc_ptr += step, ++sbc_ptr) {
		step = utf8::first(dbc_ptr, uch);
		char cch = utf8::dbc2sbc(uch, step);
        if (cch != *sbc_ptr) {
            return false;
        }
    }
    return true;
}

int main(int argc, char **argv) {
    bool dbc2sbc_test_passed = test_dbc2sbc();
    return dbc2sbc_test_passed ? EXIT_SUCCESS : EXIT_FAILURE;
}
