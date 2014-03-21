###############################################################################
#
# debug_counter Unit Test Makefile.
#
###############################################################################
UMODULE := debug_counter
UMODULE_SUBDIR := $(dir $(lastword $(MAKEFILE_LIST)))
include $(BUILDER)/utest.mk
