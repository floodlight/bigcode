###############################################################################
#
# bloom_filter Unit Test Makefile.
#
###############################################################################
UMODULE := bloom_filter
UMODULE_SUBDIR := $(dir $(lastword $(MAKEFILE_LIST)))
include $(BUILDER)/utest.mk
