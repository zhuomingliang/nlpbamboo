CURDIR := $(SRCTOP)/source/common
VPATH := $(VPATH) $(CURDIR) 
SRCS := $(SRCS) $(get_src)

$(call INST_HEADER,$(SRCTOP)/source/common/token.hxx,bamboo/ )
