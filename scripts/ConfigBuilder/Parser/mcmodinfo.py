# -*- coding: utf-8 -*-


class ModInfoStruct_t:
	""" class representing single module information """
	def __init__(self, filepath):
		self.MFilePath = filepath # path to ".mmod" file
		self.RequiredByList = []  # list of module names, which depend on this module

		# following info comes from ".mmod" file:
		self.ModName       = ""        # unique name of the module
		self.InitProc      = ""        # name of module-init function in source
		self.Type          = "Other"   # type of module (for example "ShellCommand")
		self.Description   = ""        # description text, will be printed in GUI
		self.DependsOnList = []        # list of module names, which required by this module
		self.SrcList       = []        # list of strings, representing paths to sources of this module
		self.MDef          = ""        # macro definition for autoconf/autoconf.h


class ModulesDict_t(dict):
	''' ModulesDict_t calss holds info about all known modules
	    it acts as a dict(aka map) { str : ModInfoStruct_t }, which associates module name with info '''
	def __init__(self):
		dict.__init__(self)

		self.KnownArchs = []
		self.KnownPersistents = []


	def GetAllModDependants(self, ModName):
		''' yields all depednats names '''
		for DependantName in self[ModName].RequiredByList:
			for SubDependantName in self.GetAllModDependants(DependantName):
				yield SubDependantName
			yield DependantName


	def GetAllModRequirements(self, ModName):
		''' yields all required modules names '''
		for ReqModName in self[ModName].DependsOnList:
			for SubReqModName in self.GetAllModRequirements(ReqModName):
				yield SubReqModName
			yield ReqModName

