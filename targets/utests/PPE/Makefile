###############################################################################
#
#  PPE Unit Testing Module Makefile
#
#  
#
###############################################################################
include ../../../init.mk

DEPENDMODULES := AIM uCli BigList IOF

MODULE := PPE_utest
TEST_MODULE :=  PPE

GLOBAL_CFLAGS += -DPPE_CONFIG_INCLUDE_UTM=1 -DPPE_CONFIG_INCLUDE_UCLI=1
GLOBAL_CFLAGS += -DUCLI_CONFIG_INCLUDE_FGETS_LOOP=1
GLOBAL_CFLAGS += -DAIM_CONFIG_INCLUDE_MAIN=1
GLOBAL_CFLAGS += -DAIM_CONFIG_INCLUDE_MODULES_INIT=1

PEDANTIC := 1

include $(BUILDER)/build-unit-test.mk

