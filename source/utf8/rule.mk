CURDIR := $(SRCTOP)/source/utf8
VPATH := $(VPATH) $(CURDIR) 
SRCS := $(SRCS) $(get_src)


$(eval $(call LIB_template,utf8,utf8))

# $(eval $(call SO_template,mod_cnqp_db,mod_cnqp_db))
# $(eval $(call EXE_template,aprtest,aprtest))



