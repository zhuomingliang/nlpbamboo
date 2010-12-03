#ifndef KEA_TEXT_PARSER_HXX
#define KEA_TEXT_PARSER_HXX

#include "kea_doc.hxx"
#include "segment_tool.hxx"
#include "config_factory.hxx"
#include "token_filter.hxx"
#include <vector>

namespace bamboo { namespace kea {

class TextParser {
protected:
	int _split_line(const char * text, std::vector<char *> &blocks);
	int _split_sentence(std::vector<YCToken *> &token_list, YCDoc & doc);
	SegmentTool *_segment_tool;
	TokenFilter *_token_filter;

	bool _is_filter_word(YCToken * token);

protected:
	int _feature_min_length;
	int _feature_min_utf8_length;

public:
	TextParser(IConfig *);
	~TextParser();

	int parse_text(const char * text, YCDoc &doc);
	int parse_text(const char * title, const char * text, YCDoc &doc);
};

}} //end of namespace bamboo::kea

#endif
