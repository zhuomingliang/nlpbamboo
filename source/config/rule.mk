CURDIR := $(SRCTOP)/source/config
VPATH := $(VPATH) $(CURDIR) 
SRCS := $(SRCS) $(get_src)

$(eval $(call LIB_template,config,simple_config config_factory))
