MODULE_big = pg_cnlexizer
OBJS = pg_cnlexizer.o
DATA_built = pg_cnlexizer.sql
PGXS := $(shell pg_config --pgxs)
PG_CPPFLAGS += -DNDEBUG  -I../../../source/cnlexizer
SHLIB_LINK=-L../../../build/lib -lcnlexizer -lstdc++
include $(PGXS)
