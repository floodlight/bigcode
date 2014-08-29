###############################################################################
#
# 
#
###############################################################################
THIS_DIR := $(dir $(lastword $(MAKEFILE_LIST)))
slot_allocator_INCLUDES := -I $(THIS_DIR)inc
slot_allocator_INTERNAL_INCLUDES := -I $(THIS_DIR)src
slot_allocator_DEPENDMODULE_ENTRIES := init:slot_allocator ucli:slot_allocator

