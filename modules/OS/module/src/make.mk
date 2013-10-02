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

LIBRARY := OS
OS_SUBDIR := $(dir $(lastword $(MAKEFILE_LIST)))
include $(BUILDER)/lib.mk

ifndef OS_MAKE_CONFIG_AUTOSELECT
OS_MAKE_CONFIG_AUTOSELECT=1
endif

ifeq ($(OS_MAKE_CONFIG_AUTOSELECT),1)
 uname := $(shell uname -s)
 ifeq ($(uname),Linux)
  OS_CFLAGS += -DOS_CONFIG_INCLUDE_POSIX=1
  GLOBAL_LINK_LIBS += -lrt -lpthread
 endif
 ifeq ($(uname),Darwin)
  OS_CFLAGS += -DOS_CONFIG_INCLUDE_OSX=1
 endif
endif
 
