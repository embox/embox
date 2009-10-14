# -*- coding: utf-8 -*-

import string, re, codecs, os, shutil

from ..Parser import mcglobals

class ConfigGenerator:
	def __init__(self):
		self.gConfig = mcglobals.gConfig
		self.CurPreset = self.gConfig.PresetsDict[self.gConfig.CurPresetName]
		self.CurArch = self.CurPreset.ArchsToArchSettingsDict[self.CurPreset.CurrentArchName]
		self.CurCompilerSettings = self.CurArch.CompilersToSettingsDict[self.CurArch.CurrentCompilerName]

	def generate(self):
		for file in ("scripts/autoconf", "scripts/autoconf.h"):
		        if not os.path.exists(file):
		                shutil.copyfile(file + ".in", file)
		self.genLinkScript()
		self.genAutoconf()
		self.genAutoconfh()

	def genLinkScript(self):
		""" Edit link script """
		file = self.CurPreset.CurrentLinkScript
		arch = self.CurPreset.CurrentArchName
		content = self.read_file('scripts/' + file)
		content = re.sub('OUTPUT_ARCH\((\w+)\)', "OUTPUT_ARCH({0})".format(arch), content)
		self.write_file('scripts/' + file, content)
		os.remove("include/asm")
		os.symlink("asm-{0}".format(arch), "include/asm")

	def genAutoconf(self):
		""" Generate autoconf """
		#-- read autoconf
		content = self.read_file('scripts/autoconf')
		#-- CC_PACKET, CFLAGS, LDFLAGS --------------
		content = re.sub( "CFLAGS=([A-Za-z0-9_\\-# ]+)",  "CFLAGS="    + self.CurCompilerSettings.CFLAGS,  content)
		content = re.sub( "LDFLAGS=([A-Za-z0-9_\\-# ]+)", "LDFLAGS="   + self.CurCompilerSettings.LDFLAGS, content)
		content = re.sub( "CC_PACKET=(\\w+(-\\w+)?)",     "CC_PACKET=" + self.CurArch.CurrentCompilerName, content)
		#-- Modules ---------------------------------
		for module in mcglobals.gModulesDict.keys():
			mdef    = mcglobals.gModulesDict[module].MDef
			inc     = self.CurPreset.StagedModulesDict[module]
			content = self.replacer(mdef, inc, content)
		#-- Link Script -----------------------------
		for linkScript in mcglobals.constLinkScripts.keys():
			mdef = mcglobals.constLinkScripts[linkScript]
			if self.CurPreset.CurrentLinkScript == linkScript:
				inc = True
			else:
				inc = False
			content = self.replacer(mdef, inc, content)
		#-- write autoconf
		self.write_file('scripts/autoconf', content)

	def genAutoconfh(self):
		""" Generate autoconf.h """
		#-- read autoconf.h
		content = self.read_file('scripts/autoconf.h')
        	#-- Modules ---------------------------------
		for module in mcglobals.gModulesDict.keys():
		        mdef    = mcglobals.gModulesDict[module].MDef
            		inc     = self.CurPreset.StagedModulesDict[module]
            		content = self.replacer_h(mdef, inc, content)
            	#-- Link Script -----------------------------
            	for linkScript in mcglobals.constLinkScripts.keys():
            	        mdef = mcglobals.constLinkScripts[linkScript]
            	        if self.CurPreset.CurrentLinkScript == linkScript:
            	    		inc = 1
            	        else:
            	                inc = 0
            	        content = self.replacer_h(mdef, inc, content)
            	#-- Preproc defines -------------------------
            	for mdef in self.CurPreset.PreprocDefnsDict.keys():
			val = self.CurPreset.PreprocDefnsDict[mdef].Value
			inc = self.CurPreset.PreprocDefnsDict[mdef].isEnabled
			content = self.replacer_h(mdef, inc, content, val)
		#-- write autoconf.h
		self.write_file('scripts/autoconf.h', content)

	#-------------------------------------------------------------------------------------------

	def write_file(self, name, content):
    		with codecs.open(name, 'w+', "utf-8") as file:
                        file.write(content)
                file.close()

        def read_file(self, name):
                with codecs.open(name, 'r+', "utf-8") as file:
                	content = file.read()
                file.close()
                return content

	def replacer(self, mdef, inc, content):
		if mdef == "":
			return content
	        mask = '{0}(\s*)=(\s*)(\w*)'.format(mdef)
	        if inc == True:
	                content = re.sub(mask, mdef + "=y", content)
	        else:
	                content = re.sub(mask, mdef + "=n", content)
	        return content

	def replacer_h(self, mdef, inc, content, val = "1"):
		if mdef == "":
			return content
	        mask = '#([undefi]{5,6})([ \t]+)' + mdef + '([ \t]*)([a-zA-Z0-9_<>., \"]+)'
	        if inc == True:
	                content = re.sub(mask, '#define {0} {1}'.format(mdef, val), content)
	        else:
	                content = re.sub(mask, '#undef {0}'.format(mdef), content)
	        return content
