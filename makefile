pathsearch = $(firstword $(wildcard $(addsuffix /$(1),$(subst :, ,$(PATH)))))
QPTOP=$(call pathsearch,qptop)
ifeq ($(QPTOP),)
SRCTOP=.
else
SRCTOP := $(shell qptop)
endif

include $(SRCTOP)/Make.conf
-include $(SRCTOP)/App.conf

# define your cflags here
APPCFLAGS := $(CPPFLAGS_CONF)
APPLDFLAGS := -L$(SRCTOP)/lib $(LDFLAGS_CONF)

OBJDIR := $(SRCTOP)/lib
DEPDIR := $(SRCTOP)/build
CXXFLAGS :=
GEN_FILES :=

all :: predir

dbg :: all


###############################  GCC Compiler 默认设定  ########################
CC := gcc
CXX := g++
CFLAGS := $(APPCFLAGS) $(CFLAGS_CONF_GCC)
LDFLAGS := $(APPLDFLAGS) $(LDFLAGS_CONF_GCC)
CXXFLAGS := $(CXXFLAGS_CONF_GCC)
SH_CFLAGS := -shared -fpic -fPIC
SH_LDFLAGS := -shared -fpic -fPIC -Wl,-soname=$$(notdir $$@)


LIBDIR_AUX := $(SRCTOP)/lib.release
DEPDIR_AUX := $(SRCTOP)/build.release



ifeq ($(MAKECMDGOALS),dbg)
CFLAGS += -g -DQP_DEBUG
LIBDIR_AUX := $(SRCTOP)/lib.debug
DEPDIR_AUX := $(SRCTOP)/build.debug
endif

predir ::
	@test -d $(LIBDIR_AUX) || mkdir -p $(LIBDIR_AUX)
	@test -d $(DEPDIR_AUX) || mkdir -p $(DEPDIR_AUX)
	@rm -rf $(OBJDIR) $(DEPDIR)
	@ln -sf $(LIBDIR_AUX) $(OBJDIR)
	@ln -sf $(DEPDIR_AUX) $(DEPDIR)

################################# 辅助性的模板   ###################################
DEP_P = $(addprefix $(DEPDIR)/,$(notdir $(addsuffix .P,$(basename $@)))) 
DEP_D = $(addprefix $(DEPDIR)/,$(notdir $(addsuffix .d,$(basename $@)))) 
DEP_DD = $(addprefix $(DEPDIR)/,$(notdir $(addsuffix .dd,$(basename $@)))) 


yes_require = $(if $(1), \
	$(if $(subst yes,,$(1)),$(error please spefify $(2) in your configure)), \
		$(error please spefify $(2) in your configure))

apxs_flags = $(foreach src,$(1),$(eval $(src)_CFLAGS += -I$(shell $(APXS) -q INCLUDEDIR)))

define INST_HEADER
$(foreach src,aa,$(eval 
install ::
	test -d $(DESTDIR)/$(INST_INCDIR)/$(2) || mkdir -p $(DESTDIR)/$(INST_INCDIR)/$(2)
	install -m 0644 $(1) $(DESTDIR)/$(INST_INCDIR)/$(2)
))
endef

define INST_BIN
$(foreach src,aa,$(eval 
install ::
	test -d $(DESTDIR)/$(INST_BINDIR)/$(2) || mkdir -p $(DESTDIR)/$(INST_BINDIR)/$(2)
	install -m 0755 $(1) $(DESTDIR)/$(INST_BINDIR)/$(2)
))
endef

define INST_LIB
$(foreach src,aa,$(eval 
install ::
	test -d $(DESTDIR)/$(INST_BINDIR)/$(2) || mkdir -p $(DESTDIR)/$(INST_BINDIR)/$(2)
	install -m 0755 $(1) $(DESTDIR)/$(INST_BINDIR)/$(2)
))
endef

define ADD_CFLAGS
$(foreach src,$(1),$(eval 
			$(src)_CFLAGS += $(2)
			))
endef

define php_flags
$(foreach src,$(1),$(eval 
			$(src)_CFLAGS += $(shell $(PHP) --includes)
			$(src)_LDLIBS += $(shell $(PHP) --ldflags)
			$(src)_LDLIBS += $(shell $(PHP) --libs)
			))
