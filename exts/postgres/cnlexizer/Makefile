MODULE_big = pg_cnlexizer
OBJS = pg_cnlexizer.o
DATA_built = pg_cnlexizer.sql
PGXS := $(shell pg_config --pgxs)
PG_CPPFLAGS += -DNDEBUG  -I/opt/cnlexizer/include
SHLIB_LINK=-L/opt/cnlexizer/lib -lcnlexizer -lstdc++
include $(PGXS)
