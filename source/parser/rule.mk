CURDIR := $(SRCTOP)/source/parser
VPATH := $(VPATH) $(CURDIR) 
SRCS := $(SRCS) $(get_src)

$(call INST_HEADER,$(SRCTOP)/source/parser/parser_factory.hxx $(SRCTOP)/source/parser/parser.hxx,bamboo/)

