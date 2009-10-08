# -*- coding: utf-8 -*-
# Monitor configurator:
# === Config-GUI ===
# date: 26.06.09
# author: sikmir
# requirement: python >= 2.6

import Tkinter
from Tkinter import *
from .conftab import *
from ..Misc.misc import *
import tkSimpleDialog

class confgui:
	def __init__(self, core_obj):
		self.core = core_obj
		self.tabs = core_obj.tabs
		self.menu = core_obj.menu
		self.var  = core_obj.var
		self.widgets = { "0" : { "0" : None } }
		self.frame   = { "0" : 0 }
		self.root    = Tkinter.Tk()
		self.root.title("Monitor Configurator")
		#-- Create the menu frame, and add menus to the menu frame
		self.frame["Menu"] = Tkinter.Frame(self.root)
		self.frame["Menu"].pack(fill = Tkinter.X, side = Tkinter.TOP)
		self.frame["Menu"].tk_menuBar(self.file_menu(), self.help_menu())
		#-- Create the info frame and fill with initial contents
		self.frame["Info"] = Tkinter.Frame(self.root)
		self.frame["Info"].pack(fill = Tkinter.X, side = Tkinter.TOP, pady = 1)
		#-- Tabs frame
		self.frame["Main"] = conftab(self.frame["Info"], LEFT)
		#-- Init frames
		for item in sorted(self.menu.keys()):
			self.frame[item] = Tkinter.Frame(self.frame["Main"]())

	def main(self):
		""" build tabs """
		for item in sorted(self.menu.keys()):
			self.frame["Main"].add_screen(self.frame[item], item)
		self.root.mainloop()

	def About(self):
		view_window = Tkinter.Toplevel(self.root)
		about_text = "Monitor configurator\nAuthor: Nikolay Korotky\n2009"
		Tkinter.Label(view_window,  text = about_text).pack()
		Tkinter.Button(view_window, text = 'OK', command = view_window.destroy).pack()
		view_window.focus_set()
		view_window.grab_set()
		view_window.wait_window()

	def file_menu(self):
		file_btn = Tkinter.Menubutton(self.frame["Menu"], text='File', underline = 0)
		file_btn.pack(side = Tkinter.LEFT, padx = "2m")
		file_btn.menu = Tkinter.Menu(file_btn)
		#-- File menu ------
		file_btn.menu.add_command(label = "Save",            underline = 0, command = self.core.make_conf)
		file_btn.menu.add_command(label = "Save as default", underline = 0, command = self.core.make_def_conf)
		file_btn.menu.add_command(label = "Load default",    underline = 0, command = self.core.reload_config)
		file_btn.menu.add('separator')
		file_btn.menu.add_command(label = 'Exit',            underline = 0, command = file_btn.quit)
		#-------------------
		file_btn['menu'] = file_btn.menu
		return file_btn

	def help_menu(self):
		help_btn = Tkinter.Menubutton(self.frame["Menu"], text = 'Help', underline = 0,)
		help_btn.pack(side = Tkinter.LEFT, padx = "2m")
		help_btn.menu = Tkinter.Menu(help_btn)
		#-- Help menu ------
		help_btn.menu.add_command(label = "About",           underline = 0, command = self.About)
		#-------------------
		help_btn['menu'] = help_btn.menu
		return help_btn

	#-- ARCH TAB -----------------------------------------------------------------------------------------------------

	def change_arch(self):
		mod_name = 'Arch'
		self.tabs[mod_name]["Arch_num"] = self.var[mod_name]["Arch_num"].get()
		for item in [ "Compiler", "Ldflags", "Cflags" ]:
			arch = self.core.get_arch()
			inc  = self.tabs[mod_name][arch][item]["inc"]
		        self.var[mod_name][item].set(inc)

	def show_arch(self, mod_name):
		""" Arch widget """
		frame = self.frame[mod_name]
		Label(frame, text = mod_name, width = 25, background = "lightblue").grid(row = 0, column = 0)
		Label(frame, text = "",       width = 45, background = "lightblue").grid(row = 0, column = 1)
		self.var[mod_name]["Arch_num"] = IntVar()
		self.widgets[mod_name] = { "0" : None }
		k = 1
		for item in ("sparc",):
			self.widgets[mod_name][item] = Radiobutton(frame, \
							 text     = item,
							 value    = self.tabs[mod_name][item]["num"], \
							 variable = self.var[mod_name]["Arch_num"], \
							 command  = self.change_arch, \
							 anchor   = W)
			self.widgets[mod_name][item].grid(row = k, column = 0, sticky = W)
			k += 1
		current_arch_num = self.tabs[mod_name]["Arch_num"]
		self.var[mod_name]["Arch_num"].set(current_arch_num)
		#-- Compiler, Ldflags, Cflags subframes
		for item in [ "Compiler", "Ldflags", "Cflags" ]:
		        Label(frame, text = item, width = 25, background = "lightblue").grid(row = k, column = 0)
		    	self.var[mod_name][item] = StringVar()
		    	current_arch = self.core.get_arch()
		    	inc = self.tabs[mod_name][current_arch][item]["inc"]
		    	status = self.tabs[mod_name][current_arch][item]["status"]
		        self.widgets[mod_name][item] = Entry( frame, width = 25, \
		    					      state        = getStatus(status), \
		    					      textvariable = self.var[mod_name][item])
		        self.widgets[mod_name][item].grid(row = 1 + k, column = 0)
	    		self.var[mod_name][item].set(inc)
		        k += 2

	#-- CONIO TAB ----------------------------------------------------------------------------------------------------

	def show_conio(self, mod_name):
		""" Conio widget """
		frame = self.frame[mod_name]
		Label(frame, text = mod_name, width = 25, background = "lightblue").grid(row = 0, column = 0)
		Label(frame, text = "",       width = 45, background = "lightblue").grid(row = 0, column = 1)
		k = 1
		self.widgets[mod_name] = { "0" : None }
		for item in self.tabs[mod_name].keys():
		        Label(frame, text = item, width = 25, background = "lightblue").grid(row = k, column = 0)
		        self.var[mod_name][item] = StringVar()
		        inc    = self.tabs[mod_name][item]["inc"]
		        status = self.tabs[mod_name][item]["status"]
		        self.widgets[mod_name][item] = Entry( frame, width = 25, \
		    					      state        = getStatus(status), \
		    					      textvariable = self.var[mod_name][item])
		        self.widgets[mod_name][item].grid(row = 1 + k, column = 0)
		        self.var[mod_name][item].set(inc)
		        k += 2

	#-- COMMON TAB-------------------------------------------------------------------------------------------------

	def show_common(self, mod_name):
		""" Common widget """
		frame = self.frame[mod_name]
		Label(frame, text = mod_name, width = 25, background = "green").grid(row = 0, column = 0)
		Label(frame, text = "",       width = 45, background = "green").grid(row = 0, column = 1)
		k = 1
		self.widgets[mod_name] = { "0" : None }
                #-- Target entry
                item = "Target"
                Label(frame, text = item, width = 25, background = "lightblue").grid(row = k, column = 0)
                self.var[mod_name][item] = StringVar()
                inc = self.tabs[mod_name][item]["inc"]
                status = self.tabs[mod_name][item]["status"]
                self.widgets[mod_name][item] = Entry( frame, width        = 25, \
                					     state        = getStatus(status), \
                					     textvariable = self.var[mod_name][item])
                self.widgets[mod_name][item].grid(row = 1 + k, column = 0)
                self.var[mod_name][item].set(inc)
                k += 2
		#-- Sign checksum, Disassemble
		for item in ["Sign_bin", "Disassemble"]:
			self.var[mod_name][item] = IntVar()
			desc   = self.tabs[mod_name][item]["desc"]
			inc    = self.tabs[mod_name][item]["inc"]
			status = self.tabs[mod_name][item]["status"]
			self.widgets[mod_name][item] = Checkbutton(frame, \
							 text     = desc, \
							 state    = getStatus(status), \
							 anchor   = W, \
							 variable = self.var[mod_name][item], \
							 command  = (lambda item = item: onPress(self.tabs[mod_name], item, "inc")))
			self.widgets[mod_name][item].grid(row = k, column = 0, sticky = W)
			self.var[mod_name][item].set(inc)
			k += 1
		#-- Subframes
		k = 1
		for item in sorted(self.menu.keys()):
			if self.menu[item] == 1 and item not in ["Conio", "Users", "Drivers"]:
				self.var[mod_name][item] = IntVar()
				desc   = self.tabs[mod_name][item]["desc"]
				inc    = self.tabs[mod_name][item]["inc"]
				status = self.tabs[mod_name][item]["status"]
				self.widgets[mod_name][item] = Checkbutton(frame, \
							    text     = desc, \
							    state    = getStatus(status), \
							    anchor   = W, \
		            				    variable = self.var[mod_name][item], \
		            				    command  = (lambda mod = item: self.onPress_mod(mod)))
				self.widgets[mod_name][item].grid(row = k, column = 1, sticky = W)
				self.var[mod_name][item].set(inc)
				k += 1

	def onPress_mod(self, mod):
		mod_name = "Common"
		onPress(self.tabs[mod_name], mod, "inc")
		for item in self.tabs[mod]:
			inc = self.var[mod_name][mod].get()
			if inc == 1:
				self.widgets[mod][item].configure(state = NORMAL)
				self.tabs[mod][item]["status"] = 1
			else:
				self.widgets[mod][item].configure(state = DISABLED)
				self.tabs[mod][item]["status"] = 0

	#-- TABLE TEMPLATE FRAME ---------------------------------------------------------------------------------------------

	def onPress_dep(self, mod_name, item):
		onPress(self.tabs[mod_name], item, "inc")
		if not "dep" in self.tabs[mod_name][item].keys() or \
		   self.widgets[mod_name][item]["state"] == getStatus(0):
			return
		for mod in self.tabs[mod_name][item]["dep"].keys():
                        for obj in self.tabs[mod_name][item]["dep"][mod]:
                    		if self.tabs[mod_name][item]["inc"] == 1:
                    			self.widgets[mod][obj].configure(state = NORMAL)
                    		else:
                    			if self.tabs[mod][obj]["inc"] == 1:
                            			self.widgets[mod][obj].invoke()
                            		self.widgets[mod][obj].configure(state = DISABLED)
                self.widgets[mod_name][item].configure(state = NORMAL)

	def show_table(self, mod_name):
		""" Table widget """
		frame = self.frame[mod_name]
		Label(frame, text = mod_name,      width = 25, background = "lightblue").grid(row = 0, column = 0)
		Label(frame, text = "Description", width = 45, background = "lightblue").grid(row = 0, column = 1)
		row = 1
		self.widgets[mod_name] = { "0" : None }
		for item in sorted(self.tabs[mod_name].keys()):
			inc    = self.tabs[mod_name][item]["inc"]
			status = self.tabs[mod_name][item]["status"]
			desc   = self.tabs[mod_name][item]["desc"]
			self.var[mod_name][item] = IntVar()
			self.widgets[mod_name][item] = Checkbutton(frame, \
							 text     = item, \
							 state    = getStatus(status), \
							 anchor   = W, \
							 variable = self.var[mod_name][item], \
							 command  = (lambda item = item: self.onPress_dep(mod_name, item)))
			self.widgets[mod_name][item].grid(row = row, column = 0, sticky = W)
			self.var[mod_name][item].set(inc)
			Label(frame, \
				text   = desc, \
				state  = getStatus(status), \
				width  = 45, \
				anchor = W).grid(row = row, column = 1, sticky = W)
			row += 1

	#-- LIST TEMPLATE FRAME --------------------------------------------------------------------------------------------

	def onPress_radio(self, mod_name, item):
		onPress(self.tabs[mod_name], item, "inc")
		for entry in self.tabs[mod_name]:
			if item != entry:
				self.tabs[mod_name][entry]["inc"] = 0
				self.var[mod_name][entry].set(0)
		if not "dep" in self.tabs[mod_name][item].keys() or \
		   self.widgets[mod_name][item]["state"] == getStatus(0):
			return
		for mod in self.tabs[mod_name][item]["dep"].keys():
			for obj in self.tabs[mod_name][item]["dep"][mod]:
				if self.widgets[mod][obj]["state"] == getStatus(1) and \
				   self.var[mod][obj].get() == 1:
					self.widgets[mod][obj].invoke()

	def show_list(self, mod_name):
		""" List widget """
		frame = self.frame[mod_name]
		Label(frame, text = mod_name, width = 25, background = "lightblue").grid(row = 0, column = 0)
		Label(frame, text = "",       width = 45, background = "lightblue").grid(row = 0, column = 1)
		row = 1
		self.widgets[mod_name] = { "0" : None }
		for item in sorted(self.tabs[mod_name].keys()):
			inc    = self.tabs[mod_name][item]["inc"]
			status = self.tabs[mod_name][item]["status"]
			self.var[mod_name][item] = IntVar()
		        self.widgets[mod_name][item] = Radiobutton(frame, \
		    					 text     = item, \
		    					 value    = 1, \
		    					 state    = getStatus(status), \
		    					 anchor   = W, \
		                			 variable = self.var[mod_name][item], \
		                			 command  = (lambda item=item: self.onPress_radio(mod_name, item)))
		        self.widgets[mod_name][item].grid(row = row, column = 0, sticky = W)
		        self.var[mod_name][item].set(inc)
			row += 1
