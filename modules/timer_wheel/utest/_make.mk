###############################################################################
#
# timer_wheel Unit Test Makefile.
#
###############################################################################
UMODULE := timer_wheel
UMODULE_SUBDIR := $(dir $(lastword $(MAKEFILE_LIST)))
include $(BUILDER)/utest.mk
