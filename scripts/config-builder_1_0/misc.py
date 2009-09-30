# -*- coding: utf-8 -*-
# Monitor misc
# date: 26.06.09
# author: sikmir

import re, codecs, string

def write_file(name, content):
	with codecs.open(name, 'w+', "utf-8") as file:
    		file.write(content)
        file.close()

def read_file(name):
        with codecs.open(name, 'r+', "utf-8") as file:
                content = file.read()
        file.close()
	return content

def replacer(mdef, inc, content):
	mask = '{0}(\s*)=(\s*)(\w*)'.format(mdef)
        if inc == True:
    		content = re.sub(mask, mdef + "=y", content)
        else:
                content = re.sub(mask, mdef + "=n", content)
        return content

def replacer_h(mdef, inc, content):
	mask = '#([undefi]{5,6})([ \t]+)' + mdef + '([ \t]*)[1]?'
        if inc == True:
                content = re.sub(mask, '#define {0} 1'.format(mdef), content)
        else:
        	content = re.sub(mask, '#undef {0}'.format(mdef), content)
        return content

def replacer_value(mdef, reg, val, content):
	mask = '#define([ \t]+){0}([ \t]*){1}'.format(mdef, reg)
	content = re.sub(mask, '#define {0} {1}'.format(mdef, val), content)
	return content

def onPress(ar, item, j):
        ar[item][j] = not ar[item][j]

def getStatus(i):
        if i == 1:
    		return "normal"
        if i == 0:
                return "disabled"
