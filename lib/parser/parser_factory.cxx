#include <string>
#include <cassert>

#include "crf_ner_nr_parser.hxx"
#include "crf_ner_ns_parser.hxx"
#include "crf_ner_nt_parser.hxx"
#include "crf_ner_np_parser.hxx"
#include "crf_seg_parser.hxx"
#include "crf_pos_parser.hxx"
#include "custom_parser.hxx"
#include "keyword_parser.hxx"
#include "ugm_seg_parser.hxx"
#include "mfm_seg_parser.hxx"

#include "parser_factory.hxx"

namespace bamboo {
	ParserFactory* ParserFactory::_instance = NULL;

	ParserFactory::ParserFactory():_config(NULL) {}
	ParserFactory::ParserFactory(const ParserFactory&):_config(NULL) {}
	ParserFactory& ParserFactory::operator= (const ParserFactory &) {return *this;}

	ParserFactory *ParserFactory::get_instance()
	{
		if (_instance == NULL)
			_instance = new ParserFactory();

		return _instance;
	}

	Parser *ParserFactory::create(const char *name, const char *cfg, bool verbose)
	{
#define register_parser(N, C) if (strcmp(name, (N)) == 0) return new (C)(cfg, verbose);
		register_parser("ugm_seg", UGMSegParser);
		register_parser("mfm_seg", MFMSegParser);
		register_parser("crf_seg", CRFSegParser);
		register_parser("crf_pos", CRFPosParser);
		register_parser("crf_ner_nr", CRFNRParser);
		register_parser("crf_ner_ns", CRFNSParser);
		register_parser("crf_ner_nt", CRFNTParser);
		register_parser("crf_ner_np", CRFNPParser);
		register_parser("keyword", KeywordParser);
#undef register_parser
		return NULL;
	}
}
