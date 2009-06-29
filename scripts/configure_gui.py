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
import tkSimpleDialog

class configure_gui:
	def __init__(self, conf_obj):
		self.conf_obj = conf_obj
		self.tabs = conf_obj.tabs
		self.menu = conf_obj.menu
		self.frame = { "0" : 0 }
		self.root = Tkinter.Tk()
		self.root.title(self.tabs['Common']['Title'])
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
                for i in (1, 2, 3, 4, 5):
            		self.frame[self.menu[i]] = Tkinter.Frame(self.frame["Main"]())

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

	def show_common(self):
		Label(self.frame["Common"], text=self.tabs['Common']['Title'], width=25, background="green").grid(row=0, column=0)
	        Label(self.frame["Common"], text="", width=35, background="green").grid(row=0, column=1)
	        Label(self.frame["Common"], text="Программа предназначенная для началь-", width=35).grid(row=1, column=1)
	        Label(self.frame["Common"], text="ной инициализации и тестирования ап-", width=35).grid(row=2, column=1)
	        Label(self.frame["Common"], text="паратуры. А так же для ее отладки. А", width=35).grid(row=3, column=1)
	        Label(self.frame["Common"], text="так же для отладки системного кода для", width=35).grid(row=4, column=1)
	        Label(self.frame["Common"], text=" дальнейшего переноса кода в Линукс ", width=35).grid(row=5, column=1)
	        #-- Arch subframe
	        self.conf_obj.common_var["Arch_num"] = IntVar()
	        Label(self.frame["Common"], text="Arch", width=25, background="lightblue").grid(row=1, column=0)
	        for ar, value, mdef in self.tabs['Common']['Arch']:
	                Radiobutton(self.frame["Common"], text=ar, value=value, variable=self.conf_obj.common_var["Arch_num"], \
	                                            anchor=W).grid(row=value+2, column=0, sticky=W)
	        self.conf_obj.common_var["Arch_num"].set(self.tabs["Common"]["Arch_num"][0])
		#-- Compiler, Ldflags, Cflags, Target subframes
		k = 0
		for item in [ "Compiler", "Ldflags", "Cflags", "Prompt", "Start_msg", "Target" ]:
		        Label(self.frame["Common"], text=item, width=25, background="lightblue").grid(row=5 + k, column=0)
		        self.conf_obj.common_var[item] = StringVar()
		        Entry(self.frame["Common"], width=25, textvariable=self.conf_obj.common_var[item]).grid(row=6 + k, column=0)
		        self.conf_obj.common_var[item].set(self.tabs["Common"][item][0])
		        k += 2
		#-- Sign checksum
		k += 5
		self.conf_obj.common_var["Sign_bin"] = IntVar()
		Checkbutton(self.frame["Common"], text="Sign checksum", state=NORMAL, anchor=W, \
		        	variable = self.conf_obj.common_var["Sign_bin"]).grid(row=k, column=0, sticky=W)
		self.conf_obj.common_var["Sign_bin"].set(self.tabs["Common"]["Sign_bin"][0])
		#-- Disassemble
		k += 1
		self.conf_obj.common_var["Disassemble"] = IntVar()
		Checkbutton(self.frame["Common"], text="Disassemble", state=NORMAL, anchor=W, \
			        variable = self.conf_obj.common_var["Disassemble"]).grid(row=k, column=0, sticky=W)
		self.conf_obj.common_var["Disassemble"].set(self.tabs["Common"]["Disassemble"][0])

	def show_drivers(self):
	        Label(self.frame[self.menu[1]], text=self.menu[1], width=25, background="lightblue").grid(row=0, column=0)
	        Label(self.frame[self.menu[1]], text="Description", width=55, background="lightblue").grid(row=0, column=1)
	        self.conf_obj.vard = IntVar()
	        row = 1
	        for item in self.tabs[self.menu[1]].keys():
	                if item != "common":
	                        Label(self.frame[self.menu[1]], text=item, width=25, background="lightblue").grid(row=row, column=0)
	                        row +=1
	                tmp = 1
	                for driver, inc, status, desc, mdef in self.tabs[self.menu[1]][item]:
	                        setattr(self.conf_obj.vard, driver, IntVar())
	                        Checkbutton(self.frame[self.menu[1]], text=driver, state=getStatus(status), anchor=W, \
	                    		variable = getattr(self.conf_obj.vard, driver), \
	                        	command=(lambda tmp=tmp, item=item: \
	                        	self.conf_obj.onPress_dep(item, tmp-1, 1))).grid(row=row, column=0, sticky=W)
	                        getattr(self.conf_obj.vard, driver).set(inc)
	                        Label(self.frame[self.menu[1]], text=desc, state=getStatus(status), \
	                    					width=55, anchor=W).grid(row=row, column=1, sticky=W)
	                        row += 1
	                        tmp += 1
	                if item != "common":
	                        Label(self.frame[self.menu[1]], text="---------------------------------------", \
	                    							    width=25).grid(row=row, column=0)
	                        row += 1

	def show_tests(self):
	        Label(self.frame[self.menu[2]], text=self.menu[2], width=25, background="lightblue").grid(row=0, column=0)
	        Label(self.frame[self.menu[2]], text="Description", width=35, background="lightblue").grid(row=0, column=1)
	        self.conf_obj.vart = IntVar()
	        row = 1
	        for test_name, inc, status, desc, mdef in self.tabs[self.menu[2]]:
	                setattr(self.conf_obj.vart, test_name, IntVar())
	                Checkbutton(self.frame[self.menu[2]], text=test_name, state=getStatus(status), anchor=W, \
	            		variable = getattr(self.conf_obj.vart, test_name), \
	                        command=(lambda row=row: onPress(self.tabs[self.menu[2]], \
	                    						    row-1, 1))).grid(row=row, column=0, sticky=W)
	                getattr(self.conf_obj.vart, test_name).set(inc)
	                Label(self.frame[self.menu[2]], text=desc, state=getStatus(status), \
	            						    width=35, anchor=W).grid(row=row, column=1, sticky=W)
	                row += 1

	def show_users(self):
		Label(self.frame[self.menu[3]], text=self.menu[3], width=25, background="lightblue").grid(row=0, column=0)
	        Label(self.frame[self.menu[3]], text="Description", width=35, background="lightblue").grid(row=0, column=1)
	        self.conf_obj.varc = IntVar()
	        row = 1
	        for cmd, inc, status, desc, mdef in self.tabs[self.menu[3]]:
	                setattr(self.conf_obj.varc, cmd, IntVar())
	                Checkbutton(self.frame[self.menu[3]], text=cmd, state=getStatus(status), anchor=W, \
	            		variable = getattr(self.conf_obj.varc, cmd), \
	                        command=(lambda row=row: onPress(self.tabs[self.menu[3]], \
	                    							row-1, 1))).grid(row=row, column=0, sticky=W)
	                getattr(self.conf_obj.varc, cmd).set(inc)
	                Label(self.frame[self.menu[3]], text=desc, state=getStatus(status), \
	            							width=35, anchor=W).grid(row=row, column=1, sticky=W)
	                row += 1

	def show_levels(self):
	        Label(self.frame[self.menu[4]], text="Verbous level", width=25, background="lightblue").grid(row=0, column=0)
	        Label(self.frame[self.menu[4]], text="", width=35).grid(row=0, column=1)
	        for i in range( len(self.tabs[self.menu[4]].keys()) ):
	                name = str(self.tabs[self.menu[4]].keys()[i])
	                self.conf_obj.level_var[name] = IntVar()
	                Checkbutton(self.frame[self.menu[4]], text=self.tabs[self.menu[4]].keys()[i], state=NORMAL, anchor=W, \
	                                        variable = self.conf_obj.level_var[name]).grid(row=i+1, column=0, sticky=W)
	                self.conf_obj.level_var[name].set(self.tabs[self.menu[4]][name][0])

	def show_build(self):
	        Label(self.frame[self.menu[5]], text=self.menu[5], width=25, background="lightblue").grid(row=0, column=0)
	        Label(self.frame[self.menu[5]], text="", width=35).grid(row=0, column=1)
	        for i in range( len(self.tabs[self.menu[5]].keys()) ):
	                name = str(self.tabs[self.menu[5]].keys()[i])
	                self.conf_obj.build_var[name] = IntVar()
	                Checkbutton(self.frame[self.menu[5]], text=self.tabs[self.menu[5]].keys()[i], state=NORMAL, anchor=W, \
	                                            variable = self.conf_obj.build_var[name]).grid(row=i+1, column=0, sticky=W)
	    	        self.conf_obj.build_var[name].set(self.tabs[self.menu[5]][name][0])
