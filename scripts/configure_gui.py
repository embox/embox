#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Monitor configurator gui
# date: 26.06.09
# author: sikmir
# requirement: python >= 2.6

import Tkinter
from Tkinter import *
from conf_tab import *
from misc import *
import tkSimpleDialog, re

class configure_gui:
	def __init__(self, conf_obj):
		self.conf_obj = conf_obj
		self.tabs = conf_obj.tabs
		self.menu = conf_obj.menu
		self.frame = { "0" : 0 }
		self.root = Tkinter.Tk()
		self.root.title(self.tabs['Title'])
		#-- Create the menu frame, and add menus to the menu frame
		self.frame["Menu"] = Tkinter.Frame(self.root)
		self.frame["Menu"].pack(fill=Tkinter.X, side=Tkinter.TOP)
		self.frame["Menu"].tk_menuBar(self.file_menu(), self.help_menu())
		#-- Create the info frame and fill with initial contents
		self.frame["Info"] = Tkinter.Frame(self.root)
		self.frame["Info"].pack(fill=Tkinter.X, side=Tkinter.TOP, pady=1)
		#-- Tabs frame
		self.frame["Main"] = conf_tab(self.frame["Info"], LEFT)
		#-- Common frame
		self.frame["Common"] = Tkinter.Frame(self.frame["Main"]())
		#-- Drivers, Tests, Commands, Level, Build frames
		for item in self.menu:
			self.frame[item] = Tkinter.Frame(self.frame["Main"]())

	def main(self):
		#-- build tabs
		for i in range( len(self.menu) ):
			self.frame["Main"].add_screen(self.frame[str(self.menu[i])], str(self.menu[i]))
		self.root.mainloop()

	def About(self):
		view_window = Tkinter.Toplevel(self.root)
		about_text = "Monitor configurator\nAuthor: Nikolay Korotky\n2009"
		Tkinter.Label(view_window,  text=about_text).pack()
		Tkinter.Button(view_window, text='OK', command=view_window.destroy).pack()
		view_window.focus_set()
		view_window.grab_set()
		view_window.wait_window()

	def file_menu(self):
		file_btn = Tkinter.Menubutton(self.frame["Menu"], text='File', underline=0)
		file_btn.pack(side=Tkinter.LEFT, padx="2m")
		file_btn.menu = Tkinter.Menu(file_btn)
		file_btn.menu.add_command(label="Save", underline=0, command=self.conf_obj.make_conf)
		file_btn.menu.add_command(label="Save as default", underline=0, command=self.conf_obj.make_def_conf)
		file_btn.menu.add_command(label="Load default", underline=0, command=self.conf_obj.reload_config)
		file_btn.menu.add('separator')
		file_btn.menu.add_command(label='Exit', underline=0, command=file_btn.quit)
		file_btn['menu'] = file_btn.menu
		return file_btn

	def help_menu(self):
		help_btn = Tkinter.Menubutton(self.frame["Menu"], text='Help', underline=0,)
		help_btn.pack(side=Tkinter.LEFT, padx="2m")
		help_btn.menu = Tkinter.Menu(help_btn)
		help_btn.menu.add_command(label="About", underline=0, command=self.About)
		help_btn['menu'] = help_btn.menu
		return help_btn

	#---------------------------------------------------------------------------------------------------------------

	def change_arch(self):
		for item in [ "Compiler", "Ldflags", "Cflags" ]:
			arch = self.conf_obj.get_arch()
		        self.conf_obj.var['Arch'][item].set(self.conf_obj.archs[arch][item]["inc"])

	def show_common(self, text):
		mod_name = "Common"
		Label(self.frame[mod_name], text=text, width=25, background="green").grid(row=0, column=0)
		Label(self.frame[mod_name], text="", width=35, background="green").grid(row=0, column=1)
		#-- Arch subframe
		self.conf_obj.var['Arch']["Arch_num"] = IntVar()
		Label(self.frame[mod_name], text="Arch", width = 25, background="lightblue").grid(row=1, column=0)
		k = 2
		for ar in ("sparc",):
			Radiobutton(self.frame[mod_name], text = ar, value = self.conf_obj.archs[ar]['num'], \
				variable = self.conf_obj.var['Arch']['Arch_num'], \
				command = self.change_arch, \
				anchor = W).grid(row = k, column = 0, sticky = W)
			k += 1
		self.conf_obj.var['Arch']["Arch_num"].set(self.conf_obj.archs['Arch_num'])
		#-- Compiler, Ldflags, Cflags subframes
		for item in [ "Compiler", "Ldflags", "Cflags" ]:
		        Label(self.frame[mod_name], text=item, width=25, background="lightblue").grid(row = k, column=0)
		    	self.conf_obj.var['Arch'][item] = StringVar()
		        Entry(self.frame[mod_name], width=25, textvariable=self.conf_obj.var['Arch'][item]).grid(row = 1 + k, column=0)
	    		self.conf_obj.var['Arch'][item].set(self.conf_obj.archs[self.conf_obj.get_arch()][item]["inc"])
		        k += 2
		#-- Prompt, Start_msg, Target subframes
		for item in [ "Prompt", "Start_msg", "Target" ]:
		        Label(self.frame[mod_name], text=item, width=25, background="lightblue").grid(row = k, column=0)
		        self.conf_obj.var[mod_name][item] = StringVar()
		        Entry(self.frame[mod_name], width=25, textvariable=self.conf_obj.var["Common"][item]).grid(row = 1 + k, column=0)
		        self.conf_obj.var[mod_name][item].set(self.tabs[mod_name][item]["inc"])
		        k += 2
		#-- Sign checksum, Disassemble
		for item in ["Sign_bin", "Disassemble"]:
			self.conf_obj.var[mod_name][item] = IntVar()
			Checkbutton(self.frame[mod_name], text=self.tabs[mod_name][item]["desc"], state=NORMAL, anchor=W, \
				variable = self.conf_obj.var[mod_name][item]).grid(row=k, column=0, sticky=W)
			self.conf_obj.var[mod_name][item].set(self.tabs[mod_name][item]["inc"])
			k += 1
		#-- Express tests
		self.conf_obj.var[mod_name]["Express"] = IntVar()
		Checkbutton(self.frame[mod_name], text=self.tabs[mod_name]["Express"]["desc"], state=NORMAL, anchor=W, \
		                variable = self.conf_obj.var[mod_name]["Express"], \
		                command=(lambda: self.onPress_tests())).grid(row=1, column=1, sticky=W)
		self.conf_obj.var[mod_name]["Express"].set(self.tabs[mod_name]["Express"]["inc"])

	def onPress_tests(self):
		for test in self.tabs["Tests"]:
			inc = self.conf_obj.var["Common"]["Express"].get()
			self.conf_obj.var["Tests"][test].set(inc)

	def onPress_dep(self, ar, item, inc):
		onPress(ar, item, inc)
		for dep in ar[item]["dep"]:
			if not dep == "":
				if re.match("test_", dep) == None:
					if self.tabs["Commands"][dep]["inc"] == 1:
						onPress(self.tabs["Commands"], dep, "inc")
						inc = self.tabs["Commands"][dep]["inc"]
						self.conf_obj.var["Commands"][dep].set(inc)
				else:
					if self.tabs["Tests"][dep]["inc"] == 1:
						onPress(self.tabs["Tests"], dep, "inc")
						inc = self.tabs["Tests"][dep]["inc"]
						self.conf_obj.var["Tests"][dep].set(inc)

	def show_drivers(self, text):
		mod_name = "Drivers"
	        Label(self.frame[mod_name], text=text, width=25, background="lightblue").grid(row=0, column=0)
	        Label(self.frame[mod_name], text="Description", width=55, background="lightblue").grid(row=0, column=1)
	        row = 1
	        for item in self.tabs[mod_name].keys():
	                if item != "common":
	                        Label(self.frame[mod_name], text=item, width=25, background="lightblue").grid(row=row, column=0)
	                        row +=1
	                for driver in self.tabs[mod_name][item].keys():
	            		inc = self.tabs[mod_name][item][driver]["inc"]
	            		status = self.tabs[mod_name][item][driver]["status"]
	            		desc = self.tabs[mod_name][item][driver]["desc"]
	            		mdef = self.tabs[mod_name][item][driver]["mdef"]
	                        self.conf_obj.var[mod_name][driver] = IntVar()
	                        Checkbutton(self.frame[mod_name], text=driver, state=getStatus(status), anchor=W, \
	                    		variable = self.conf_obj.var[mod_name][driver], \
	                        	command=(lambda item=item, driver=driver: \
	                        	self.onPress_dep(self.tabs[mod_name][item], driver, "inc"))).grid(row=row, column=0, sticky=W)
	                        self.conf_obj.var[mod_name][driver].set(inc)
	                        Label(self.frame[mod_name], text=desc, state=getStatus(status), \
	                    					width=55, anchor=W).grid(row=row, column=1, sticky=W)
	                        row += 1
	                if item != "common":
	                        Label(self.frame[mod_name], text="---------------------------------------", \
	                    							    width=25).grid(row=row, column=0)
	                        row += 1

	def show_table(self, mod_name, text, column=2):
		Label(self.frame[mod_name], text=text, width=25, background="lightblue").grid(row=0, column=0)
		if column == 2:
			desc_text = "Description"
		else:
			desc_text = ""
		Label(self.frame[mod_name], text=desc_text, width=35, background="lightblue").grid(row=0, column=1)
		row = 1
		for item in sorted(self.tabs[mod_name].keys()):
			inc = self.tabs[mod_name][item]["inc"]
			status = self.tabs[mod_name][item]["status"]
			desc = self.tabs[mod_name][item]["desc"]
			self.conf_obj.var[mod_name][item] = IntVar()
			Checkbutton(self.frame[mod_name], text=item, state=getStatus(status), anchor=W, \
				    variable = self.conf_obj.var[mod_name][item], \
				    command=(lambda item=item: onPress(self.tabs[mod_name], \
							item, "inc"))).grid(row=row, column=0, sticky=W)
			self.conf_obj.var[mod_name][item].set(inc)
			if column == 2:
				Label(self.frame[mod_name], text=desc, state=getStatus(status), \
					width=35, anchor=W).grid(row=row, column=1, sticky=W)
			row += 1
