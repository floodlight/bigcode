###############################################################################
#
# slot_allocator Unit Test Makefile.
#
###############################################################################
UMODULE := slot_allocator
UMODULE_SUBDIR := $(dir $(lastword $(MAKEFILE_LIST)))
include $(BUILDER)/utest.mk
