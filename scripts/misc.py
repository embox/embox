#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Monitor misc
# date: 26.06.09
# author: sikmir

import re

def write_file(name, content):
	with open(name, 'w+') as file:
    		file.write(content)
        file.close()

def read_file(name):
        with open(name, 'r+') as file:
                content = file.read()
        file.close()
	return content

def replacer(mdef, inc, content):
	mask = mdef + '(\s*)=(\s*)(\w*)'
        if inc == True:
    		content = re.sub(mask, mdef + "=y", content)
        else:
                content = re.sub(mask, mdef + "=n", content)
        return content

def replacer_h(mdef, inc, content):
	mask = '#([undefi]{5,6})([ \t]+)' + mdef + '([ \t]*)[1]?'
        if inc == True:
                content = re.sub(mask, "#define " + mdef + " 1", content)
        else:
        	content = re.sub(mask, "#undef " + mdef, content)
        return content

def onPress(ar, item, j):
        ar[item][j] = not ar[item][j]

def getStatus(i):
        if i == 1:
    		return "normal"
        if i == 0:
                return "disabled"
