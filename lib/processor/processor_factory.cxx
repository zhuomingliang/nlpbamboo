#include <vector>
#include <string>
#include "processor_factory.hxx"
#include "processor.hxx"
#include "iconfig.hxx"

#include "break_processor.hxx"
#include "crf_ner_np_processor.hxx"
#include "crf_ner_nr_processor.hxx"
#include "crf_ner_ns_processor.hxx"
#include "crf_ner_nt_processor.hxx"
#include "crf_pos_processor.hxx"
#include "crf_seg4ner_processor.hxx"
#include "crf_seg_processor.hxx"
#include "maxforward_combine_processor.hxx"
#include "maxforward_processor.hxx"
#include "prepare_processor.hxx"
#include "single_combine_processor.hxx"
#include "ugm_seg_processor.hxx"


namespace bamboo {
	ProcessorFactory* ProcessorFactory::_instance = NULL;
    Processor *ProcessorFactory::create(const char *name, bool verbose)
	{		
        if (_config == NULL)
			throw std::runtime_error(std::string("no configuration specified"));
		if (name == NULL)
			throw std::runtime_error(std::string("no name specified"));

#define register_processor(N, C) if (strcmp(name, (N)) == 0) return new (C)(_config)
        register_processor("break", BreakProcessor);
        register_processor("crf_ner_np", CRFNPProcessor);
        register_processor("crf_ner_nr", CRFNRProcessor);
        register_processor("crf_ner_ns", CRFNSProcessor);
        register_processor("crf_ner_nt", CRFNTProcessor);
        register_processor("crf_pos", CRFPosProcessor);
        register_processor("crf_seg4ner", CRFSeg4nerProcessor);
        register_processor("crf_seg", CRFSegProcessor);
        register_processor("maxforward_combine", MaxforwardCombineProcessor);
        register_processor("maxforward", MaxforwardProcessor);
        register_processor("prepare", PrepareProcessor);
        register_processor("single_combine", SingleCombineProcessor);
        register_processor("ugm_seg", UnigramProcessor);  
#undef register_parser
        return NULL;
	}
}
