CURDIR := $(SRCTOP)/source/parser
VPATH := $(VPATH) $(CURDIR) 
SRCS := $(SRCS) $(get_src)


INST_INCFILES += $(SRCTOP)/source/parser/parser_factory.hxx
INST_INCFILES += $(SRCTOP)/source/parser/parser.hxx
