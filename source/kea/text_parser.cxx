#include "text_parser.hxx"
#include "kea_mmap.hxx"
#include "segment_tool.hxx"
#include <cstdio>

namespace bamboo { namespace kea {

TextParser::TextParser(IConfig * config)
	:_segment_tool(NULL)
{
	SegmentTool &tool = SegmentTool::get_instance();
	_segment_tool = &tool;
	TokenFilter &fil = TokenFilter::get_instance();
	_token_filter = &fil;

	config->get_value("ke_feature_min_length", _feature_min_length);
	config->get_value("ke_feature_min_utf8_length", _feature_min_utf8_length);
}

TextParser::~TextParser() {
}

int TextParser::_split_line(const char * text, std::vector<char *> & blocks)  {
	if(!text) return 0;

	enum { min_block_size = 10 };
	size_t len = strlen(text);
	char * text_buf = new char [len+1], * tb_ptr = text_buf;
	blocks.reserve(min_block_size);

	for(;;) {
		if(*text=='\n' || *text=='\r' || *text=='\0') {
			if(tb_ptr > text_buf) {
				*tb_ptr = 0;
				if(blocks.size() == blocks.capacity()) {
					blocks.reserve(blocks.capacity()*2);
				}
				blocks.push_back(strdup(text_buf));
				tb_ptr = text_buf;
			}
			if(*text==0) break;
		} else {
			*tb_ptr++ = *text;
		}
		text++;
	}

	delete [] text_buf;
	return 0;
}

int TextParser::_split_sentence(std::vector<YCToken *> &token_list, YCDoc & doc) {
	size_t i, len;
	len = token_list.size();
	YCSentence * sent = new YCSentence();
	for(i=0; i<len; i++) {
		char * token = token_list[i]->get_token();
		if(_segment_tool->is_punct(token)) {
			if(sent->token_list.size()>0) {
				doc.sent_list.push_back(sent);
				sent = new YCSentence();
			}
			delete token_list[i];
		} else if(_is_filter_word(token_list[i])) {
			delete token_list[i];
		}else {
			sent->token_list.push_back(token_list[i]);
		}
	}
	if(sent->token_list.size()>0) {
		doc.sent_list.push_back(sent);
	} else {
		delete sent;
	}
	return 0;
}

bool TextParser::_is_filter_word(YCToken * token) {
	return _token_filter->is_filter_word(token);
}

int TextParser::parse_text(const char * text, YCDoc & doc) {
	return parse_text(NULL, text, doc);
}

int TextParser::parse_text(const char * title, const char * text, YCDoc & doc) {
	std::vector<YCToken *> tokens;
	if(title) {
		YCSentence * sent = new YCSentence();
		sent->is_title = true;
		_segment_tool->segment(title, tokens);
		size_t i, size = tokens.size();
		for(i=0; i<size; ++i) {
			if(_is_filter_word(tokens[i])) {
				delete tokens[i];
			} else {
				sent->token_list.push_back(tokens[i]);
			}
		}
		doc.sent_list.push_back(sent);
	}
	if(text) {
		//split line
		std::vector<char *> text_block;
		_split_line(text, text_block);

		std::vector<char *>::iterator tb_it
			= text_block.begin();
		for(; tb_it != text_block.end(); ++tb_it) {
			//word segment
			_segment_tool->segment(*tb_it, tokens);
			//split sentence according to punctuation
			_split_sentence(tokens, doc);
			free(*tb_it);
		}

		text_block.clear();
	}
	return 0;
}

}} //end of namespace bamboo::kea

