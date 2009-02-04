#ifndef PROCESSOR_FACTORY_HXX
#define PROCESSOR_FACTORY_HXX

#include <dlfcn.h>

#include <string>
#include <cassert>

#include "utf8.hxx"
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

	Processor *create(const char *name, bool verbose=false)
	{
		std::string			module;
		const char			*processor_root;
		void				*handle;
		Processor			*processor;
		_create_processor_t	create;

		if (_config == NULL)
			throw std::runtime_error(std::string("no configuration specified"));
		if (name == NULL)
			throw std::runtime_error(std::string("no name specified"));

		_config->get_value("processor_root", processor_root);
		module.append(processor_root).append("/").append(name).append(".so");

		if (verbose)
			std::cerr << "creating processor " << module << std::endl;
		if (!(handle = dlopen(module.c_str(), RTLD_NOW | RTLD_GLOBAL)))
			throw std::runtime_error(std::string(dlerror()));
		_dl_handles.push_back(handle);
		if (!(create = (_create_processor_t)dlsym(handle, "create_processor")))
			throw std::runtime_error(std::string(dlerror()));
		if (!(processor = create(_config)))
			throw std::runtime_error(std::string("can not create processor: ") + module);
		
		return processor;
	}
};

};

#endif /* PROCESSOR_FACTORY_HXX */
