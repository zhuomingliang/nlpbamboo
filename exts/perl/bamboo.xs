#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

#include "ppport.h"


MODULE = bamboo		PACKAGE = bamboo		

void* init(cfg)
  const char *cfg;
  CODE:
	RETVAL = (void *)bamboo_init(cfg);
  OUTPUT:
    RETVAL

void clean(handle)
  void *handle;
  CODE:
	bamboo_clean(handle);

SV* parse(handle, text)
  void *handle;
  const char *text;
  PPCODE:
    {
	  const char *t;
	  bamboo_parse(handle, &t, text);
	  XPUSHs(sv_2mortal(newSVpv(t, 0)));
	}
