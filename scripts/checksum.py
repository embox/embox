#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Monitor check md5 sum
# date: 26.06.09
# author: sikmir

import sys, string, os, traceback, re, getopt, zlib, hashlib

objdump, bin_dir, target = (None, None, None)

def file_md5(file, use_system = False):
	if isinstance(file, basestring):
		if use_system:
			sysname = os.uname()[0]
		        if sysname in ('Linux', 'linux'):
				os.system("md5sum " + str(file))
				# TODO:
		file = open(file, 'rb')
        h = hashlib.md5()
        block = file.read(h.block_size)
        while block:
    		h.update(block)
    		block = file.read(h.block_size)
	file.close()
	return h.hexdigest()

def main():
	global objdump, bin_dir, target
	os.system(str(objdump) + " -s " + str(bin_dir) + "/" + str(target) + "_ram > " + str(bin_dir) + "/ram.objdump")
	os.system(str(objdump) + " -s " + str(bin_dir) + "/" + str(target) + "_rom > " + str(bin_dir) + "/rom.objdump")
	for file in [ "ram", "rom" ]:
		content = ""
		flag = 0
	        with open(str(bin_dir) + '/' + file + '.objdump', 'r+') as fobj:
			for line in fobj.readlines():
				if re.search("Contents of section", line):
					flag = 0
				if re.search(".text", line) or re.search(".data", line):
				        flag = 1
				if flag == 1:
					content += line
	        fobj.close()
#	        with open(str(bin_dir) + '/' + file + '.objdump', 'w+') as fobj:
#                        fobj.write(content)
#                fobj.close()
#                with open(str(bin_dir) + '/' + file + '.crc32', 'w+') as fcrc32:
#            		a = zlib.crc32(content)
#            		fcrc32.write(str(a))
#            	fcrc32.close()
#		with open(str(bin_dir) + '/' + file + '.md5', 'w+') as fmd5:
#                        fmd5.write(file_md5(str(bin_dir) + '/' + file + '.objdump'))
#                fmd5.close()

if __name__=='__main__':
        try:
                opts, args = getopt.getopt(sys.argv[1:], "ho:d:t:", ["help", "objdump=", "bin_dir=", "taget="])
                for o, a in opts:
                        if o in ("-h", "--help"):
                                print "Usage: checksum.py [-o <objdump>] [-d <bin_dir>] [-t <target>] [-h]\n"
                                sys.exit()
                        elif o in ("-o", "--objdump"):
                                objdump = a
                        elif o in ("-d", "--bin_dir"):
                    		bin_dir = a
                    	elif o in ("-t", "--taget"):
                    		target = a
                        else:
			        assert False, "unhandled option"
		main()
	except:
	        traceback.print_exc()
