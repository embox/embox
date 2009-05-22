#!/usr/bin/env python
# -*- coding: utf-8 -*-
# date: 19.05.09
# author: sikmir

import Tkinter
from Tkinter import *
import string, os, traceback
import tkSimpleDialog
import re
import json
import shutil

commands, tests, drivers, arch = (None, None, None, None)
error_level, trace_level, warn_level, debug_level, test_system_level, leon3_level = (None, None, None, None, None, None)
error_level_var, trace_level_var, warn_level_var, debug_level_var, test_system_level_var, leon3_level_var = (None, None, None, None, None, None)
root, menu_frame, info_frame = (None, None, None)
debug, release, simulation, docs, dis = (None, None, None, None, None)
debug_var, release_var, simulation_var, docs_var, dis_var = (None, None, None, None, None)
compiler, target, arch_num, cflags, ldflags = (None, None, None, None, None)
comiler_var, target_var, arch_num_var, cflags_var, ldflags_var = (None, None, None, None, None)
shell_inc, tests_inc, subdirs, tests_table_inc = (None, None, None, None)

# tabs realization
class monitor_tab:
        def __init__(self, master, side=LEFT):
                self.active_fr = None
                self.count = 0
                self.choice = IntVar(0)

                if side in (TOP, BOTTOM):
                        self.side = LEFT
                else:
                        self.side = TOP

                self.rb_fr = Frame(master, borderwidth=2, relief=RIDGE)
                self.rb_fr.pack(side=side, fill=BOTH)
                self.screen_fr = Frame(master, borderwidth=2, relief=RIDGE)
                self.screen_fr.pack(fill=BOTH)

        def __call__(self):
                return self.screen_fr

        def add_screen(self, fr, title):
                b = Radiobutton(self.rb_fr, text=title, indicatoron=0, \
                        variable=self.choice, value=self.count, \
                        command=lambda: self.display(fr))
                b.pack(fill=BOTH, side=self.side)

                if not self.active_fr:
                        fr.pack(fill=BOTH, expand=1)
                        self.active_fr = fr

                self.count += 1

                return b

	def display(self, fr):
                self.active_fr.forget()
                fr.pack(fill=BOTH, expand=1)
                self.active_fr = fr

def read_config(fileconf):
	""" Read config file """
        global error_level, trace_level, warn_level, debug_level, test_system_level, leon3_level
        global debug, release, simulation, docs, dis, compiler, target, arch_num, cflags, ldflags
        global shell_inc, tests_inc, subdirs, tests_table_inc
        global commands, tests, drivers, arch
        with open(fileconf, 'r') as conf:
                config = conf.read()
                json_conf = json.loads(config)
                commands =  json_conf['commands']
                tests = json_conf['tests']
                drivers = json_conf['drivers']
                arch = json_conf['arch']
                compiler = json_conf['compiler']
                target = json_conf['target']
                arch_num = json_conf['arch_num']
                shell_inc = json_conf['shell_inc']
                tests_inc = json_conf['tests_inc']
                subdirs = json_conf['subdirs']
                tests_table_inc = json_conf['tests_table_inc']
                error_level = json_conf['error_level']
                trace_level = json_conf['trace_level']
                warn_level = json_conf['warn_level']
                debug_level = json_conf['debug_level']
                test_system_level = json_conf['test_system_level']
                leon3_level = json_conf['leon3_level']
                debug = json_conf['debug']
                release = json_conf['release']
                simulation = json_conf['simulation']
                docs = json_conf['docs']
                dis = json_conf['dis']
                cflags = json_conf['cflags']
                ldflags = json_conf['ldflags']
        conf.close()

