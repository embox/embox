# -*- coding: utf-8 -*-

import copy, pickle, os

import mcmsg, mcerrors, mcmodinfo
from mcscan import ScanAndParse


#################################################
####### classes (and structs) ###################


class PreprocDefnInfoStruct_t:
	""" class representing single preprocessor definition information """
	def __init__(self, Value, isEnabled):
		self.Value = Value          # definition value (can be empty string)
		self.isEnabled = isEnabled  # wether this definition will be passed to compiler (as argument)



class CompilerSettings_t:
	""" CompilerSettings_t is a class, which holds settings for specific compiler:
	    Path, CFLAGS, LDFLAGS """
	def __init__(self, Path, CFLAGS, LDFLAGS):
		self.Path = Path
		self.CFLAGS = CFLAGS
		self.LDFLAGS = LDFLAGS

	def SafeInit(self, InInstance):
		ClobberedFields = []

		try:
			if isinstance(InInstance.Path, unicode):
				self.Path = InInstance.Path
			else:
				ClobberedFields.append('Path (was '+type(InInstance.Path).__name__+')')
		except AttributeError:
			ClobberedFields.append('Path (missing)')

		try:
			if isinstance(InInstance.CFLAGS, unicode):
				self.CFLAGS = InInstance.CFLAGS
			else:
				ClobberedFields.append('CFLAGS (was '+type(InInstance.CFLAGS).__name__+')')
		except AttributeError:
			ClobberedFields.append('CFLAGS (missing)')

		try:
			if isinstance(InInstance.LDFLAGS, unicode):
				self.LDFLAGS = InInstance.LDFLAGS
			else:
				ClobberedFields.append('LDFLAGS (was '+type(InInstance.LDFLAGS).__name__+')')
		except AttributeError:
			ClobberedFields.append('LDFLAGS (missing)')

		return ClobberedFields


class ArchSettings_t:
	""" ArchSettings_t is a class, which holds build settings for specific architecture """
	def __init__(self,ExcludedModules):

		# we need a deepcopy of defaults, because each arch must have it's own compiler settings, not a link to one place
		self.CompilersToSettingsDict = copy.deepcopy(defCompilersToSettingsDict)

		self.CurrentCompilerName = self.CompilersToSettingsDict.keys()[0]

		self.ExcludedModules = ExcludedModules


	def SafeInit(self, InInstance):
		ClobberedFields = []

		try:
			if InInstance.CurrentCompilerName in defCompilersToSettingsDict:
				self.CurrentCompilerName = InInstance.CurrentCompilerName
			else:
				ClobberedFields.append('CurrentCompilerName')
		except AttributeError:
			ClobberedFields.append('CurrentCompilerName')

		try:
			if isinstance(InInstance.CompilersToSettingsDict, dict):
				for Compiler in defCompilersToSettingsDict:
					# for each compiler in defaults there must be a corresponding settings in InInstance
					if Compiler in InInstance.CompilersToSettingsDict:
						# good. compiler info present in InInstance, we should try to init our settings with it
						ClobberedSettings = self.CompilersToSettingsDict[Compiler].SafeInit(InInstance.CompilersToSettingsDict[Compiler])
						for ClSe in ClobberedSettings:
							ClobberedFields.append('CompilersToSettingsDict['+Compiler+'].'+ClSe)
					else:
						# there is no settings for this compiler in InInstance, so leave default settings, but indicate an error
						ClobberedFields.append('CompilersToSettingsDict['+Compiler+']')
			else:
				# there is no CompilersToSettingsDict in InInstance at all! so leave default settings, but indicate an error
				ClobberedFields.append('CompilersToSettingsDict')
		except AttributeError:
			ClobberedFields.append('CompilersToSettingsDict')

		return ClobberedFields



