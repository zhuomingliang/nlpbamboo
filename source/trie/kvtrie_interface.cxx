#include "kvtrie_interface.h"
#include "kvtrie.hxx"

void *kvtrie_open(const char *index, const char *data)
{
	KVTrie *kvt;
	kvt = new KVTrie(index, data);
	return kvt;
}

const char *kvtrie_search(void *handle, const char *k)
{
	KVTrie *kvt = (KVTrie *)handle;
	const char *v;
	return (*kvt)[k];
}

void kvtrie_close(void *handle)
{
	delete (KVTrie *)handle;
}
