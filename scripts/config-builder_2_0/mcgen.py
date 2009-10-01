# -*- coding: utf-8 -*-

import string, re, codecs, os, shutil

import mcglobals

class ConfigGenerator:
	def __init__(self, gConfig):
		self.gConfig = gConfig

	def generate(self):
		for file in ("scripts/autoconf", "scripts/autoconf.h"):
		        if not os.path.exists(file):
		                shutil.copyfile(file + ".in", file)
		self.genLinkScript()
		self.genAutoconf()
		self.genAutoconfh()

	def genLinkScript(self):
		""" Edit link script """
		file = self.gConfig.PresetsDict[self.gConfig.CurPresetName].CurrentLinkScript
		arch = self.gConfig.PresetsDict[self.gConfig.CurPresetName].CurrentArchName
		content = self.read_file('scripts/' + file)
		content = re.sub('OUTPUT_ARCH\((\w+)\)', "OUTPUT_ARCH({0})".format(arch), content)
		self.write_file('scripts/' + file, content)
		os.remove("include/asm")
		os.symlink("asm-{0}".format(arch), "include/asm")

	def genAutoconf(self):
		""" Generate autoconf """
		#-- read autoconf
		content = self.read_file('scripts/autoconf')
		CurPreset = mcglobals.gConfig.PresetsDict[mcglobals.gConfig.CurPresetName]
		CurArch = CurPreset.ArchsToArchSettingsDict[CurPreset.CurrentArchName]
		CurCompilerSettings = CurArch.CompilersToSettingsDict[CurArch.CurrentCompilerName]
		#-- Arch ------------------------------------
		for item in mcglobals.gModulesDict.KnownArchs:
		        mdef    = mcglobals.gModulesDict[item].MDef
		        if CurPreset.CurrentArchName == item:
		    		inc = 1
		    	else:
		    		inc = 0
		        content = self.replacer( mdef, inc, content)
		#-- CC_PACKET, CFLAGS, LDFLAGS --------------
		content = re.sub( "CFLAGS=([A-Za-z0-9_\\-# ]+)",  "CFLAGS="    + CurCompilerSettings.CFLAGS,  content)
		content = re.sub( "LDFLAGS=([A-Za-z0-9_\\-# ]+)", "LDFLAGS="   + CurCompilerSettings.LDFLAGS, content)
		content = re.sub( "CC_PACKET=(\\w+(-\\w+)?)",     "CC_PACKET=" + CurArch.CurrentCompilerName, content)
		#-- Modules ---------------------------------
		for module in mcglobals.gModulesDict.keys():
			mdef    = mcglobals.gModulesDict[module].MDef
			inc     = CurPreset.StagedModulesDict[module]
			content = self.replacer(mdef, inc, content)
		#-- write autoconf
		self.write_file('scripts/autoconf', content)

	def genAutoconfh(self):
		""" Generate autoconf.h """
		#-- read autoconf.h
		content = self.read_file('scripts/autoconf.h')
		#-- Arch ------------------------------------
		CurPreset = mcglobals.gConfig.PresetsDict[mcglobals.gConfig.CurPresetName]
		for item in mcglobals.gModulesDict.KnownArchs:
		        mdef    = mcglobals.gModulesDict[item].MDef
		        if CurPreset.CurrentArchName == item:
		    		inc = 1
		    	else:
		    		inc = 0
		        content = self.replacer_h(mdef, inc, content)
        	#-- Modules ---------------------------------
		for module in mcglobals.gModulesDict.keys():
		        mdef    = mcglobals.gModulesDict[module].MDef
            		inc     = CurPreset.StagedModulesDict[module]
            		print "\""+mdef+"\""
            		content = self.replacer_h(mdef, inc, content)
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

	def replacer_h(self, mdef, inc, content):
		if mdef == "":
			return content
	        mask = '#([undefi]{5,6})([ \t]+)' + mdef + '([ \t]*)[1]?'
	        if inc == True:
	                content = re.sub(mask, '#define {0} 1'.format(mdef), content)
	        else:
	                content = re.sub(mask, '#undef {0}'.format(mdef), content)
	        return content
