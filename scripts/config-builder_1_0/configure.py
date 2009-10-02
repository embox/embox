#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Monitor configurator:
# === Config-Core ===
# date: 19.05.09
# author: sikmir
# requirement: python >= 2.6

from misc import *
from configure_gui import *
from configure_gen import *
import sys, string, os, traceback, json, shutil, getopt

class configure:
	def __init__(self, mode="x"):
		self.mode = mode
		self.files, self.linkers, self.menu = ( None, None, None )
		self.tabs, self.var = ({ "0" : 0 }, { "0" : 0 })

	def read_config(self, fileconf):
		""" Read .config/.config.in file """
		config       = read_file(fileconf)
		json_conf    = json.loads(config)
		self.files   = json_conf['Files']
		self.linkers = json_conf['Linkers']
		self.menu    = json_conf['Menu']
		for tab in self.menu.keys():
			self.tabs[tab] = json_conf[tab]

	def write_config(self, fileconf):
		""" Write .config/.config.in file """
		tmp = dict([('Files', self.files), \
			   ('Linkers', self.linkers), \
			   ('Menu', self.menu)])
		for item in sorted(self.menu.keys()):
			tmp[item] = self.tabs[item]
		#-- Arch
		mod_name = "Arch"
		for item in ("Cflags", "Ldflags", "Compiler"):
			arch = self.get_arch()
			tmp[mod_name][arch][item]["inc"] = self.var[mod_name][item].get()
		#-- Common
		mod_name = "Common"
		item     = "Target"
		tmp[mod_name][item]["inc"] = self.var[mod_name][item].get()
		#-- Conio
		mod_name = "Conio"
		for item in ("Prompt", "Prompt_max_lenght", "Start_msg"):
			tmp[mod_name][item]["inc"] = self.var[mod_name][item].get()
		write_file(fileconf, json.dumps(tmp, sort_keys = True, indent = 4))

	def reload_config(self):
		""" Reload config """
		self.read_config(".config.in")
		#-- Arch
		mod_name     = "Arch"
		cur_arch_num = self.tabs[mod_name]["Arch_num"]
		self.var[mod_name]["Arch_num"].set(cur_arch_num)
		for item in ("Cflags", "Ldflags", "Compiler"):
			arch = self.get_arch()
			inc  = self.tabs[mod_name][arch][item]["inc"]
			self.var[mod_name][item].set(inc)
		#-- Subframes
		for mod_name in self.menu:
			if mod_name != "Arch":
			      for item in self.tabs[mod_name].keys():
				       inc    = self.tabs[mod_name][item]["inc"]
				       status = self.tabs[mod_name][item]["status"]
				       self.var[mod_name][item].set(inc)
				       self.gui.widgets[mod_name][item].configure(state = getStatus(status))

	def get_arch(self):
		mod_name = "Arch"
		if self.var[mod_name]["Arch_num"].get() == 0:
			return "sparc"

	def make_conf(self):
		""" Generate code """
		code_gen = configure_gen(self)
#		code_gen.build_commands(self.files["shell_inc"], self.files["users_inc"])
		if self.mode == "x":
			code_gen.write_autoconf_h(self.files["autoconf_h"])
			code_gen.build_link(self.linkers)
			code_gen.write_autoconf(self.files["autoconf"])
			self.write_config(".config")

	def make_def_conf(self):
		""" Generate default code """
		code_gen = configure_gen(self)
#		code_gen.build_commands(self.files["shell_inc"], self.files["users_inc"])
		if self.mode == "x":
			code_gen.write_autoconf_h(self.files["autoconf_h"] + ".in")
			code_gen.build_link(self.linkers)
			code_gen.write_autoconf(self.files["autoconf"] + ".in")
			self.write_config(".config.in")

	def config_cmp(self, fconf1, fconf2):
		""" some verification """
		config1, config2 = read_file(fconf1), read_file(fconf2)
		json1, json2 = json.loads(config1), json.loads(config2)
		if not json1.keys().sort() == json2.keys().sort():
			return False
		for item in json1["Menu"].keys():
			if not json1[item].keys().sort() == json2[item].keys().sort():
				return False
		return True

	def restore_config(self):
		""" Restore files from default if not exist and read .config """
		for file in (".config", "scripts/autoconf", "scripts/autoconf.h"):
			if not os.path.exists(file):
			      shutil.copyfile(file + ".in", file)
		if not self.config_cmp(".config", ".config.in"):
			print ".config and .config.in have sharp distinction, maybe .config is out of date?"
			shutil.copyfile(".config.in", ".config")
			shutil.copyfile("scripts/autoconf.in", "scripts/autoconf")
		self.read_config(".config")
		for tab in self.menu.keys():
			self.var[tab] = { "0" : 0 }
		shutil.copyfile(".config", ".config.old")

	def main(self):
		""" Main: TIP: add self.gui.show_<item>(<name>) for new tabs here after declaration in configure_gui.py"""
		if self.mode == "x":
			self.gui = configure_gui(self)
			#-- Common frame
			self.gui.show_common("Common")
			#-- Arch frame
			self.gui.show_arch("Arch")
			#-- Drivers frame
			self.gui.show_table("Drivers")
			#-- Tests frame
			self.gui.show_table("Tests")
			#-- Users frame
			self.gui.show_table("Users")
			#-- Net frame
			self.gui.show_table("Net")
			#-- Conio frame
			self.gui.show_conio("Conio")
			#-- Fs frame
			self.gui.show_table("Fs")
			#-- Level frame
			self.gui.show_table("Levels")
			#-- Build frame
			self.gui.show_list("Build")
			#-- main loop
			self.gui.main()
		elif obj.mode == "menu":
			self.make_conf()
		else:
			print "Unknown mode"

if __name__=='__main__':
	try:
		opts, args = getopt.getopt(sys.argv[1:], "hm:", ["help", "mode="])
		for o, a in opts:
			if o in ("-h", "--help"):
				print "Usage: configure.py [-m <mode>] [-h]\n"
			elif o in ("-m", "--mode"):
				mode = a
				obj = configure(mode)
				obj.restore_config()
				obj.main()
			else:
				assert False, "unhandled option"
	except:
		traceback.print_exc()
