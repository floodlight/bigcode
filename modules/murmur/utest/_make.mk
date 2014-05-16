###############################################################################
#
# murmur Unit Test Makefile.
#
###############################################################################
UMODULE := murmur
UMODULE_SUBDIR := $(dir $(lastword $(MAKEFILE_LIST)))
include $(BUILDER)/utest.mk
