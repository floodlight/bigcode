#!/usr/bin/env python
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
#
# This is an enhanced version of the ucli autogenerator.
#
# This script will generate all module definitions and intermediate
# node trees based solely on the handler names.
#
# This was always what the original generator was supposed to do,
# but I was lazy.
#
# The command hierarchy is determined via handler names.
# Node names are delimited by double underscores.
#
# For example, the following declarations:
#
# ucli_status_t <module>__earth__na__us__ca__sf__(ucli_status_t* uc);
# ucli_status_t <module>__earth__na__us__ca__la__(ucli_status_t* uc);
# ucli_status_t <module>__earth__na__us__ca__sd__(ucli_status_t* uc);
# ucli_status_t <module>__earth__na__us__nv__reno__(ucli_status_t* us);
# ucli_status_t <module>__earth__na__mexico__qr__cancun__(ucli_status_t* uc);
# ucli_status_t <module>__earth__na__mexico__bc__tj__(ucli_status_t* uc);
# ucli_status_t <module>__earth__sa__brazil__rio__(ucli_status_t* uc);
# ucli_status_t <module>__earth__sa__brazil__sp__(ucli_status_t* uc);
# ucli_status_t <module>__moon__seas__tranquility__(ucli_status_t* uc);
# ucli_status_t <module>__moon__seas__serenity__(ucli_status_t* uc)
#
# And so on and so forth would generate the following CLI tree:
#
# ROOT:
#   earth:
#     na:
#       us:
#         ca:
#           [ sf, la, sd ]
#         nv:
#           [ reno ]
#     mexico:
#       qr:
#         [ cancun ]
#       bc:
#         [ tj ]
#     sa:
#       brazil:
#         [ rio, sp ]
#   moon:
#     seas:
#       [ tranquility, serenity ]
#
#
# This is still a work in progress.
#     TODO:
#         1. Allow handlers to be scraped from multiple files.
#         2. Fix embarassingness-ness of it.
################################################################
import os
import sys
import re
import pprint
import infra
import wod

class UcliGenerator(object):
    def __init__(self):
        self.nodes = {}

    def add_file(self, fname):
        with open(fname, "r") as f:
            source = f.read()
            matches = re.findall("(?P<condition>#if.*?\n)?(?:\s*static\s*)?ucli_status_t\s+(?P<function>[_a-zA-Z]\w*)\(\s*ucli_context_t\s*\*\s*[_a-zA-Z]\w*\s*\)", source);
            for m in matches:
                self.add_symbol(m[1])

    def add_symbol(self, symbol):
        fields = []
        for f in symbol.split('__'):
            if f:
                fields.append(f)

        # This is the command name
        command = fields.pop()

        #
        # This is the name of the module handler table that should
        # be declared for all commands in that module.
        #
        # This symbol needs to remain a node as well (which will
        # contain the module) so its not popped from the fields.
        #
        module = fields[-1]

        n = self.nodes
        for node in fields:
            if node not in n:
                n[node] = {}
            n = n[node]

        if module not in n:
            n[module] = []
        n[module].append(symbol)

    #
    # Emit variable and structure definitions.
    #
    def __emit_definitions(self, key, value, parent):
        s = ""
        if type(value) is list:
            # key is the module name, list are the handlers.
            name = "%s__%s" % (parent, key)

            # Handler table
            s += 'static ucli_command_handler_f %s__handlers__[] = \n' % (name)
            s += '{\n'
            for handler in value:
                s += '    %s,\n' % (handler)
            s += '    NULL\n'
            s += '};\n'

            # Module
            s += 'static ucli_module_t %s__module__ = \n' % (name)
            s += '{\n'
            s += '    "%s",\n' % key
            s += '    NULL,\n'
            s += '    %s__handlers__,\n' % (name)
            s += '    NULL,\n'
            s += '    NULL\n'
            s += '};\n'

        elif type(value) is dict:
            # key is a node name
            if parent is None:
                name = key
            else:
                name = "%s__%s" % (parent, key)

            s += 'ucli_node_t* %s__node__ = NULL;\n' % (name)
            for(k, v) in value.iteritems():
                s += self.__emit_definitions(k, v, name)
        else:
            raise Exception(type(value))
        return s



    def __emit_inits(self, key, value, parent):
        s = ""
        if type(value) is list:
            # key is module name, list are the handlers
            s += "    ucli_module_init(&%s__%s__module__);\n" % (parent, key)
        elif type(value) is dict:
            if parent is None:
                name = key
            else:
                name = "%s__%s" % (parent, key)
            s += '    if(%s__node__ == NULL) %s__node__ = ucli_node_create("%s", NULL, NULL);\n' % (name, name, key.replace("_ucli", ""))
            for(k, v) in value.iteritems():
                s += self.__emit_inits(k, v, name)

        return s

    def __emit_tree(self, key, value, parent):
        s = ""
        if type(value) is list:
            # Add module to parent node
            s += "    ucli_node_module_add(%s__node__, &%s__%s__module__);\n" % (parent, parent, key)
        elif type(value) is dict:
            # Add node to parent node
            if parent is None:
                name = key
            else:
                name = "%s__%s" % (parent, key)
            if parent:
                s += "    ucli_node_subnode_add(%s__node__, %s__%s__node__);\n" % (parent, parent, key)
            for(k, v) in value.iteritems():
                s += self.__emit_tree(k, v, name)
        else:
            raise Exception(type(value))

        return s;

    def generate_str(self):
        s = ""
        for(k, v) in self.nodes.iteritems():
            s += self.__emit_definitions(k, v, None)
        s += "static ucli_node_t* __ucli_auto_init__(void)\n"
        s += "{\n"
        for(k, v) in self.nodes.iteritems():
            s += self.__emit_inits(k, v, None)
        for(k, v) in self.nodes.iteritems():
            s += self.__emit_tree(k, v, None)
        s += "    return %s__node__;\n" % self.nodes.keys()[0]
        s += "}\n"
        return s


    def update(self, src, stdout=False):
        source = open(src).read()
        start = "/* <auto.ucli.handlers.start> */"
        end   = "/* <auto.ucli.handlers.end> */"

        s = start
        s += """
/******************************************************************************
 *
 * These handler table(s) were autogenerated from the symbols in this
 * source file.
 *
 *****************************************************************************/
"""
        s += self.generate_str()
        s += "/******************************************************************************/\n"
        s += end

        # Replace old handler tables
        rstart = re.sub("\*", "\\\*", start)
        rend = re.sub("\*", "\\\*", end)
        expr = rstart + ".*" + rend
        source = re.sub(expr, s, source, flags=re.DOTALL)

        if stdout:
            print source
        else:
            wod.write_on_diff(src, source, msg=False)

if __name__ == '__main__':
    import argparse
    ap = argparse.ArgumentParser("uclihandlers2")
    ap.add_argument("source", help="uCli Source file.")
    ap.add_argument("--stdout", help="Send output to stdout instead of updating the source file.", action='store_true')
    ops = ap.parse_args()
    u = UcliGenerator()
    u.add_file(ops.source)
    u.update(ops.source, ops.stdout)
