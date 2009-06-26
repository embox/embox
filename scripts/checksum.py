#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Monitor check md5 sum
# date: 26.06.09
# author: sikmir

import sys, string, os, traceback, re, getopt, zlib, hashlib, math

objdump, bin_dir, target = (None, None, None)
checksum = {"0": 0}

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

def build_crc32_sum():
	global bin_dir, checksum
        for file in [ "ram", "rom", "sim" ]:
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
                with open(str(bin_dir) + '/' + file + '.objdump', 'w+') as fobj:
                        fobj.write(content)
                fobj.close()
		checksum[file] = math.fabs(zlib.crc32(content))

def build_md5():
	global bin_dir
        for file in [ "ram", "rom", "sim" ]:
                with open(str(bin_dir) + '/' + file + '.md5', 'w+') as fmd5:
                        fmd5.write(file_md5(str(bin_dir) + '/' + file + '.objdump'))
                fmd5.close()

def rebuild_linker():
	global checksum
	for file in ("ram", "rom", "sim"):
		with open("scripts/link" + file, 'r+') as flink:
		        content = flink.read()
		flink.close()
		content = re.sub('__checksum = (\w+);', "__checksum = 0x%08X;" % checksum[file], content)
		with open("scripts/link" + file, 'w+') as flink:
		        flink.write(content)
		flink.close()

def main():
	global objdump, bin_dir, target
	#TODO:
	os.system(str(objdump) + " -s " + str(bin_dir) + "/" + str(target) + "_ram > " + str(bin_dir) + "/ram.objdump")
	os.system(str(objdump) + " -s " + str(bin_dir) + "/" + str(target) + "_rom > " + str(bin_dir) + "/rom.objdump")
	os.system(str(objdump) + " -s " + str(bin_dir) + "/" + str(target) + "_sim > " + str(bin_dir) + "/sim.objdump")
	build_crc32_sum()
#	build_md5()
	rebuild_linker()
	for file in [ "ram", "rom", "sim" ]:
		os.remove(str(bin_dir) + '/' + file + '.objdump')

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
