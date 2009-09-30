# -*- coding: utf-8 -*-

import string, re, codecs, os

import mcglobals

class ConfigGenerator:
	def __init__(self, gConfig):
		self.gConfig = gConfig

	def generate(self):
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
		#TODO: ...
		#-- write autoconf
		self.write_file('scripts/autoconf', content)

	def genAutoconfh(self):
		""" Generate autoconf.h """
		#-- read autoconf.h
		content = self.read_file('scripts/autoconf.h')
		#TODO: ...
		#-- write autoconf.h
		self.write_file('scripts/autoconf.h', content)

	def write_file(self, name, content):
    		with codecs.open(name, 'w+', "utf-8") as file:
                        file.write(content)
                file.close()

        def read_file(self, name):
                with codecs.open(name, 'r+', "utf-8") as file:
                	content = file.read()
                file.close()
                return content
