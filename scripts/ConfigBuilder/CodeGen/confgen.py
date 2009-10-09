# -*- coding: utf-8 -*-
# Monitor configurator:
# === Code-Generator ===
# date: 11.07.09
# author: sikmir
# requirement: python >= 2.6

from ..Misc.misc import *
import string, re, os

class confgen:
	def __init__(self, core_obj):
    		self.core = core_obj
		self.tabs = core_obj.tabs
		self.menu = core_obj.menu
		self.var  = core_obj.var

	def build_link(self, files):
		""" Edit link scripts """
	        for file in files:
	                content = read_file('scripts/' + file)
	                content = re.sub('OUTPUT_ARCH\((\w+)\)', "OUTPUT_ARCH({0})".format(self.core.get_arch()), content)
	                write_file('scripts/' + file, content)
	        os.remove("include/asm")
	        os.symlink("asm-{0}".format(self.core.get_arch()), "include/asm")

	def write_autoconf(self, file):
		""" Generate autoconf """
	        #-- read autoconf
	        content = read_file(file)
		#-- Arch ------------------------------------------------------------------------
	        mod_name = "Arch"
	        for item in ("sparc",):
	                mdef    = self.tabs[mod_name][item]["mdef"]
	                inc     = (self.tabs[mod_name][item]["num"] == self.var[mod_name]["Arch_num"].get())
	                content = replacer( mdef, inc, content)
	        #-- Compiler, CFLAGS, LDFLAGS
	        arch = self.core.get_arch()
	        for item in ("Compiler", "Cflags", "Ldflags"):
	    		mdef = self.tabs[mod_name][arch][item]["mdef"]
	    		reg  = self.tabs[mod_name][arch][item]["re"]
	    		content = re.sub( mdef + '=' + reg, mdef + "=" + self.var[mod_name][item].get(), content)
		#-- Common ---------------------------------------------------------------------
                mod_name = "Common"
                mdef     = self.tabs[mod_name]["Target"]["mdef"]
                reg      = self.tabs[mod_name]["Target"]["re"]
                content  = re.sub(mdef + '=' + reg, mdef + "=" + self.var[mod_name]["Target"].get(), content)
		#-- Sign checksum, Disassemble
                for item in ["Sign_bin", "Disassemble"]:
                        mdef    = self.tabs[mod_name][item]["mdef"]
                        inc     = (self.var[mod_name][item].get() == 1)
                        content = replacer(mdef, inc, content)
                #-- Subframes enable
                for item in self.menu.keys():
            		if self.menu[item] == 1 and item not in ["Conio", "Users", "Drivers"]:
            			mdef    = self.tabs[mod_name][item]["mdef"]
            			inc     = (self.var[mod_name][item].get() == 1)
            			content = replacer(mdef, inc, content)
        	#-- Subframes --------------------------------------------------------------------
		for mod_name in self.menu:
        	        if not mod_name in ("Common", "Arch", "Conio"):
        	                for item in self.tabs[mod_name].keys():
        	                        inc     = self.tabs[mod_name][item]["inc"]
        	                        mdef    = self.tabs[mod_name][item]["mdef"]
        	                        content = replacer(mdef, inc, content)
                #-- write autoconf
                write_file(file, content)

	def write_autoconf_h(self, file):
		""" Generate autoconf.h """
		#-- read conf_h
		content = read_file(file)
		#-- Arch ------------------------------------------------------------------------
		mod_name = "Arch"
		for item in ("sparc",):
			mdef    = self.tabs[mod_name][item]['mdef']
			inc     = (self.tabs[mod_name][item]["num"] == self.var[mod_name]["Arch_num"].get())
			content = replacer_h(mdef, inc, content)
		#-- Conio ----------------------------------------------------------------------
		mod_name = "Conio"
		for item in ["Prompt"]:#, "Start_msg"]:
			mdef = self.tabs[mod_name][item]["mdef"]
			re   = self.tabs[mod_name][item]["re"]
			content = replacer_value(mdef, re, '"{0}"'.format(self.var[mod_name][item].get()), content)
		item = "Prompt_max_lenght"
		mdef = self.tabs[mod_name][item]["mdef"]
		re   = self.tabs[mod_name][item]["re"]
		content = replacer_value(mdef, re, self.var[mod_name][item].get(), content)
		#-- Common ---------------------------------------------------------------------
		mod_name = "Common"
		for item in self.menu.keys():
			if self.menu[item] == 1 and item not in ["Conio", "Users", "Drivers"]:
				mdef    = self.tabs[mod_name][item]["mdef"]
				inc     = (self.var[mod_name][item].get() == 1)
				content = replacer_h(mdef, inc, content)
		#-- Subframes ------------------------------------------------------------------
		for mod_name in self.menu:
			if not mod_name in ("Common", "Arch", "Conio"):
				for item in self.tabs[mod_name].keys():
					inc     = self.tabs[mod_name][item]["inc"]
					mdef    = self.tabs[mod_name][item]["mdef"]
					content = replacer_h(mdef, inc, content)
		#-- write conf_h
		write_file(file, content)
