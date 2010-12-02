#include "utf8.hxx"
using namespace bamboo;

bool test_dbc2sbc() {
    const char *dbcs = "　！＂＃＄％＆＇（）＊＋，－．／０１２３４５６７８９：；＜＝＞？＠ＡＢＣＤＥＦＧＨＩＪＫＬＭＮＯＰＱＲＳＴＵＶＷＸＹＺ［＼］＾＿｀ａｂｃｄｅｆｇｈｉｊｋｌｍｎｏｐｑｒｓｔｕｖｗｘｙｚ｛｜｝～";
    const char *sbcs = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";

	char uch[8];
    size_t step;
    const char *dbc_ptr = dbcs, *sbc_ptr = sbcs;
	for (; *dbc_ptr && *sbc_ptr; dbc_ptr += step, ++sbc_ptr) {
		step = utf8::first(dbc_ptr, uch);
		char cch = utf8::dbc2sbc(uch, step);
        if (cch != *sbc_ptr) {
            return false;
        }
    }
    if (*dbc_ptr || *sbc_ptr) {
        return false;
    } else {
        return true;
    }
}

int main(int argc, char **argv) {
    return test_dbc2sbc() ? EXIT_SUCCESS : EXIT_FAILURE;
}