def write_config(fileconf):
	""" Write config file """
        global error_level_var, trace_level_var, warn_level_var, debug_level_var, test_system_level_var, leon3_level_var
        global debug_var, release_var, simulation_var, docs_var, dis_var, compiler_var, target_var, arch_num_var
        global shell_inc, tests_inc, subdirs, tests_table_inc
        global commands, tests, drivers, arch, cflags, ldflags
        with open(fileconf, 'w+') as conf:
                tmp = {'shell_inc' : "src/conio/shell.inc", 'tests_inc' : "src/conio/tests.inc", \
                        'subdirs' : "src/tests/subdirs", 'tests_table_inc' : "src/tests/tests_table.inc" }
                tmp['tests'] = tests
                tmp['commands'] = commands
                tmp['drivers'] = drivers
                tmp['arch'] = arch
                tmp["compiler"] = compiler_var.get()
                tmp["target"] = target_var.get()
                tmp["cflags"] = cflags_var.get()
                tmp["ldflags"] = ldflags_var.get()
                tmp["arch_num"] = arch_num_var.get()
                tmp["error_level"] = error_level_var.get()
                tmp["trace_level"] = trace_level_var.get()
                tmp["warn_level"] = warn_level_var.get()
                tmp["debug_level"] = debug_level_var.get()
                tmp["test_system_level"] = test_system_level_var.get()
                tmp["leon3_level"] = leon3_level_var.get()
                tmp["debug"] = debug_var.get()
                tmp["release"] = release_var.get()
                tmp["simulation"] = simulation_var.get()
                tmp["docs"] = docs_var.get()
                tmp["dis"] = dis_var.get()
                conf.write(json.dumps(tmp))
        conf.close()

def reload_config(fileconf):
        """ Reload config """
	read_config(fileconf)
	#TODO:

def onPress(ar, i, j):
	ar[i][j] = not ar[i][j]

def getStatus(i):
	if i == 1:
		return "normal"
	if i == 0:
		return "disabled"

def make_conf():
	build_commands()
	build_makefile()
	build_tests()
	build_drivers()
	write_config("config")

def build_commands():
	#-- generate src/conio/shell.inc
	with open(shell_inc, 'w+') as fshell:
		fshell.write("//Don't edit! shell.inc: auto-generated by configure.py\n\n")
		for test, pack, inc, status, desc in commands:
			if inc == True:
				if test != "wmem":
					fshell.write("{\"" + test + "\", \"" + desc + "\", " + test + "_shell_handler},\n")
				else:
					fshell.write("{\"" + test + "\", \"" + desc + "\", " + test + "_shell_handler}\n")
	fshell.close()
	#-- generate src/conio/tests.inc
	with open(tests_inc, 'w+') as ftest_include:
		ftest_include.write("//Don't edit! test.inc: auto-generated by configure.py\n\n")
		for test, pack, inc, status, desc in commands:
			if inc == True:
				if test != "arp":
					ftest_include.write("#include \"" + test + ".h\"\n")
				else:
					ftest_include.write("#include \"" + test + "c.h\"\n")
	ftest_include.close()
	#-- generate src/tests/subdirs
	with open(subdirs, 'w+') as fsubdirs:
		fsubdirs.write("SUBDIRS:= \\\n")
		for test, pack, inc, status, desc in commands:
			if inc == True:
				if pack != "wmem":
					fsubdirs.write(str(pack) + " \\\n")
				else:
					fsubdirs.write(str(pack) + " \n")
	fsubdirs.close()

def repl_arch(m):
	return "CPU_ARCH:= " + arch[arch_num_var.get()][0]

def repl_compil(m):
	return "CC_PACKET:= " + compiler_var.get()

def repl_target(m):
	return "TARGET:= " + target_var.get()

def repl_all(m):
	repl = "all: "
	if debug_var.get() == 1:
		repl += "debug "
	if release_var.get() == 1:
		repl += "release "
	if simulation_var.get() == 1:
		repl += "simulation "
	if docs_var.get() == 1:
		repl += "docs "
	return repl

def repl_cflag(m):
	repl = "CCFLAGS:= " + cflags_var.get()
	if leon3_level_var.get() == 1:
		repl += " -DLEON3"
	if test_system_level_var.get() == 1:
		repl += " -D_TEST_SYSTEM_"
	if error_level_var.get() == 1:
		repl += " -D_ERROR"
	if trace_level_var.get() == 1:
		repl += " -D_TRACE"
	if warn_level_var.get() == 1:
		repl += " -D_WARN"
	if debug_level_var.get() == 1:
		repl += " -D_DEBUG"
	return repl

