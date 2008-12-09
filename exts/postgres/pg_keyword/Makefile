MODULE_big = pg_keyword
OBJS = pg_keyword.o

DATA_built = pg_keyword.sql
DATA = uninstall_pg_keyword.sql

PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
PG_CPPFLAGS += -DNDEBUG  -I/usr/include/bamboo
SHLIB_LINK = -L/usr/lib -lbamboo -lstdc++
include $(PGXS)
