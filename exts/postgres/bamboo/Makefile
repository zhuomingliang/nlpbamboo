MODULE_big = bamboo
OBJS = bamboo.o
DATA_built = bamboo.sql
PGXS := $(shell pg_config --pgxs)
PG_CPPFLAGS += -DNDEBUG  -I/opt/bamboo/include
SHLIB_LINK=-L/opt/bamboo/lib -lbamboo -lstdc++
include $(PGXS)
