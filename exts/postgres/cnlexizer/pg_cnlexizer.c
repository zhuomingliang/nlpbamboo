/*
 * Copyright (c) 2008, detrox@gmail.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the <organization> nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY <copyright holder> ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <copyright holder> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */

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
	handle = cnlexizer_init(NULL);
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

	if (handle == NULL) 
		elog(ERROR, "cnlexizer init failed.");
	t = palloc(((VARSIZE(in) - VARHDRSZ + 1) << 1) + 1);
	cnlexizer_process(handle, t, s);

	PG_RETURN_TEXT_P(DatumGetTextP(DirectFunctionCall1(textin, CStringGetDatum(t))));
}