endef

define yws_flags
$(foreach src,$(1),$(eval 
			$(src)_CFLAGS += $(YWS_INC)
			$(src)_LDLIBS += $(YWS_LIB)
			$(src)_LDLIBS += -lyws))
endef

define curl_flags
$(foreach src,$(1),$(eval 
			$(src)_CFLAGS += $(CURL_INC)
			$(src)_LDLIBS += $(CURL_LIB) -lcurl))
endef
define event_flags
$(foreach src,$(1),$(eval 
			$(src)_CFLAGS += $(EVENT_INC)
			$(src)_LDLIBS += -levent))
endef

define apr_flags
$(foreach src,$(1),$(eval 
			$(src)_CFLAGS += $(shell  $(APR) --cppflags --includes)
			$(src)_LDLIBS += $(shell $(APR) --link-ld)))
endef

define apu_flags
$(foreach src,$(1),$(eval 
			$(src)_CFLAGS += $(shell  $(APU) --includes) 
			$(src)_LDLIBS += $(shell $(APU) --link-ld)))
endef

define ap2_flags
$(foreach src,$(1),$(eval 
			$(src)_CFLAGS += $(APACHE2_INC)))
endef

define apreq2_flags
$(foreach src,$(1),$(eval 
			$(src)_CFLAGS += $(APREQ2_INC)))
endef

define tcl_flags
$(foreach src,$(1),$(eval 
			$(src)_CFLAGS += $(TCL_INC)
			$(src)_LDLIBS   += $(TCL_LIB_FILE)))
endef


define lib_name
$(if $(findstring $(OBJDIR)/lib$(1).a,$(LIB)), \
     $(OBJDIR)/lib$(1).a, \
	 $(if $(findstring $(OBJDIR)/$(1).so,$(SO)), \
	      $(OBJDIR)/$(1).so, \
          $(error Sorry, there is no library named $(1)) \
      ))
endef

define dlib_name
$(if $(findstring $(OBJDIR)/lib$(1)_d.a,$(LIB)), \
     $(OBJDIR)/lib$(1)_d.a, \
	 $(if $(findstring $(OBJDIR)/$(1).so,$(SO)), \
	      $(OBJDIR)/$(1).so, \
          $(error Sorry, there is no library named $(1)) \
      ))
endef

dep_strip = $(filter %.o %.do %.so,$(1)) \
			$(patsubst lib%.a,-l%,$(notdir $(filter %.a, $(1))))