class MonitorPreset_t:
	""" MonitorPreset_t is a class, which holds configuration info for specific preset
	    Each preset holds several architecture settings - one for each known arch """
	def __init__(self, ArchsToExcludedModsDict, ModulesDict):

		# list of the known linkscripts is global and constant (constLinkScripts), so we don't need a deepcopy
		self.CurrentLinkScript = constLinkScripts[0]

		# we need a deepcopy of defaults, because each preset must have it's own preprocdefns, not a link to one place
		self.PreprocDefnsDict = copy.deepcopy(defPreprocDefnsDict)

		# StagedModulesDict indicates, whether specific module will be built into Monitor image
	   # it is a dict(aka map) { str : bool }, which associates module name with staged flag
		self.StagedModulesDict = {}

		for ModName in ModulesDict:
			if ModName in ModulesDict.KnownPersistents:
				self.StagedModulesDict[ModName] = True
			else:
				self.StagedModulesDict[ModName] = False

		self.ArchsToArchSettingsDict = {}

		for Arch in ArchsToExcludedModsDict:
			self.ArchsToArchSettingsDict[Arch] = ArchSettings_t(ArchsToExcludedModsDict[Arch])

		self.CurrentArchName = ModulesDict.KnownArchs[0]

		self.StagedModulesDict[self.CurrentArchName] = True



	def EditPreprocDef(self, DefStr, Val, isEnabled):
		self.PreprocDefnsDict[DefStr] = PreprocDefnInfoStruct_t(Val, isEnabled)



	def SafeInitStagedMods(self, InInstance, ModulesDict):
		Retval = True

		for ModName in self.StagedModulesDict:
			if ModName in InInstance:
				if InInstance[ModName]==True:
					try:
						ReqsToStage = self.CheckReqsToStage(ModName, ModulesDict)
					except mcerrors.StageErr as Err:
						print Err
					else:
						#print 'stage ', ModName
						self.StagedModulesDict[ModName] = True
						for CurReqModName in ReqsToStage:
							self.StagedModulesDict[CurReqModName] = True
				else:
					try:
						if ModName == self.CurrentArchName:
							raise mcerrors.UnstageCurArchErr(ModName)

						DepsToUnstage = self.CheckDepsToUnstage(ModName, ModulesDict)
					except mcerrors.StageErr as Err:
						print Err
					else:
						#print 'unstage ', ModName
						self.StagedModulesDict[ModName] = False
						for CurDepModName in DepsToUnstage:
							self.StagedModulesDict[CurDepModName] = False
			else:
				#print 'no module', ModName
				Retval = False

		return Retval


	def SafeInit(self, InInstance, ModulesDict):
		ClobberedFields = []

		try:
			if isinstance(InInstance.CurrentArchName, unicode):
				if InInstance.CurrentArchName in ModulesDict.KnownArchs:
					self.CurrentArchName = InInstance.CurrentArchName
			else:
				ClobberedFields.append('CurrentArchName (was '+type(InInstance.CurrentArchName).__name__+')')
		except AttributeError:
			ClobberedFields.append('CurrentArchName (missing)')

		try:
			if isinstance(InInstance.StagedModulesDict, dict):
				if self.SafeInitStagedMods(InInstance.StagedModulesDict, ModulesDict) == False:
					ClobberedFields.append('StagedModulesDict[some modules]')
			else:
				ClobberedFields.append('StagedModulesDict')
		except AttributeError:
			ClobberedFields.append('StagedModulesDict')

		try:
			if isinstance(InInstance.PreprocDefnsDict, dict):
				self.PreprocDefnsDict = InInstance.PreprocDefnsDict # TODO it is unsafe if ppdef format will be changed
			else:
				ClobberedFields.append('PreprocDefnsDict')
		except AttributeError:
			ClobberedFields.append('PreprocDefnsDict')

		try:
			if InInstance.CurrentLinkScript in constLinkScripts:
				self.CurrentLinkScript = InInstance.CurrentLinkScript
			else:
				ClobberedFields.append('CurrentLinkScript (was '+InInstance.CurrentLinkScript+')')
		except AttributeError:
			ClobberedFields.append('CurrentLinkScript (missing)')

		try:
			if isinstance(InInstance.ArchsToArchSettingsDict, dict):
				for Arch in InInstance.ArchsToArchSettingsDict:
					if Arch in ModulesDict.KnownArchs:
						ClobberedAS = self.ArchsToArchSettingsDict[Arch].SafeInit(InInstance.ArchsToArchSettingsDict[Arch])
						for CAS in ClobberedAS:
							ClobberedFields.append('ArchsToArchSettingsDict['+Arch+'].'+CAS)
					else:
						ClobberedFields.append('ArchsToArchSettingsDict['+Arch+']')
			else:
				ClobberedFields.append('ArchsToArchSettingsDict')
		except AttributeError:
			ClobberedFields.append('ArchsToArchSettingsDict')

		return ClobberedFields


	def CheckDepsToUnstage(self, ModName, ModulesDict):
		DepsToUnstage = []
		for DependantName in ModulesDict.GetAllModDependants(ModName):
			if DependantName in ModulesDict.KnownPersistents:
				raise mcerrors.UnstagePersistentErr(ModName, DependantName)
			elif self.StagedModulesDict[DependantName]:
				DepsToUnstage.append(DependantName)

		return DepsToUnstage


	def SafeUnstageModule(self, ModName, ModulesDict):
		try:
			if ModName == self.CurrentArchName:
				raise mcerrors.UnstageCurArchErr(ModName)
			DepsToUnstage = self.CheckDepsToUnstage(ModName, ModulesDict)
		except mcerrors.StageErr as StageErr:
			mcmsg.WarnMsgBox(str(StageErr))
			return []

		if len(DepsToUnstage) == 0:
			self.StagedModulesDict[ModName] = False
			return [ModName]

		return self.AskUnstageModWithDeps(ModName, DepsToUnstage, ModulesDict)


	def AskUnstageModWithDeps(self, ModName, Dependants, ModulesDict):
		ModsUnstaged = []

		msg  = "These modules depend on module <b>"+ModName+"</b>:<br> "
		for Dep in Dependants:
			msg += "<b>"+Dep+"</b>("+ModulesDict[Dep].Type+")<br> "
		msg += "<br>They will also be disabled, if you really want to disable <b>"+ModName+"</b>"

		if mcmsg.QuestionMsgBox(msg):
			self.StagedModulesDict[ModName] = False
			for CurDepModName in Dependants:
				self.StagedModulesDict[CurDepModName] = False

			ModsUnstaged = Dependants
			ModsUnstaged.append(ModName)

		return ModsUnstaged

	def CheckReqsToStage(self, ModName, ModulesDict):
		CurArch = self.ArchsToArchSettingsDict[self.CurrentArchName]
		if ModName in CurArch.ExcludedModules:
			raise mcerrors.StageModExcludedByArchErr(ModName, self.CurrentArchName)

		ReqsToStage = []
		for ReqModName in ModulesDict.GetAllModRequirements(ModName):
			if ReqModName in CurArch.ExcludedModules:
				raise mcerrors.StageReqExcludedByArchErr(ModName, ReqModName, self.CurrentArchName)
			elif not self.StagedModulesDict[ReqModName]:
				ReqsToStage.append(ReqModName)

		return ReqsToStage


	def SafeStageModule(self, ModName, ModulesDict):
		try:
			ReqsToStage = self.CheckReqsToStage(ModName, ModulesDict)
		except mcerrors.StageErr as StageErr:
			mcmsg.WarnMsgBox(str(StageErr))
			return []

		if len(ReqsToStage) == 0:
			self.StagedModulesDict[ModName] = True
			return [ModName]

		msg  = "These modules are required for module <b>"+ModName+"</b>:<br> "
		for Req in ReqsToStage:
			msg += "<b>"+Req+"</b>("+ModulesDict[Req].Type+")<br> "
		msg += "<br>They will also be enabled, if you really want to enable <b>"+ModName+"</b>"

		if mcmsg.QuestionMsgBox(msg):
			self.StagedModulesDict[ModName] = True
			for CurReqModName in ReqsToStage:
				self.StagedModulesDict[CurReqModName] = True

			ReqsToStage.append(ModName)
			return ReqsToStage

		return []


	def AskChangeArch(self, NewArchName, ModulesDict):
		if NewArchName not in ModulesDict:
			mcmsg.WarnMsgBox('CONSISTENCY ERROR: ther is no arch '+NewArchName+' in ModulesDict')
			return False
		if NewArchName not in self.ArchsToArchSettingsDict:
			mcmsg.WarnMsgBox('CONSISTENCY ERROR: ther is no arch '+NewArchName+' in ArchsToArchSettingsDict')
			return False

		try:
			DepsToUnstage = self.CheckDepsToUnstage(self.CurrentArchName, ModulesDict)
		except mcerrors.StageErr as StageErr:
			mcmsg.WarnMsgBox(str(StageErr))
			return False

		if len(DepsToUnstage) > 0:
			if len(self.AskUnstageModWithDeps(self.CurrentArchName, DepsToUnstage, ModulesDict)) == 0:
				return False

		self.StagedModulesDict[self.CurrentArchName] = False
		self.StagedModulesDict[NewArchName] = True
		self.CurrentArchName = NewArchName

		return True



