#include "processor_factory.hxx"

namespace bamboo {
	ProcessorFactory* ProcessorFactory::_instance = NULL;
	std::vector<void *> ProcessorFactory::_dl_handles;
}
