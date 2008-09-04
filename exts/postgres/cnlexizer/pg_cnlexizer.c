#include <unistd.h>
#include <assert.h>
#include <string.h>

#include "postgres.h"
#include "utils/builtins.h"
#include "cnlexizer_interface.hxx"

PG_MODULE_MAGIC;

PG_FUNCTION_INFO_V1(cnlexizer);
Datum cnlexizer(PG_FUNCTION_ARGS);

static void *handle = NULL;

void _PG_init(void);
void _PG_fini(void);

void _PG_init(void)
{
	handle = cnlexizer_init("/etc/cnlexizer.cfg");
}

void _PG_fini(void)
{
	cnlexizer_clean(handle);
}

Datum cnlexizer(PG_FUNCTION_ARGS)
{
	text *in = PG_GETARG_TEXT_P(0);
	char *s;
	char *t;

	s = (char *) palloc(VARSIZE(in) - VARHDRSZ + 1);
	memcpy(s, VARDATA(in), VARSIZE(in) - VARHDRSZ);
	s[VARSIZE(in) - VARHDRSZ] = '\0';

	if (handle == NULL) elog(ERROR, "can not init segmentor");
	t = palloc((VARSIZE(in) - VARHDRSZ + 1) * 2 + 1);
	cnlexizer_process(handle, t, s);

	PG_RETURN_TEXT_P(DatumGetTextP(DirectFunctionCall1(textin, CStringGetDatum(t))));
}