def repl_ldflag(m):
        repl = "LDFLAGS:= " + ldflags_var.get()
        return repl

def build_makefile():
	#-- generate makefile
	with open('makefile', 'r+') as fmk:
		content = fmk.read()
	fmk.close()
	content = re.sub('CPU_ARCH:= (\w+)', repl_arch, content)
	content = re.sub('CC_PACKET:= (\w+(-\w+)?)', repl_compil, content)
	content = re.sub('TARGET:= (\w+)', repl_target, content)
	content = re.sub('CCFLAGS:= ([A-Za-z0-9_\-# ]+)', repl_cflag, content)
	content = re.sub('LDFLAGS:= ([A-Za-z0-9_\-# ]+)', repl_ldflag, content)
	with open('makefile', 'w+') as fmk:
		fmk.write(content)
	fmk.close()
	#-- generate src/makefile
	with open('src/makefile', 'r+') as fmk:
		content = fmk.read()
	fmk.close()
	content = re.sub('all: ([a-z ]+)', repl_all, content)
	with open('src/makefile', 'w+') as fmk:
		fmk.write(content)
	fmk.close()

def build_tests():
	#-- generate src/tests/tests_table.inc
	with open(tests_table_inc, 'w+') as ftest:
		ftest.write("//Don't edit! shell.inc: auto-generated by configure.py\n\n")
		for desc, inc, status, test in tests:
			if inc == True:
				ftest.write("{\"" + desc + "\", " + test + "},\n")
			else:
				ftest.write("{\"" + desc + "\", NULL},\n")
		ftest.write("{\"empty\", NULL}\n")
	ftest.close()

def build_drivers():
	pass

#-----------------------------GUI------------------------------
def About():
	view_window = Tkinter.Toplevel(root)
	about_text = "Monitor configurator\nAuthor: Nikolay Korotky\n2009"
	Tkinter.Label(view_window,  text=about_text).pack()
	Tkinter.Button(view_window, text='OK', command=view_window.destroy).pack()
        view_window.focus_set()
        view_window.grab_set()
        view_window.wait_window()

def file_menu():
	file_btn = Tkinter.Menubutton(menu_frame, text='File', underline=0)
	file_btn.pack(side=Tkinter.LEFT, padx="2m")
	file_btn.menu = Tkinter.Menu(file_btn)
	file_btn.menu.add_command(label="Save", underline=0, command=make_conf)
	file_btn.menu.add_command(label="Load default", underline=0, command=reload_config("config.default"))
	file_btn.menu.add('separator')
	file_btn.menu.add_command(label='Exit', underline=0, command=file_btn.quit)
	file_btn['menu'] = file_btn.menu
	return file_btn

def help_menu():
	help_btn = Tkinter.Menubutton(menu_frame, text='Help', underline=0,)
	help_btn.pack(side=Tkinter.LEFT, padx="2m")
	help_btn.menu = Tkinter.Menu(help_btn)
	help_btn.menu.add_command(label="About", underline=0, command=About)
	help_btn['menu'] = help_btn.menu
	return help_btn

