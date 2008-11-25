#ifndef YCAKE_SEGMENT_TOOL_HXX
#define YCAKE_SEGMENT_TOOL_HXX

#include "bamboo.hxx"
#include "ilexicon.hxx"
#include "datrie.hxx"
#include "utf8.hxx"
#include "ycake_doc.hxx"
#include "config_factory.hxx"

namespace bamboo { namespace ycake {


class SegmentTool {
protected:
	bool _is_init;
	bamboo::Parser *_parser;
	std::vector<bamboo::Token> _tokens;
	DATrie _punct;

public:
	static SegmentTool &get_instance() {
		static SegmentTool tool;
		return tool;
	}

	SegmentTool()
		:_is_init(false),_parser(NULL)
	{
	}

	~SegmentTool() {
		if(_parser) delete _parser;
	}

	int init(IConfig * config) {
		const char * bamboo_cfg;
		config->get_value("ke_bamboo_cfg", bamboo_cfg);

		try {
			_parser = new bamboo::Parser(bamboo_cfg);
		} catch(std::exception &e) {
#ifdef DEBUG
			std::cerr << __func__ << ": " << e.what();
#endif
			return -1;
		}
		_is_init = true;

		const char * punctuation;
		config->get_value("ke_punctuation", punctuation);

		size_t length, i;
		char uch[4];
		length = utf8::length(punctuation);
		for (i = 0; i < length; i++) {
			utf8::sub(uch, punctuation, i, 1);
			_punct.insert(uch, i+1);
		}

		return 0;
	}

	size_t segment(const char * text, std::vector<YCToken *> &vec) {
		return bambooSegment(text, vec);
	}

	size_t bambooSegment(const char * text, std::vector<YCToken *> &vec) {
		if(!_parser) return 0;

		_tokens.clear(); vec.clear();
		_parser->parse(_tokens, text);
		size_t i, len;
		len = _tokens.size();
		for(i=0; i<len; ++i) {
			vec.push_back(new YCToken(_tokens[i].token, _tokens[i].pos));
		}
		return len;
	}

	bool is_punct(const char * token) {
		int v = _punct.search(token);
		if(v>0) return true;
		return false;
	}
};

}} // end of namespace bamboo::ycake

#endif // end of YCAKE_SEGMENT_TOOL_HXX
