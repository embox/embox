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
		self.objdump, self.bin_dir, self.target = (None, None, None)
		self.checksum = {"0": 0}
		self.builds = []

	def file_md5(self, file, use_system = False):
		if isinstance(file, basestring):
			if use_system:
				sysname = os.uname()[0]
		    		if sysname in ('Linux', 'linux'):
            				po = Popen('md5sum -t "%s"' % file, shell=True, stdout=-1, stderr=-1)
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

	def build_crc32_sum(self):
    		for item in self.builds:
    			content = ""
            		flag = 0
            		with open(str(self.bin_dir) + '/' + item + '.objdump', 'r+') as fobj:
                    		for line in fobj.readlines():
                            		if re.search("Contents of section", line):
                                    		flag = 0
                            		if re.search(".text", line) or re.search(".data", line):
                                    		flag = 1
                            		if flag == 1:
                                    		content += line
            		fobj.close()
            		write_file(str(self.bin_dir) + '/' + item + '.objdump', content)
			self.checksum[item] = math.fabs(zlib.crc32(content))

	def build_md5(self):
    		for item in self.builds:
            		write_file(str(self.bin_dir) + '/' + str(self.target) + "_" + item + '.md5', \
            				self.file_md5(str(self.bin_dir) + '/' + item + '.objdump'))

	def rebuild_linker(self, iszero):
		for item in self.builds:
			content = read_file("scripts/link" + item)
			if iszero == True:
				content = re.sub('__checksum = (\w+);', "__checksum = 0x00000000;", content)
			else:
				content = re.sub('__checksum = (\w+);', "__checksum = 0x%08X;" % self.checksum[item], content)
			write_file("scripts/link" + item, content)

	def main(self):
		for item in self.builds:
			os.system(str(self.objdump) + " -s " + str(self.bin_dir) + "/" + str(self.target) + "_" + \
					    item + " > " + str(self.bin_dir) + "/" + item + ".objdump")
		self.build_crc32_sum()
		self.build_md5()
		self.rebuild_linker(False)
		for item in self.builds:
			os.remove(str(self.bin_dir) + '/' + item + '.objdump')

if __name__=='__main__':
        try:
                opts, args = getopt.getopt(sys.argv[1:], "ho:d:t:srbc", ["help", "objdump=", "bin_dir=", \
            						 "taget=", "sim", "release", "debug", "clean"])
            	obj = Checksum()
                for o, a in opts:
                        if o in ("-h", "--help"):
                                print "Usage: checksum.py [-o <objdump>] [-d <bin_dir>] [-t <target>] [-h]\n"
                                sys.exit()
                        elif o in ("-o", "--objdump"):
                                obj.objdump = a
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
                    	elif o in ("-c", "--clean"):
                    		obj.rebuild_linker(True)
                    		sys.exit()
                        else:
			        assert False, "unhandled option"
		obj.main()
	except:
	        traceback.print_exc()
