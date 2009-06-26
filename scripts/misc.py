#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Monitor misc
# date: 26.06.09
# author: sikmir

def write_file(name, content):
	with open(name, 'w+') as file:
    		file.write(content)
        file.close()

def read_file(name):
        with open(name, 'r+') as file:
                content = file.read()
        file.close()
	return content
