#ifndef KVTRIE_INTERFACE_H
#define KVTRIE_INTERFACE_H

extern "C" {
	void *kvtrie_open(const char *index, const char *data);
	const char *kvtrie_search(void *handle, const char *k);
	void kvtrie_close(void *handle);
};

#endif
