############################################################
# <bsn.cl fy=2013 v=epl>
# </bsn.cl>
############################################################
#
#
#
############################################################

include ../../../init.mk
MODULE := orc_utest
TEST_MODULE := orc
DEPENDMODULES := AIM ELS OS uCli BigList IOF
GLOBAL_CFLAGS += -DAIM_CONFIG_INCLUDE_MODULES_INIT=1
GLOBAL_CFLAGS += -DAIM_CONFIG_INCLUDE_MAIN=1
GLOBAL_LINK_LIBS += -ldl -rdynamic -lpthread   # pthreads is needed to unbork gdb

include $(BUILDER)/build-unit-test.mk
