############################################################
# <bsn.cl fy=2013 v=epl>
# 
#        Copyright 2013, 2014 Big Switch Networks, Inc.       
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

THIS_DIR := $(dir $(lastword $(MAKEFILE_LIST)))
orc_INCLUDES := -I $(THIS_DIR)inc
orc_INTERNAL_INCLUDES := -I $(THIS_DIR)src
orc_DEPENDMODULE_ENTRIES := init:orc ucli:orc

OFDPA_HEADERS := $(dir $(shell $(ONL_PKG_INSTALL) ofdpa-2.0-closed:powerpc --find-file ofdpa_api.h))
OFDPA_CLIENT_LIB := $(shell $(ONL_PKG_INSTALL) ofdpa-2.0-closed:powerpc --find-file libofdpa_rpc_client.so.1)

GLOBAL_CFLAGS += -I$(OFDPA_HEADERS)
GLOBAL_CFLAGS += -g

GLOBAL_LINK_LIBS += -lpthread -lm
GLOBAL_LINK_LIBS += $(OFDPA_CLIENT_LIB)
