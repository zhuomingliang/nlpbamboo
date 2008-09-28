#ifndef PROCESSOR_HXX
#define PROCESSOR_HXX

#include <vector>
#include "config_factory.hxx"
#include "lex_token.hxx"

#define PROCESSOR_MAGIC /* Nothing */
#define PROCESSOR_MODULE(NAME) extern "C" {Processor *create_processor(IConfig *config) {return new NAME(config);}}

class Processor {
protected:
	virtual bool _can_process(LexToken *) = 0;
	virtual void _process(LexToken *token, std::vector<LexToken *> &out) = 0;

public:
	Processor() {};
	Processor(IConfig *_config) {};
	virtual ~Processor() {};

	virtual void process(std::vector<LexToken *> &in, std::vector<LexToken *> &out)
	{
		size_t i, length;

		if (in.empty()) return;
		length = in.size();
		for (i = 0; i < length; i++) {
			if (_can_process(in[i])) {
				_process(in[i], out);
				delete in[i];
			} else {
				out.push_back(in[i]);
			}
		}
	}
};

#endif // PROCESSOR_HXX
