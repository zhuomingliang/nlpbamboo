#ifndef IPROCESSOR_HXX
#define IPROCESSOR_HXX

#include "config_factory.hxx"
#include <vector>

#define TUPLE_TYPE_UNKNOW 0 

class LexAttribute {
public:
	LexAttribute() {};
	LexAttribute(void *) {};
};

class IProcessor {
public:
	IProcessor() {};
	IProcessor(IConfig *_config) {};
	virtual ~IProcessor() {};

virtual int process(std::vector< std::pair<std::string, LexAttribute> > &prod) = 0;
};

#endif // IPROCESSOR_HXX
