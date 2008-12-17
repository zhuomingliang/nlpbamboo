#ifndef KEA_SEGMENT_TOOL_HXX
#define KEA_SEGMENT_TOOL_HXX

#include "ilexicon.hxx"
#include "datrie.hxx"
#include "utf8.hxx"
#include <vector>
#include "kea_doc.hxx"
#include "config_factory.hxx"
#include "processor_factory.hxx"

namespace bamboo { namespace kea {


class SegmentTool {
protected:
	bool _is_init;
	std::vector<bamboo::Token> _tokens;
	DATrie * _punct;

	int _verbose;
	int _use_break;
	int _use_single_combine;
	std::vector<TokenImpl *> _token_fifo[2];
	std::vector<TokenImpl *> *_in, *_out, *_swap;
	std::vector<Processor *> _procs;

public:
	static SegmentTool &get_instance() {
		static SegmentTool tool;
		return tool;
	}

	SegmentTool()
		:_is_init(false),_verbose(0),_in(&_token_fifo[0]),_out(&_token_fifo[1])
	{
	}

	~SegmentTool() {
		size_t i;
		i = _procs.size();
		while(i--) delete _procs[i];
		_procs.clear();
	}

	int init(IConfig * config) {
		if(_is_init) {
			_fini();
		}

		bool use_ner = false;
		std::vector<std::string> ner_chain;
		std::vector<std::string>::iterator nit;

		(*config)["prepare_characterize"] = "1";
		(*config)["ner_output_type"] = "1";

		config->get_value("verbose", _verbose);
		config->get_value("_use_break", _use_break);
		config->get_value("_use_single_combine", _use_single_combine);
		config->get_value("_ner_chain", ner_chain);
		if(ner_chain.size() > 0) use_ner = true;
		
		ProcessorFactory * factory;
		factory = ProcessorFactory::get_instance();
		factory->set_config(config);

		_procs.push_back(factory->create("prepare", _verbose));
		if(use_ner)
			_procs.push_back(factory->create("crf_seg4ner", _verbose));
		else
			_procs.push_back(factory->create("crf_seg", _verbose));
		if(use_ner) {
			for(nit=ner_chain.begin(); nit!=ner_chain.end(); ++nit) {
				_procs.push_back(factory->create(nit->c_str(), _verbose));
			}
		} else {
			if (_use_single_combine)
				_procs.push_back(factory->create("single_combine", _verbose));
			if (_use_break)
				_procs.push_back(factory->create("break", _verbose));
		}

		const char * punctuation;
		config->get_value("ke_punctuation", punctuation);

		_punct = new DATrie;
		size_t length, i;
		char uch[4];
		length = utf8::length(punctuation);
		for (i = 0; i < length; i++) {
			utf8::sub(uch, punctuation, i, 1);
			_punct->insert(uch, i+1);
		}

		_is_init = true;

		return 0;
	}

	size_t segment(const char * text, std::vector<YCToken *> &vec) {
		return _segment(text, vec);
	}

	bool is_punct(const char * token) {
		int v = _punct->search(token);
		if(v>0) return true;
		return false;
	}

protected:
	size_t _segment(const char * s, std::vector<YCToken *> &out)
	{
		if(!_is_init) {
			return 0;
		}

		size_t i, length;

		length = utf8::length(s);
		_in->clear();
		if (length > _in->capacity()) {
			_in->reserve(length << 1);
			_out->reserve(length << 1);
		}
		_in->push_back(new TokenImpl(s));
		length = _procs.size();
		for (i = 0; i < length; i++) {
			_out->clear();
			_procs[i]->process(*_in, *_out);
			/* switch in & out queue */
			_swap = _out;
			_out = _in;
			_in = _swap;
		}

		length = _in->size();

		if(out.size() > 0 ) out.clear();
		out.reserve(length);

		for(i=0; i<length; ++i) {
			out.push_back(new YCToken((*_in)[i]->get_orig_token(), (*_in)[i]->get_pos()));
			delete (*_in)[i];
		}

		return _in->size();
	}

	void _fini() {
		if(_is_init) {
			size_t i;
			i = _procs.size();
			while(i--) delete _procs[i];
			_procs.clear();
			delete _punct;
			_is_init = false;
		}
	}
};

}} // end of namespace bamboo::kea

#endif // end of KEA_SEGMENT_TOOL_HXX
