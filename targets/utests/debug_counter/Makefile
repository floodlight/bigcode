###############################################################################
#
#
#
###############################################################################

include ../../../init.mk
MODULE := debug_counter_utest
TEST_MODULE := debug_counter
DEPENDMODULES := AIM
GLOBAL_CFLAGS += -DAIM_CONFIG_INCLUDE_MODULES_INIT=1
GLOBAL_CFLAGS += -DAIM_CONFIG_INCLUDE_MAIN=1
GLOBAL_LINK_LIBS += -lpthread
include $(BUILDER)/build-unit-test.mk
