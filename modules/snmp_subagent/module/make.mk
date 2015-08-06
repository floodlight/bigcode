###############################################################################
#
#
#
###############################################################################
THIS_DIR := $(dir $(lastword $(MAKEFILE_LIST)))
snmp_subagent_INCLUDES := -I $(THIS_DIR)inc
snmp_subagent_INTERNAL_INCLUDES := -I $(THIS_DIR)src
snmp_subagent_DEPENDMODULE_ENTRIES := init:snmp_subagent ucli:snmp_subagent
snmp_subagent_GLOBAL_LINK_LIBS += -lnetsnmpagent -lnetsnmphelpers -lnetsnmpmibs -lnetsnmp


