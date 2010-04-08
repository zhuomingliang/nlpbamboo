MODULE_big = pg_tokenize
OBJS = pg_tokenize.o

DATA_built = pg_tokenize.sql
DATA = uninstall_pg_tokenize.sql

PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
PG_CPPFLAGS += -DNDEBUG  -I/usr/local/include/bamboo
SHLIB_LINK = -L/usr/lib -lbamboo -lstdc++
include $(PGXS)
