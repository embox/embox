#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Monitor check md5 sum
# date: 26.06.09
# author: sikmir

import sys, string, os, traceback, re, getopt, zlib, hashlib, math
from misc import *
from subprocess import Popen

class Checksum:
	def __init__(self):
		self.objcopy, self.bin_dir, self.target = (None, None, None)
		self.checksum = 0
		self.build = None

	def file_md5(self, file, use_system = False):
		if isinstance(file, basestring):
			if use_system:
				sysname = os.uname()[0]
		    		if sysname in ('Linux', 'linux'):
            				po = Popen('md5sum -b "%s"' % file, shell=True, stdout=-1, stderr=-1)
                            		po.wait()
                            		m = po.stdout.readline().strip()
                            		if len(m) == 32:
                                    		return m
			file = open(file, 'rb')
    		h = hashlib.md5()
    		block = file.read(h.block_size)
    		while block:
    			h.update(block)
    			block = file.read(h.block_size)
		file.close()
		return h.hexdigest()

	def build_md5(self):
		ObjFileName = str(self.bin_dir) + '/' + self.build + '.objcopy'
		Md5FileName = str(self.bin_dir) + '/' + str(self.target) + "_" + self.build + '.md5'
    		self.checksum = self.file_md5(ObjFileName, use_system = True)
        	write_file(Md5FileName, self.checksum)

	def rebuild_autoconf_h(self, iszero):
		content = read_file('scripts/autoconf.h')
		mdef = '#define MD5_CHECKSUM "(\w+)"'
		if iszero == True:
			content = re.sub(mdef, '#define MD5_CHECKSUM "0"', content)
		else:
			content = re.sub(mdef, '#define MD5_CHECKSUM "%s"' % self.checksum, content)
		write_file('scripts/autoconf.h', content)

	def main(self):
		os.system(str(self.objcopy) + " -j .data -j .text " + str(self.bin_dir) + "/" + str(self.target) + "_" + \
				    self.build + " " + str(self.bin_dir) + "/" + self.build + ".objcopy")
		self.build_md5()
		self.rebuild_autoconf_h(False)
		os.remove(str(self.bin_dir) + '/' + self.build + '.objcopy')

if __name__=='__main__':
        try:
                opts, args = getopt.getopt(sys.argv[1:], "ho:d:t:b:c", ["help", "objcopy=", "bin_dir=", \
            						 "taget=", "build=", "clean"])
            	obj = Checksum()
            	run = "main"
                for o, a in opts:
                        if o in ("-h", "--help"):
                                run = "help"
                        elif o in ("-c", "--clean"):
                    		run = "clean"
                    	elif o in ("-o", "--objcopy"):
                        	obj.objcopy = a
                    	elif o in ("-d", "--bin_dir"):
                    		obj.bin_dir = a
                    	elif o in ("-t", "--target"):
                    		obj.target = a
                    	elif o in ("-b", "--build"):
                    		if a == "debug":
                    			obj.build = "ram"
                    		elif a == "release":
                    			obj.build = "rom"
                    	else:
				assert False, "unhandled option"
		if run == "clean":
			obj.rebuild_autoconf_h(True)
		elif run == "main":
			obj.main()
		elif run == "help":
			print "Usage: checksum.py [-o <objcopy>] [-d <bin_dir>] [-t <target>] [-hc] [-b <build>]\n"
	except:
	        traceback.print_exc()
