# -*- coding: utf-8 -*-

import copy

class DepsErr(Exception):
	pass


class RecursiveDepsErr(DepsErr):
	def __str__(self):
		(DepStack,) = self.args
		errmsg = "ERROR: Recursive dependency somewhere in modules:\n "
		for CurModName in DepStack:
			errmsg += CurModName+"\n "
		errmsg += "Ignore to disable them all"
		return errmsg


class MissingDepsErr(DepsErr):
	def __str__(self):
		(DepStack, MissingModName) = copy.deepcopy(self.args)
		errmsg = "ERROR: Missing module "+MissingModName+" required by:\n "
		errmsg += DepStack.pop()
		if len(DepStack) > 0:
			errmsg += " and it's dependants("
			errmsg += DepStack.pop()
			for ModName in DepStack:
				errmsg += ", "+ModName
			errmsg += ")"
		errmsg += "\nIgnore to disable them all"
		return errmsg



class StageErr(Exception):
	pass


class UnstagePersistentErr(StageErr):
	def __str__(self):
		(ModName, Persistent) = self.args
		return "Module "+ModName+" is required by persistent module "+Persistent+", which can't be unstaged"


class UnstageCurArchErr(StageErr):
	def __str__(self):
		(ModName,) = self.args
		errmsg =  "Module "+ModName+" is current arch module, which can't be unstaged\n"
		errmsg += "use AskChangeArch() (or select proper arch in global config in gui) instead"
		return errmsg


class StageReqExcludedByArchErr(StageErr):
	def __str__(self):
		(ModName, BlockerName, CurAch) = self.args
		errmsg = "Module "+ModName+" depends on module "+BlockerName
		errmsg += ", which is disabled by current arch("+CurAch+"), and can't be staged\n"
		errmsg += "select proper arch in global config to enable this module"
		return errmsg


class StageModExcludedByArchErr(StageErr):
	def __str__(self):
		(ModName, CurAch) = self.args
		errmsg = "Module "+ModName+" is disabled by current arch("+CurAch+"), and can't be staged"
		return errmsg



class ScanErr(Exception):
	pass


class MissSeparatorErr(ScanErr):
	def __str__(self):
		(Path, LineNum, LineText) = self.args
		errmsg = "ERROR: Malformed module description file:\n "
		errmsg += Path+"\n "
		errmsg += "missing separator (\"=\") on line "+str(LineNum)+":\n "
		errmsg += "line text: \""+LineText+"\""
		return errmsg


class InvalidKeyErr(ScanErr):
	def __str__(self):
		(Path, LineNum, LineText, Key) = self.args
		errmsg = "ERROR: Malformed module description file:\n "
		errmsg += Path+"\n "
		errmsg += " invalid key \""+Key+"\" on line "+str(LineNum)+":\n "
		errmsg += "line text: \""+LineText+"\"\n"
		errmsg += "Possible keys are: ModName, InitProc, Description, Type, DependsOn, SrcList"
		return errmsg


class MissReqFieldErr(ScanErr):
	def __str__(self):
		(Path, Field) = self.args
		errmsg = "ERROR: Malformed module description file:\n "
		errmsg += Path+"\n "
		errmsg += "required field "+Field+" is not specified"
		return errmsg


class DupFieldErr(ScanErr):
	def __str__(self):
		(Path, Field) = self.args
		errmsg = "ERROR: Malformed module description file:\n "
		errmsg += Path+"\n "
		errmsg += Field+" specified twice. It is not allowed."
		return errmsg


class ModulesDuplicatedErr(ScanErr):
	def __str__(self):
		(ModInfo1, ModInfo2) = self.args
		errmsg = "ERROR: found two modules with the same name \""+ModInfo1.ModName+"\":\n "
		errmsg += "1) "+ModInfo1.MFilePath+":\n  "
		errmsg += "Type: "+ModInfo1.Type+":\n  "
		errmsg += "Description: "+ModInfo1.Description+":\n  "
		errmsg += "SrcList: "+str(ModInfo1.SrcList)+":\n "
		errmsg += "2) "+ModInfo2.MFilePath+":\n  "
		errmsg += "Type: "+ModInfo2.Type+":\n  "
		errmsg += "Description: "+ModInfo2.Description+":\n  "
		errmsg += "SrcList: "+str(ModInfo2.SrcList)+":\n"
		errmsg += "Ignore this to use first module"
		return errmsg


