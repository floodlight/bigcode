###############################################################################
#
# 
#
###############################################################################
THIS_DIR := $(dir $(lastword $(MAKEFILE_LIST)))
debug_counter_INCLUDES := -I $(THIS_DIR)inc
debug_counter_INTERNAL_INCLUDES := -I $(THIS_DIR)src
debug_counter_DEPENDMODULE_ENTRIES := init:debug_counter ucli:debug_counter

