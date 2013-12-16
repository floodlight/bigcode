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
#
#  jvpi
#
################################################################
include ../../../init.mk

GCC_NO_WERROR := 1

MODULE := jvpim
include $(BUILDER)/standardinit.mk

LIBRARY := jvpi
$(LIBRARY)_SUBDIR := $(dir $(lastword $(MAKEFILE_LIST)))
$(LIBRARY)_SRCS := $($(LIBRARY)_SUBDIR)/VPI_wrap.c

# swig must run an a order-prerequisite to generate VPI_wrap.c
VPI_wrap.c : | swig

include $(BUILDER)/lib.mk

DEPENDMODULES := AIM BigList OS
include $(BUILDER)/dependmodules.mk

SHAREDLIB := libjvpi.so

$(SHAREDLIB)_TARGETS := $(ALL_TARGETS)
include $(BUILDER)/so.mk

include $(BUILDER)/targets.mk

LDFLAGS += -lpthread
GLOBAL_LINK_LIBS := -lvpi

GLOBAL_INCLUDES += -I /usr/lib/jvm/java-6-openjdk/include -I /usr/lib/jvm/default-java/include
GLOBAL_CFLAGS += -fPIC
GLOBAL_CFLAGS += -I$(VPI_BASEDIR)/module/inc

.DEFAULT_GOAL := test

include Makefile.swig

java: | swig
	javac -sourcepath . $(OUTDIR)/*.java
	javac -sourcepath . $(OUTDIR)/../*.java
	javac -sourcepath . *.java

export LD_LIBRARY_PATH := $(BINARY_DIR):/usr/lib
test: sharedlibs java
	java TestJVPI

tests: test
