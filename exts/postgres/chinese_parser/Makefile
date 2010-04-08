# chinese tesearch build-in, following the postgresql contrib - test_parser

MODULE_big = chinese_parser
OBJS = chinese_parser.o
DATA_built = chinese_parser.sql
DATA = uninstall_chinese_parser.sql
REGRESS = chinese_parser

PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
PG_CPPFLAGS += -fPIC -DNDEBUG -I/usr/local/include/bamboo 
SHLIB_LINK=-L/usr/lib -lbamboo -lstdc++
include $(PGXS)
