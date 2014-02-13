###############################################################################
#
# 
#
###############################################################################
THIS_DIR := $(dir $(lastword $(MAKEFILE_LIST)))
timer_wheel_INCLUDES := -I $(THIS_DIR)inc
timer_wheel_INTERNAL_INCLUDES := -I $(THIS_DIR)src
timer_wheel_DEPENDMODULE_ENTRIES := init:timer_wheel ucli:timer_wheel

