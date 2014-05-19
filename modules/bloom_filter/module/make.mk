###############################################################################
#
# 
#
###############################################################################
THIS_DIR := $(dir $(lastword $(MAKEFILE_LIST)))
bloom_filter_INCLUDES := -I $(THIS_DIR)inc
bloom_filter_INTERNAL_INCLUDES := -I $(THIS_DIR)src
