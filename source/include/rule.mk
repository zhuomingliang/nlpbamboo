CURDIR := $(SRCTOP)/source/include
VPATH := $(VPATH) $(CURDIR) 
SRCS := $(SRCS) $(get_src)
$(call INST_HEADER,$(SRCTOP)/source/include/bamboo.hxx $(SRCTOP)/source/include/bamboo_defs.h,bamboo/ )




