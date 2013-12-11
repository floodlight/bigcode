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

GCC_NO_WERROR := 1

MODULE := pyvpim
include $(BUILDER)/standardinit.mk

DEPENDMODULES := AIM BigList
include $(BUILDER)/dependmodules.mk

# Build a library based on the VPI python bindings.
LIBRARY := pyvpi
$(LIBRARY)_SUBDIR := $(VPI_BASEDIR)/module/src
$(LIBRARY)_SRCS := $($(LIBRARY)_SUBDIR)/vpi_python.c
include $(BUILDER)/lib.mk

# Build a shared library with the python bindings library and
# link against libvpi
SHAREDLIB := pyvpi.so
$(SHAREDLIB)_TARGETS := $(ALL_TARGETS)
GLOBAL_LINK_LIBS := -lvpi
include $(BUILDER)/so.mk

GLOBAL_CFLAGS += -fPIC -DVPI_CONFIG_INCLUDE_PYTHON_BINDINGS=1
GLOBAL_CFLAGS += -I$(VPI_BASEDIR)/module/inc
GLOBAL_CFLAGS += $(shell pkg-config --cflags python)
LDFLAGS += -lpthread

include $(BUILDER)/targets.mk

.DEFAULT_GOAL := deb

sdist: $(SHAREDLIB)
	rm -rf dist
	rm -rf vpi*.tar.gz
	python setup.py sdist
	mv dist/vpi*.tar.gz .
	rm -rf dist

deb: sdist
	rm -rf deb_dist
	rm -rf *.deb
	py2dsc -m "Support <support@bigswitch.com>" vpi*.tar.gz
	sed -i "s/python:Depends}/python:Depends}, libvpi/" deb_dist/vpi-*/debian/control
	cd deb_dist/vpi-* && debuild -i -us -uc -b
	mv deb_dist/python-vpi*.deb .
	rm -rf deb_dist
