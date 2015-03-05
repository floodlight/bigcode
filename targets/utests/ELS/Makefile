############################################################
# <bsn.cl fy=2014 v=epl-1.0>
# </bsn.cl>
############################################################
#
#  ELS Unit Testing Module Makefile
#
############################################################

include ../../../init.mk

MODULE := ELS_utest
TEST_MODULE :=  ELS
DEPENDMODULES := AIM OS

GLOBAL_LINK_LIBS += -lpthread -ledit
PEDANTIC := 1
GLOBAL_CFLAGS += -DAIM_CONFIG_INCLUDE_POSIX=1

include $(BUILDER)/build-unit-test.mk