get_src = $(wildcard $(CURDIR)/*.c) $(wildcard $(CURDIR)/*.cpp) \
		  $(wildcard $(CURDIR)/*.cc) $(wildcard $(CURDIR)/*.cxx)

ifneq ($(GCC_VER),2)
define cxx_obj	
	@cpp -MMD -MF $(DEP_P) $(MYCPPFLAGS) $< >/dev/null
	$(CXX) $(CXXFLAGS) -c $(MYCFLAGS) -o $@ $<
	@sed -e 's,\(^.*\.o\),$@ ,' $(DEP_P) >$(DEP_D); rm -f $(DEP_P); \
	echo [DEP] $(DEP_D);
endef	
define cxx_dobj	
	@cpp -MMD -MF $(DEP_P) $(MYCPPFLAGS) $< >/dev/null
	$(CXX)  $(CXXFLAGS) -c $(SH_CFLAGS) $(MYCFLAGS) -o $@ $<
	@sed -e 's,\(^.*\.o\),$@,' $(DEP_P) >$(DEP_DD); rm -f $(DEP_P); \
	echo [DEP] $(DEP_DD);
endef	

define c_obj	
	@cpp -MMD -MF $(DEP_P) $(MYCPPFLAGS) $< >/dev/null
	$(CC) -std=c99 -c $(MYCFLAGS) -o $@ $<
	@sed -e 's,\(^.*\.o\),$@,' $(DEP_P) >$(DEP_D); rm -f $(DEP_P); \
	echo [DEP] $(DEP_D);
endef	
define c_dobj	
	@cpp -MMD -MF $(DEP_P) $(MYCPPFLAGS) $< >/dev/null
	$(CC) -std=c99 -c $(SH_CFLAGS) $(MYCFLAGS) -o $@ $<
	@sed -e 's,\(^.*\.o\),$@,' $(DEP_P) >$(DEP_DD); rm -f $(DEP_P); \
	echo [DEP] $(DEP_DD);
endef	

else  # gcc 2.x 系列的 cpp 处理 -MMD 问题太大了，没法用

define cxx_obj	
	$(CXX) $(CXXFLAGS) -Wp,-MMD,$(DEP_P) -c $(MYCFLAGS) -o $@ $<
	@sed -e 's,\(^.*\.o\),$@ ,' $(DEP_P) >$(DEP_D); rm -f $(DEP_P); \
	echo [DEP] $(DEP_D);
endef	
define cxx_dobj	
	$(CXX) $(SH_CFLAGS) $(CXXFLAGS) -Wp,-MMD,$(DEP_P) -c $(MYCFLAGS) -o $@ $<
	@sed -e 's,\(^.*\.o\),$@ ,' $(DEP_P) >$(DEP_DD); rm -f $(DEP_P); \
	echo [DEP] $(DEP_DD);
endef	
define c_obj	
	$(CC) -Wp,-MMD,$(DEP_P) -c $(MYCFLAGS) -o $@ $<
	@sed -e 's,\(^.*\.o\),$@ ,' $(DEP_P) >$(DEP_D); rm -f $(DEP_P); \
	echo [DEP] $(DEP_D);
endef	
define c_dobj	
	$(CC) $(SH_CFLAGS) -Wp,-MMD,$(DEP_P) -c $(MYCFLAGS) -o $@ $<
	@sed -e 's,\(^.*\.o\),$@ ,' $(DEP_P) >$(DEP_DD); rm -f $(DEP_P); \
	echo [DEP] $(DEP_DD);
endef	

endif

define EXE_template
EXE += $(addprefix $(OBJDIR)/,$(1))
$(addprefix $(OBJDIR)/,$(1)) : $(addsuffix .o,$(addprefix $(OBJDIR)/,$(2))) \
			                   $(if $(3),$(foreach deplib,$(3),$(call lib_name,$(deplib)))) 
	$(CXX) $(LDFLAGS) $$($(1)_LDFLAGS) -o $$@ $$(call dep_strip, $$^) $$($(1)_LDLIBS) $(LDLIBS)
endef

define INST_EXE_template
EXE += $(addprefix $(OBJDIR)/,$(1))
$(addprefix $(OBJDIR)/,$(1)) : $(addsuffix .o,$(addprefix $(OBJDIR)/,$(2))) \
			                   $(if $(3),$(foreach deplib,$(3),$(call lib_name,$(deplib)))) 
	$(CXX) $(LDFLAGS) $$($(1)_LDFLAGS) -o $$@ $$(call dep_strip, $$^) $$($(1)_LDLIBS) $(LDLIBS)

install :: $(addprefix $(OBJDIR)/,$(1))
	@test -d $(DESTDIR)/$(INST_BINDIR) || mkdir -p $(DESTDIR)/$(INST_BINDIR)
	install -m 0755 $$< $(DESTDIR)/$(INST_BINDIR)/


endef

define SO_template
SO += $(addsuffix .so,$(addprefix $(OBJDIR)/,$(1)))
$(addsuffix .so,$(addprefix $(OBJDIR)/,$(1))) : $(addsuffix .do,$(addprefix $(OBJDIR)/,$(2))) \
			                   $(if $(3),$(foreach deplib,$(3),$(call dlib_name,$(deplib)))) 
	$(CXX) $(SH_LDFLAGS) $(LDFLAGS) $$($(1)_LDFLAGS) -o $$@  $$(call dep_strip, $$^) $$($(1)_LDLIBS) $(LDLIBS)
endef

define INST_SO_template
SO += $(addsuffix .so,$(addprefix $(OBJDIR)/,$(1)))
$(addsuffix .so,$(addprefix $(OBJDIR)/,$(1))) : $(addsuffix .do,$(addprefix $(OBJDIR)/,$(2))) \
			                   $(if $(3),$(foreach deplib,$(3),$(call dlib_name,$(deplib)))) 
	$(CXX) $(SH_LDFLAGS) $(LDFLAGS) $$($(1)_LDFLAGS) -o $$@  $$(call dep_strip, $$^) $$($(1)_LDLIBS) $(LDLIBS)


install :: $(addsuffix .so,$(addprefix $(OBJDIR)/,$(1))) 
	@test -d $(DESTDIR)/$(INST_LIBDIR) || mkdir -p $(DESTDIR)/$(INST_LIBDIR)
	install -m 0755 $$< $(DESTDIR)/$(INST_LIBDIR)/
endef


define LIB_template
LIB += $(OBJDIR)/lib$(1).a
LIB += $(OBJDIR)/lib$(1)_d.a
$(OBJDIR)/lib$(1).a : $(addsuffix .o,$(addprefix $(OBJDIR)/,$(2)))
	rm -f $$@; $(AR) rcs $$@ $$^
$(OBJDIR)/lib$(1)_d.a : $(addsuffix .do,$(addprefix $(OBJDIR)/,$(2)))
	rm -f $$@; $(AR) rcs $$@ $$^
endef

define INST_LIB_template
LIB += $(OBJDIR)/lib$(1).a
LIB += $(OBJDIR)/lib$(1)_d.a
$(OBJDIR)/lib$(1).a : $(addsuffix .o,$(addprefix $(OBJDIR)/,$(2)))
	rm -f $$@; $(AR) rcs $$@ $$^
$(OBJDIR)/lib$(1)_d.a : $(addsuffix .do,$(addprefix $(OBJDIR)/,$(2)))
	rm -f $$@; $(AR) rcs $$@ $$^

install :: $(OBJDIR)/lib$(1).a
	@test -d $(DESTDIR)/$(INST_LIBDIR) || mkdir -p $(DESTDIR)/$(INST_LIBDIR)
	install -m 0755 $$< $(DESTDIR)/$(INST_LIBDIR)/
endef



# include your rule.mk here
include Make.inc

cpp_flag = $(filter -I% -D%, $(1))
MYCFLAGS = $(sort $($(basename $(notdir $@))_CFLAGS) $(CFLAGS))
MYCPPFLAGS = $(call cpp_flag, $(MYCFLAGS))


$(OBJDIR)/%.do : %.c
	$(c_dobj)

$(OBJDIR)/%.do : %.cpp
	$(cxx_dobj)
$(OBJDIR)/%.do : %.cc
	$(cxx_dobj)
$(OBJDIR)/%.do : %.cxx
	$(cxx_dobj)

$(OBJDIR)/%.o : %.c
	$(c_obj)

$(OBJDIR)/%.o : %.cpp
	$(cxx_obj)	
$(OBJDIR)/%.o : %.cc
	$(cxx_obj)	
$(OBJDIR)/%.o : %.cxx
	$(cxx_obj)	

SRCS := $(sort $(SRCS))


.PHONY : dep

dep += $(addprefix $(DEPDIR)/,$(notdir $(addsuffix .d,$(basename $(SRCS))))) \
		$(addprefix $(DEPDIR)/,$(notdir $(addsuffix .dd,$(basename $(SRCS))))) \
        $(addprefix $(DEPDIR)/,$(notdir $(addsuffix .d,$(basename $(filter %.c %.cpp %.cc %.cxx,$(GEN_FILES)))))) \
        $(addprefix $(DEPDIR)/,$(notdir $(addsuffix .dd,$(basename $(filter %.c %.cpp %.cc %.cxx,$(GEN_FILES))))))

-include $(dep)

.PHONY : all clean dbg help  install info

all :: $(GEN_FILES) $(LIB) $(SO) $(EXE) $(BIN)


info ::
	@echo "EXE: $(EXE)"
	@echo "SO: $(SO)"
	@echo "LIB: $(LIB)"
	@echo "DEP: $(dep)"
	@echo "SRCS: $(SRCS)"

clean :
	rm -rf $(DEPDIR)/*; 
	find $(OBJDIR)/ -maxdepth 1 -type f | xargs rm -f; 
	rm -f $(GEN_FILES); 
	rm -f config.log

help ::
	@echo "gmake                   build the whole project"
	@echo "gmake  dbg              enable compiler&linker's debug feature"
	@echo "gmake  clean            clean the project"
	@echo "gmake  install          install files"
	@echo "gmake  info             echo EXE LIB SO