class MonitorConfig_t:
	""" MonitorConfig_t is a class, which holds few presets with configuration info """
	def __init__(self):

		global gCfgDumpFileName,  defSrcDir
		gCfgDumpFileName = os.path.abspath(gCfgDumpFileName)

		if os.path.exists(gCfgDumpFileName):
			CfgDumpFlie = open(gCfgDumpFileName, 'r+')
			try:
				OldConfig = pickle.load(CfgDumpFlie)
			except:
				(Etype, Err, Trback) = sys.exc_info()
				mcmsg.AskForIgnoreMsgBox("Configuration file malformed.\n"+str(Err)+"\n\nIgnore to create new empty config")
				self.DefaultInit(defSrcDir)
			else:
				self.SafeInit(OldConfig)
				del OldConfig
			CfgDumpFlie.close()
		else:
			mcmsg.AskForIgnoreMsgBox("No configuration file ("+gCfgDumpFileName+")\n\nIgnore to create new empty config")
			self.DefaultInit(defSrcDir)


	def DefaultInit(self, SrcDir):

		self.SrcDir = SrcDir
		global gModulesDict,  defOutDir
		gModulesDict = ScanAndParse(os.path.abspath(SrcDir))

		self.OutDir = defOutDir

		ArchsToDependantsDict = {}

		for CurArch in gModulesDict.KnownArchs:
			ArchsToDependantsDict[CurArch] = []
			for DependantName in gModulesDict.GetAllModDependants(CurArch):
				ArchsToDependantsDict[CurArch].append(DependantName)

		ArchsToDisabledModsDict = {}

		for CurArch in gModulesDict.KnownArchs:
			ArchsToDisabledModsDict[CurArch] = []
			for OtherArch in gModulesDict.KnownArchs:
				if not OtherArch == CurArch:
					ArchsToDisabledModsDict[CurArch].extend(ArchsToDependantsDict[OtherArch])
					ArchsToDisabledModsDict[CurArch].append(OtherArch)

		self.PresetsDict = {
			'simulation' : MonitorPreset_t ( ArchsToDisabledModsDict, gModulesDict ),
			'hardware' : MonitorPreset_t ( ArchsToDisabledModsDict, gModulesDict )
		}

		self.CurPresetName = self.PresetsDict.keys()[0]


	def SafeInit(self, InInstance):

		global defSrcDir, defOutDir

		try:
			if isinstance(InInstance.SrcDir, unicode):
				gModulesDict = ScanAndParse(os.path.abspath(InInstance.SrcDir))
				self.DefaultInit(InInstance.SrcDir)
			else:
				mcmsg.WarnMsgBox('Default src dir ('+defSrcDir+') will be scanned')
				self.DefaultInit(defSrcDir)
		except AttributeError:
			mcmsg.WarnMsgBox('Default src dir ('+defSrcDir+') will be scanned')
			self.DefaultInit(defSrcDir)

		try:
			if isinstance(InInstance.OutDir, unicode):
				self.OutDir = InInstance.OutDir
			else:
				mcmsg.WarnMsgBox('Out dir is clobbered in prevoiusly stored config, leaving default ('+defOutDir+')')
		except AttributeError:
			mcmsg.WarnMsgBox('Out dir is missing in prevoiusly stored config, leaving default ('+defOutDir+')')

		try:
			if isinstance(InInstance.PresetsDict, dict):
				for PresetName in self.PresetsDict:
					if PresetName in InInstance.PresetsDict:
						ClobberedFields = self.PresetsDict[PresetName].SafeInit(InInstance.PresetsDict[PresetName], gModulesDict)
						if len(ClobberedFields)>0:
							warnmsg = 'Preset '+PresetName+' from previously stored configuration has following fields clobbered:\n'
							for CF in ClobberedFields:
								warnmsg += CF+'; '
							mcmsg.WarnMsgBox(warnmsg)
					else:
						mcmsg.WarnMsgBox('Preset '+PresetName+' is missing in previously stored configuration')
			else:
				mcmsg.WarnMsgBox('Previously stored configuration has malformed PresetsDict (not a dict)')
		except AttributeError:
			mcmsg.WarnMsgBox('Previously stored configuration has no PresetsDict')

		try:
			if InInstance.CurPresetName in self.PresetsDict:
				self.CurPresetName = InInstance.CurPresetName
		except AttributeError:
			mcmsg.WarnMsgBox('Previously stored configuration has no CurPresetName')


