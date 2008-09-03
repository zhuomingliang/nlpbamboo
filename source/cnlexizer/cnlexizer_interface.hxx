#ifndef LIB_CNLEXIZER_HXX
#define LIB_CNLEXIZER_HXX
enum cnlexizer_method {
	CLX_DEFAULT = 0,
	CLX_UNIGRAM,
	CLX_BIGRAM,
	CLX_MAXFORWARD,
	CLX_MAXBACKWARD,
	CLX_UNKNOW
};
#ifdef __cplusplus
extern "C" {
#endif	
size_t cnlexizer_process(void *handle, char *t, const char *s);
void *cnlexizer_init(const char *s);
void cnlexizer_clean(void *handle);
#ifdef __cplusplus
};
#endif

#endif //LIB_CNLEXIZER_HXX

