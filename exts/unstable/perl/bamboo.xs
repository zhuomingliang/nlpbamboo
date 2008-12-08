#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

#include "ppport.h"


MODULE = bamboo		PACKAGE = bamboo		

void* init(name, cfg)
  const char *name;
  const char *cfg;
  CODE:
  	const char default_name[] = "crf_seg";
  	if (*name == '\0')
		name = default_name;
	RETVAL = (void *)bamboo_init(name, cfg);
  OUTPUT:
    RETVAL

void clean(handle)
  void *handle;
  CODE:
	bamboo_clean(handle);

void setopt(handle, opt, arg)
  void			*handle;
  long			opt
  void			*arg;
  CODE:
	bamboo_setopt(handle, opt, arg);


SV* parse(handle)
  void *handle;
  PPCODE:
  	char *t;
    {
	  t = bamboo_parse(handle);
	  XPUSHs(sv_2mortal(newSVpv(t, 0)));
	}
