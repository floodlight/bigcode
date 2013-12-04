# -*- Makefile -*-
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
# Static rules for SWIG generation
#
################################################################

all: swig

PACKAGE=com.bigswitch.vpi.native_.swig
OUTDIR=com/bigswitch/vpi/native_/swig
swigclean:
	@rm -f VPI_wrap.c
	@rm -f $(OUTDIR)/*.java
	@rm -f $(OUTDIR)/*.class

swig: swigclean
	swig -java -package $(PACKAGE) -outdir $(OUTDIR) VPI.i

