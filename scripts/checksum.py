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
		self.checksum = {"0": 0}
		self.builds = []

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
    		for item in self.builds:
    			self.checksum[item] = self.file_md5(str(self.bin_dir) + '/' + item + '.objcopy', use_system = True)
            		write_file(str(self.bin_dir) + '/' + str(self.target) + "_" + item + '.md5', self.checksum[item])

	def rebuild_linker(self, iszero):
		for item in self.builds:
			content = read_file('scripts/autoconf.h')
			if iszero == True:
				content = re.sub('#define MD5_CHECKSUM "(\w+)"', '#define MD5_CHECKSUM "0"', content)
			else:
				content = re.sub('#define MD5_CHECKSUM "(\w+)"', '#define MD5_CHECKSUM "%s"' % self.checksum["ram"], content)
			write_file('scripts/autoconf.h', content)

	def main(self):
		for item in self.builds:
			os.system(str(self.objcopy) + " -j .data -j .text " + str(self.bin_dir) + "/" + str(self.target) + "_" + \
					    item + " " + str(self.bin_dir) + "/" + item + ".objcopy")
		self.build_md5()
		self.rebuild_linker(False)
		for item in self.builds:
			os.remove(str(self.bin_dir) + '/' + item + '.objcopy')

if __name__=='__main__':
        try:
                opts, args = getopt.getopt(sys.argv[1:], "ho:d:t:srbac", ["help", "objcopy=", "bin_dir=", \
            						 "taget=", "sim", "release", "debug", "clean"])
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
                    	elif o in ("-s", "--sim"):
                    		obj.builds.append("sim")
                    	elif o in ("-r", "--release"):
                    		obj.builds.append("rom")
                    	elif o in ("-b", "--debug"):
                    		obj.builds.append("ram")
                    	else:
				assert False, "unhandled option"
		if run == "clean":
			pass
			obj.rebuild_linker(True)
		elif run == "main":
			obj.main()
		elif run == "help":
			print "Usage: checksum.py [-o <objdump>] [-d <bin_dir>] [-t <target>] [-h]\n"
	except:
	        traceback.print_exc()
