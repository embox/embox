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
		self.widgets = { "0" : None }
		self.frame = { "0" : 0 }
		self.root = Tkinter.Tk()
		self.root.title("Monitor Configurator")
		#-- Create the menu frame, and add menus to the menu frame
		self.frame["Menu"] = Tkinter.Frame(self.root)
		self.frame["Menu"].pack(fill=Tkinter.X, side=Tkinter.TOP)
		self.frame["Menu"].tk_menuBar(self.file_menu(), self.help_menu())
		#-- Create the info frame and fill with initial contents
		self.frame["Info"] = Tkinter.Frame(self.root)
		self.frame["Info"].pack(fill=Tkinter.X, side=Tkinter.TOP, pady=1)
		#-- Tabs frame
		self.frame["Main"] = conf_tab(self.frame["Info"], LEFT)
		#-- Common, Drivers, Tests, Users, Level, Build frames
		for item in self.menu:
			self.frame[item] = Tkinter.Frame(self.frame["Main"]())

	def main(self):
		#-- build tabs
		for item in self.menu:
			self.frame["Main"].add_screen(self.frame[item], item)
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
			inc = self.conf_obj.archs[arch][item]["inc"]
		        self.conf_obj.var['Arch'][item].set(inc)

	def show_common(self, text):
		mod_name = "Common"
		Label(self.frame[mod_name], text=text, width=25, background="green").grid(row=0, column=0)
		Label(self.frame[mod_name], text="", width=45, background="green").grid(row=0, column=1)
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
		        self.widgets[item] = Entry(self.frame[mod_name], width=25, textvariable=self.conf_obj.var['Arch'][item])
		        self.widgets[item].grid(row = 1 + k, column=0)
	    		self.conf_obj.var['Arch'][item].set(self.conf_obj.archs[self.conf_obj.get_arch()][item]["inc"])
		        k += 2
		#-- Prompt, Start_msg, Target subframes
		for item in [ "Prompt", "Start_msg", "Target" ]:
		        Label(self.frame[mod_name], text=item, width=25, background="lightblue").grid(row = k, column=0)
		        self.conf_obj.var[mod_name][item] = StringVar()
		        inc = self.tabs[mod_name][item]["inc"]
		        self.widgets[item] = Entry(self.frame[mod_name], width=25, textvariable=self.conf_obj.var[mod_name][item])
		        self.widgets[item].grid(row = 1 + k, column=0)
		        self.conf_obj.var[mod_name][item].set(inc)
		        k += 2
		#-- Sign checksum, Disassemble
		for item in ["Sign_bin", "Disassemble"]:
			self.conf_obj.var[mod_name][item] = IntVar()
			desc = self.tabs[mod_name][item]["desc"]
			inc = self.tabs[mod_name][item]["inc"]
			self.widgets[item] = Checkbutton(self.frame[mod_name], text=desc, \
						state=NORMAL, anchor=W, \
						variable = self.conf_obj.var[mod_name][item])
			self.widgets[item].grid(row=k, column=0, sticky=W)
			self.conf_obj.var[mod_name][item].set(inc)
			k += 1
		#-- Express tests, Network, Drivers, Fs, Conio, Users
		k = 1
		for item in ("Tests", "Net", "Drivers", "Fs", "Conio", "Users"):
			self.conf_obj.var[mod_name][item] = IntVar()
			desc = self.tabs[mod_name][item]["desc"]
			inc = self.tabs[mod_name][item]["inc"]
			self.widgets[item] =  Checkbutton(self.frame[mod_name], text=desc, \
						state=NORMAL, anchor=W, \
		            			variable = self.conf_obj.var[mod_name][item], \
		            			command=(lambda mod=item: self.onPress_mod(mod)))
			self.widgets[item].grid(row=k, column=1, sticky=W)
			self.conf_obj.var[mod_name][item].set(inc)
			k += 1

	def onPress_mod(self, mod):
		onPress(self.tabs["Common"], mod, "inc")
		if mod in ("Fs", "Conio"):
			return
		for item in self.tabs[mod]:
			inc = self.conf_obj.var["Common"][mod].get()
			self.conf_obj.var[mod][item].set(inc)
			if inc == 1:
				self.widgets[item].configure(state=NORMAL)
				self.tabs[mod][item]["status"] = 1
			else:
				self.widgets[item].configure(state=DISABLED)
				self.tabs[mod][item]["status"] = 0

	def onPress_dep(self, mod_name, item):
		onPress(self.tabs[mod_name], item, "inc")
		if not "dep" in self.tabs[mod_name][item].keys():
			return
		for dep in self.tabs[mod_name][item]["dep"]:
			if not dep == "":
				if re.match("test_", dep) == None:
					mod_name = "Users"
					if self.tabs[mod_name][dep]["inc"] == 1:
						onPress(self.tabs[mod_name], dep, "inc")
						inc = self.tabs[mod_name][dep]["inc"]
						self.conf_obj.var[mod_name][dep].set(inc)
				else:
					mod_name = "Tests"
					if self.tabs[mod_name][dep]["inc"] == 1:
						onPress(self.tabs[mod_name], dep, "inc")
						inc = self.tabs[mod_name][dep]["inc"]
						self.conf_obj.var[mod_name][dep].set(inc)

	def show_table(self, mod_name):
		Label(self.frame[mod_name], text=mod_name, width=25, background="lightblue").grid(row=0, column=0)
		Label(self.frame[mod_name], text="Description", width=45, background="lightblue").grid(row=0, column=1)
		row = 1
		for item in sorted(self.tabs[mod_name].keys()):
			inc    = self.tabs[mod_name][item]["inc"]
			status = self.tabs[mod_name][item]["status"]
			desc   = self.tabs[mod_name][item]["desc"]
			self.conf_obj.var[mod_name][item] = IntVar()
			self.widgets[item] = Checkbutton(self.frame[mod_name], text=item, \
							 state=getStatus(status), anchor=W, \
							 variable = self.conf_obj.var[mod_name][item], \
							 command=(lambda item=item: \
							 self.onPress_dep(mod_name, item)))
			self.widgets[item].grid(row=row, column=0, sticky=W)
			self.conf_obj.var[mod_name][item].set(inc)
			Label(self.frame[mod_name], text=desc, state=getStatus(status), \
					            width=45, anchor=W).grid(row=row, column=1, sticky=W)
			row += 1

	def onPress_radio(self, mod_name, item):
		onPress(self.tabs[mod_name], item, "inc")
		for entry in self.tabs[mod_name]:
			if item != entry:
				self.tabs[mod_name][entry]["inc"] = False
				self.conf_obj.var[mod_name][entry].set(0)

	def show_list(self, mod_name):
		Label(self.frame[mod_name], text=mod_name, width=25, background="lightblue").grid(row=0, column=0)
		Label(self.frame[mod_name], text="", width=45, background="lightblue").grid(row=0, column=1)
		row = 1
		for item in sorted(self.tabs[mod_name].keys()):
			inc = self.tabs[mod_name][item]["inc"]
			status = self.tabs[mod_name][item]["status"]
			self.conf_obj.var[mod_name][item] = IntVar()
		        self.widgets[item] = Radiobutton(self.frame[mod_name], text = item, \
		    					 value = 1, state = getStatus(status), anchor = W, \
		                			 variable = self.conf_obj.var[mod_name][item], \
		                			 command=(lambda item=item: \
		                			 self.onPress_radio(mod_name, item)))
		        self.widgets[item].grid(row = row, column = 0, sticky = W)
		        self.conf_obj.var[mod_name][item].set(inc)
			row += 1
