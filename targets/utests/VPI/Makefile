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

MODULE := VPI_utest
TEST_MODULE :=  VPI
DEPENDMODULES := AIM OS BigList
LDFLAGS += -lpthread
PEDANTIC := 1

GLOBAL_CFLAGS += -DOS_CONFIG_INCLUDE_POSIX=1
GLOBAL_CFLAGS += -DAIM_CONFIG_INCLUDE_POSIX=1
GLOBAL_LINK_LIBS += -lpcap

# Tests must be run under sudo for veth interfaces
SUDO:=sudo

include $(BUILDER)/build-unit-test.mk






