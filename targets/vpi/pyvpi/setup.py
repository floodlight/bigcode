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


from distutils.core import setup
setup(name='vpi',
      version='1.0.0',
      description='Python VPI Binding',
      author='Support',
      author_email='support@bigswitch.com',
      packages=['vpi'],
      data_files=[('/usr/bin', ['vpi/pyvpitool']), ('/usr/lib', ['build/gcc-local/bin/pyvpi.so'])]
      )
