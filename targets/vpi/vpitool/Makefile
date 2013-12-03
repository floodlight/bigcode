#################################################################
#
#        Copyright 2013, Big Switch Networks, Inc.
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
#################################################################
include ../../../init.mk

.DEFAULT_GOAL := vpitool

MODULE := vpitool
include $(BUILDER)/standardinit.mk

LIBRARY := libvpitool
$(LIBRARY)_SUBDIR := $(dir $(lastword $(MAKEFILE_LIST)))
include $(BUILDER)/lib.mk

DEPENDMODULES := AIM VPI BigList uCli IOF OS PPE

include $(BUILDER)/dependmodules.mk

BINARY := vpitool
$(BINARY)_LIBRARIES := $(LIBRARY_TARGETS)
include $(BUILDER)/bin.mk

include $(BUILDER)/targets.mk

LDFLAGS += -lpthread

# For platforms that don't support libpcap
ifdef DISABLE_PCAP
GLOBAL_CFLAGS += -DVPI_CONFIG_INCLUDE_INTERFACE_PCAP=0
GLOBAL_CFLAGS += -DVPI_CONFIG_INCLUDE_INTERFACE_PCAPDUMP=0
else
GLOBAL_LINK_LIBS += -lpcap
endif

GLOBAL_CFLAGS += -DVPI_CONFIG_INCLUDE_UCLI=1
GLOBAL_CFLAGS += -DAIM_CONFIG_INCLUDE_MAIN=1
GLOBAL_CFLAGS += -DAIM_CONFIG_INCLUDE_MODULES_INIT=1
GLOBAL_CFLAGS += -DOS_CONFIG_INCLUDE_POSIX=1

GLOBAL_LINK_LIBS += -lrt

ucli:
	../../tools/uclihandlers.py main.c

