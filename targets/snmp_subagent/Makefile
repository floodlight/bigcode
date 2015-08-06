############################################################
# <bsn.cl fy=2014 v=onl>
#
#        Copyright 2014, 2015 Big Switch Networks, Inc.
#
# Licensed under the Eclipse Public License, Version 1.0 (the
# "License"); you may not use this file except in compliance
# with the License. You may obtain a copy of the License at
#
#        http://www.eclipse.org/legal/epl-v10.html
#
# Unless required by applicable law or agreed to in writing,
# software distributed under the License is distributed on an
# "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
# either express or implied. See the License for the specific
# language governing permissions and limitations under the
# License.
#
# </bsn.cl>
############################################################
#
#
#
############################################################
include ../../init.mk

.DEFAULT_GOAL := snmp-subagent
DEBUG := 1

MODULE := snmp-subagent
include $(BUILDER)/standardinit.mk

DEPENDMODULES := snmp_subagent AIM OS

include $(BUILDER)/dependmodules.mk

BINARY := snmp-subagent
$(BINARY)_LIBRARIES := $(LIBRARY_TARGETS)
include $(BUILDER)/bin.mk

GLOBAL_CFLAGS += -DAIM_CONFIG_INCLUDE_MODULES_INIT=1
GLOBAL_CFLAGS += -DAIM_CONFIG_INCLUDE_MAIN=1
GLOBAL_CFLAGS += -DAIM_CONFIG_INCLUDE_DAEMONIZE=1
GLOBAL_CFLAGS += -DSNMP_SUBAGENT_CONFIG_INCLUDE_MAIN=1
GLOBAL_CFLAGS += -DAIM_CONFIG_AIM_MAIN_FUNCTION=snmp_subagent_main
GLOBAL_CFLAGS += -DSNMP_SUBAGENT_CONFIG_INCLUDE_EXAMPLE_CLIENT=1
GLOBAL_CFLAGS += -DAIM_CONFIG_INCLUDE_PVS_SYSLOG=1

GLOBAL_LINK_LIBS += -lpthread

include $(BUILDER)/targets.mk

