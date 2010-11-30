#ifndef PROCESSOR_FACTORY_HXX
#define PROCESSOR_FACTORY_HXX

#include <vector>
#include "iconfig.hxx"
#include "processor.hxx"

namespace bamboo {

class ProcessorFactory
{
private:

	static ProcessorFactory		*_instance;

protected:
	typedef Processor* (*_create_processor_t)(IConfig *);

	static std::vector<void *>	_dl_handles;
	IConfig 					*_config;
	
	ProcessorFactory():_config(NULL) {}
	ProcessorFactory(const ProcessorFactory&):_config(NULL) {}
	ProcessorFactory& operator= (const ProcessorFactory &) { return *this;}
public:

	static ProcessorFactory *get_instance()
	{
		if (_instance == NULL)
			_instance = new ProcessorFactory();

		return _instance;
	}

	void set_config(IConfig *config)
	{
		_config = config;
	}

	static void destroy_instance()
	{
		size_t i;
		i = _dl_handles.size();
		while(i--) 
			dlclose(_dl_handles[i]);
		_dl_handles.clear();
	}

	Processor *create(const char *name, bool verbose=false);
};

};

#endif /* PROCESSOR_FACTORY_HXX */