def main():
	global root, info_line, menu_frame
	root = Tkinter.Tk()
	root.title('Monitor configure')

	#-- Create the menu frame, and add menus to the menu frame
	menu_frame = Tkinter.Frame(root)
	menu_frame.pack(fill=Tkinter.X, side=Tkinter.TOP)
	menu_frame.tk_menuBar(file_menu(), help_menu())

	#-- Create the info frame and fill with initial contents
	info_frame = Tkinter.Frame(root)
	info_frame.pack(fill=Tkinter.X, side=Tkinter.BOTTOM, pady=1)

	#-- Tabs frame
	main_frame = monitor_tab(info_frame, LEFT)

	#-- Common frame
	common_frame = Tkinter.Frame(main_frame())
	Label(common_frame, text="Monitor Configurator", width=25, background="green").grid(row=0, column=0)
	Label(common_frame, text="", width=35, background="green").grid(row=0, column=1)
	Label(common_frame, text="Программа предназначенная для началь-", width=35).grid(row=1, column=1)
	Label(common_frame, text="ной инициализации и тестирования ап-", width=35).grid(row=2, column=1)
	Label(common_frame, text="паратуры. А так же для ее отладки. А", width=35).grid(row=3, column=1)
	Label(common_frame, text="так же для отладки системного кода для", width=35).grid(row=4, column=1)
	Label(common_frame, text=" дальнейшего переноса кода в Линукс ", width=35).grid(row=5, column=1)
	#-- arch
	global arch_num_var
	arch_num_var = IntVar()
	Label(common_frame, text="Arch", width=25, background="lightblue").grid(row=1, column=0)
	for ar, value in arch:
		Radiobutton(common_frame, text=ar, value=value, variable=arch_num_var, anchor=W).grid(row=value+2, column=0, sticky=W)
	arch_num_var.set(arch_num)
	#-- Compiler
	global compiler_var
	Label(common_frame, text="Compiler", width=25, background="lightblue").grid(row=5, column=0)
	compiler_var = StringVar()
	Entry(common_frame, width=25, textvariable=compiler_var).grid(row=6, column=0)
	compiler_var.set(compiler)
	#-- LDFLAGS
	global ldflags_var
	Label(common_frame, text="LDFLAGS", width=25, background="lightblue").grid(row=7, column=0)
	ldflags_var = StringVar()
	Entry(common_frame, width=25, textvariable=ldflags_var).grid(row=8, column=0)
	ldflags_var.set(ldflags)
	#-- CFLAGS
	global cflags_var
	Label(common_frame, text="CFLAGS", width=25, background="lightblue").grid(row=9, column=0)
	cflags_var = StringVar()
	Entry(common_frame, width=25, textvariable=cflags_var).grid(row=10, column=0)
	cflags_var.set(cflags)
	#-- Target
	global target_var
	Label(common_frame, text="Target", width=25, background="lightblue").grid(row=11, column=0)
	target_var = StringVar()
	Entry(common_frame, width=25, textvariable=target_var).grid(row=12, column=0)
	target_var.set(target)

	#-- Drivers frame
	drivers_frame = Tkinter.Frame(main_frame())
	Label(drivers_frame, text="Driver", width=25, background="lightblue").grid(row=0, column=0)
	Label(drivers_frame, text="Description", width=35, background="lightblue").grid(row=0, column=1)
	vard = IntVar()
	row = 1
	for driver, inc, status, desc in drivers:
		setattr(vard, driver, IntVar())
		Checkbutton(drivers_frame, text=driver, state=getStatus(status), anchor=W,
			variable = getattr(vard, driver), command=(lambda row=row: onPress(drivers, row-1, 1))).grid(row=row, column=0, sticky=W)
		getattr(vard, driver).set(inc)
		Label(drivers_frame, text=desc, state=getStatus(status), width=35, anchor=W).grid(row=row, column=1, sticky=W)
		row = row + 1

	#-- Tests frame
	test_frame = Tkinter.Frame(main_frame())
	Label(test_frame, text="Start testing", width=25, background="lightblue").grid(row=0, column=0)
	Label(test_frame, text="Description", width=35, background="lightblue").grid(row=0, column=1)
	vart = IntVar()
	row = 1
	for desc, inc, status, test_name in tests:
		setattr(vart, test_name, IntVar())
		Checkbutton(test_frame, text=test_name, state=getStatus(status), anchor=W,
			variable = getattr(vart, test_name), command=(lambda row=row: onPress(tests, row-1, 1))).grid(row=row, column=0, sticky=W)
		getattr(vart, test_name).set(inc)
		Label(test_frame, text=desc, state=getStatus(status), width=35, anchor=W).grid(row=row, column=1, sticky=W)
		row = row + 1

	#-- Commands shell frame
	command_frame = Tkinter.Frame(main_frame())
	Label(command_frame, text="Shell commands", width=25, background="lightblue").grid(row=0, column=0)
        Label(command_frame, text="Description", width=35, background="lightblue").grid(row=0, column=1)
        varc = IntVar()
	row = 1
	for cmd, pack, inc, status, desc in commands:
		setattr(varc, cmd, IntVar())
    		Checkbutton(command_frame, text=cmd, state=getStatus(status), anchor=W,
			variable = getattr(varc, cmd), command=(lambda row=row: onPress(commands, row-1, 2))).grid(row=row, column=0, sticky=W)
		getattr(varc, cmd).set(inc)
		Label(command_frame, text=desc, state=getStatus(status), width=35, anchor=W).grid(row=row, column=1, sticky=W)
		row = row + 1

	#-- Level frame
	global error_level_var, trace_level_var, warn_level_var, debug_level_var, test_system_level_var, leon3_level_var
	level_frame = Tkinter.Frame(main_frame())
	Label(level_frame, text="Verbous level", width=25, background="lightblue").grid(row=0, column=0)
	Label(level_frame, text="", width=35).grid(row=0, column=1)
	error_level_var, trace_level_var, warn_level_var, debug_level_var, test_system_level_var, leon3_level_var = (IntVar(), IntVar(), IntVar(), IntVar(), IntVar(), IntVar())
	Checkbutton(level_frame, text="Error", state=NORMAL, anchor=W, variable = error_level_var).grid(row=1, column=0, sticky=W)
	error_level_var.set(error_level)
	Checkbutton(level_frame, text="Trace", state=NORMAL, anchor=W, variable = trace_level_var).grid(row=2, column=0, sticky=W)
	trace_level_var.set(trace_level)
	Checkbutton(level_frame, text="Warn", state=NORMAL, anchor=W, variable = warn_level_var).grid(row=3, column=0, sticky=W)
	warn_level_var.set(warn_level)
	Checkbutton(level_frame, text="Debug", state=NORMAL, anchor=W, variable = debug_level_var).grid(row=4, column=0, sticky=W)
	debug_level_var.set(debug_level)
	Checkbutton(level_frame, text="Test system", state=NORMAL, anchor=W, variable = test_system_level_var).grid(row=5, column=0, sticky=W)
	test_system_level_var.set(test_system_level)
	Checkbutton(level_frame, text="Leon3", state=NORMAL, anchor=W, variable = leon3_level_var).grid(row=6, column=0, sticky=W)
	leon3_level_var.set(leon3_level)

	#-- Build frame
	global debug_var, release_var, simulation_var, docs_var, dis_var
	build_frame = Tkinter.Frame(main_frame())
	Label(build_frame, text="Build", width=25, background="lightblue").grid(row=0, column=0)
	Label(build_frame, text="", width=35).grid(row=0, column=1)
	debug_var, release_var, simulation_var, docs_var, dis_var = (IntVar(), IntVar(), IntVar(), IntVar(), IntVar())
	Checkbutton(build_frame, text="Debug", state=NORMAL, anchor=W, variable = debug_var).grid(row=1, column=0, sticky=W)
	debug_var.set(debug)
	Checkbutton(build_frame, text="Release", state=NORMAL, anchor=W, variable = release_var).grid(row=2, column=0, sticky=W)
	release_var.set(release)
	Checkbutton(build_frame, text="Simulation", state=NORMAL, anchor=W, variable = simulation_var).grid(row=3, column=0, sticky=W)
	simulation_var.set(simulation)
	Checkbutton(build_frame, text="Disassemble", state=NORMAL, anchor=W, variable = dis_var).grid(row=4, column=0, sticky=W)
	dis_var.set(dis)
	Checkbutton(build_frame, text="Doxygen", state=NORMAL, anchor=W, variable = docs_var).grid(row=5, column=0, sticky=W)
	docs_var.set(docs)

	#-- build tabs
	main_frame.add_screen(common_frame, "Common")
	main_frame.add_screen(drivers_frame, "Drivers")
	main_frame.add_screen(test_frame, "Tests")
	main_frame.add_screen(command_frame, "Commands")
	main_frame.add_screen(level_frame, "Levels")
	main_frame.add_screen(build_frame, "Build")

	root.mainloop()

if __name__=='__main__':
	try:
		read_config("config")
		shutil.copyfile("config", "config.old")
    		main()
	except:
    		traceback.print_exc()
