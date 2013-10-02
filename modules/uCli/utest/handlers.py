#!/usr/bin/python

print "/* -*- C -*- */"

handlers = { 'first' :  [ 'alpha', 'bravo', 'charlie' ], 
             'second':  [ 'delta','echo', 'foxtrot', 'golf' ], 
             'third' : [ 'hotel', 'india' ], 
             'fourth' : [ 'juliet' ], 
             'fifth' : [ 'kilo', 'lima', 'mike' ], 
             }

order = ['first', 'second', 'third', 'fourth', 'fifth' ]

for m in order:
    for e in handlers[m]:
        print """static ucli_status_t utest_ucli_%s_%s(ucli_context_t* uc)""" % (m, e)
        print """{"""
        print """    UCLI_COMMAND_INFO(uc, "%(e)s", 0, "%(e)s"); """ % dict(e=e)
        print """    return __echo__("%s::%s", uc); """ % (m, e)
        print """}"""

print "/* <auto.ucli.handlers.start> */"
print "/* <auto.ucli.handlers.end> */"

for m in order:
    print """ucli_module_t %s_module = { "%s", NULL, utest_ucli_%s_handlers__ };""" % (m, m, m)

print
print
print """ucli_module_t* modules[] = """
print """{"""
for m in order:
    print """    &%s_module, """ % m
print """    NULL"""
print """};"""






           