#################################################
####### defaults and hardcoded constants ########

defCompilersToSettingsDict = {
	'sparc-linux' :  CompilerSettings_t(
		Path = u'/opt/sparc-linux/bin/sparc-linux-gcc',
		CFLAGS = u'-Werror -msoft-float -c -MD -mv8 -O0',
		LDFLAGS = u'-Wl -N -nostdlib'
	),
	'sparc-elf' : CompilerSettings_t(
		Path = u'/opt/sparc-elf/bin/sparc-elf-gcc',
		CFLAGS = u'-Werror -msoft-float -c -MD -mv8 -O0',
		LDFLAGS = u'-Wl -N -nostdlib'
	)
}

# PreprocDefnsDict holds info about all preprocessor definitions
# it is a dict(aka map) { str : PreprocDefnInfoStruct_t }, which associates definition sring with info
defPreprocDefnsDict = {
	'_TEST_SYSTEM_' : PreprocDefnInfoStruct_t(Value='', isEnabled=True),
	'_ERROR' : PreprocDefnInfoStruct_t(Value='', isEnabled=True),
	'_WARN' : PreprocDefnInfoStruct_t(Value='', isEnabled=True),
	'_DEBUG' : PreprocDefnInfoStruct_t(Value='', isEnabled=False)
}

defSrcDir = u'.'#'../src'

defOutDir = u'../bin'

constLinkScripts = [u'linkrom',u'linkram',u'linksim']




gCfgDumpFileName = u'.config2'

gConfig = None # must be MonitorConfig_t()

gModulesDict = mcmodinfo.ModulesDict_t()



