MODULE_big = bamboo
OBJS = bamboo.o
DATA_built = bamboo.sql
PGXS := $(shell pg_config --pgxs)
PG_CPPFLAGS += -DNDEBUG  -I/usr/include/bamboo
SHLIB_LINK=-L/usr/lib -lbamboo -lstdc++
include $(PGXS)
