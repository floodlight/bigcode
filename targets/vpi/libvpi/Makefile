################################################################
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
################################################################
include ../../../init.mk

#
# Python2.6 headers have a POSIX redefined warning
#
GCC_NO_WERROR := 1

MODULE := libvpim
include $(BUILDER)/standardinit.mk

DEPENDMODULES := AIM VPI BigList OS
include $(BUILDER)/dependmodules.mk

SHAREDLIB := libvpi.so

# Build a shared lib from all object files
$(SHAREDLIB)_TARGETS := $(ALL_TARGETS)
include $(BUILDER)/so.mk

include $(BUILDER)/targets.mk

LDFLAGS += -lpthread -lrt

# TODO use pkg-config
GLOBAL_CFLAGS += -fPIC
GLOBAL_LINK_LIBS += -lpcap -lrt

.DEFAULT_GOAL := all

all: libraries sharedlibs

.PHONY: deb

deb:
	cd deb && debuild $(DEBUILD_ARGS) $(ARCH_OPTIONS) -b -us -uc
