# -*- coding: utf-8 -*-

import os, re, codecs

from mcmsg import AskForIgnoreMsgBox
import mcmodinfo, mcerrors


def CheckDepTree(ModInfo, ModulesDict, DepStack):
	#print ModInfo.ModName
	#print DepStack
	if ModInfo.ModName in DepStack:
		raise mcerrors.RecursiveDepsErr(DepStack)
	else:
		DepStack.append(ModInfo.ModName)

	for ReqModName in ModInfo.DependsOnList:
		if ReqModName in ModulesDict:
			CheckDepTree(ModulesDict[ReqModName], ModulesDict, DepStack)
		else:
			print DepStack
			raise mcerrors.MissingDepsErr(DepStack, ReqModName)
	DepStack.pop()


def ParseModFile(ModFileFullPath):
	""" Parse file and return ModInfoStruct_t """
	CurModInfo = mcmodinfo.ModInfoStruct_t(ModFileFullPath)

	ModFile = codecs.open(ModFileFullPath, 'r+', "utf-8");

	AlreadySpecifiedKeys = []
	CurLineNum = 0
	for MFLine in ModFile:
		# parse line
		CurLineNum += 1

		# strip line from comments and whitespace
		(Key, Sep, Val) = MFLine.partition('#')
		MFLineStripped = Key.strip();

		# if resulted line is empty, ignore it
		if MFLineStripped == "":
			continue

		# test if line has a valid syntax ("=" separator)
		(Key, Sep, Val) = MFLineStripped.partition('=')
		if (Sep, Val) == ("", ""):
			raise mcerrors.MissSeparatorErr(ModFileFullPath, CurLineNum, MFLineStripped)

		Key = Key.strip() # get rid of whitespaces
		Val = Val.strip()

		if Key in AlreadySpecifiedKeys:
			raise mcerrors.DupFieldErr(ModFileFullPath, Key)
		else:
			AlreadySpecifiedKeys.append(Key)
			# test if Key is one of the valid module info keys
			if   Key == "ModName":
				CurModInfo.ModName       = Val
			elif Key == "InitProc":
				CurModInfo.InitProc      = Val
			elif Key == "Description":
				CurModInfo.Description   = Val
			elif Key == "Type":
				CurModInfo.Type          = Val
			elif Key == "DependsOn":
				CurModInfo.DependsOnList = Val.split(" ")
			elif Key == "SrcList":
				CurModInfo.SrcList       = Val.split(" ")
			elif Key == "MDef":
				CurModInfo.MDef          = Val
			else:
				raise mcerrors.InvalidKeyErr(ModFileFullPath, CurLineNum, MFLineStripped, Key)

	# check wether required fields are specified
	MissingReqFieldStr = ""
	if CurModInfo.ModName == "":
		raise mcerrors.MissReqFieldErr(ModFileFullPath, "ModName")
	if CurModInfo.SrcList == []:
		raise mcerrors.MissReqFieldErr(ModFileFullPath, "SrcList")

	ModFile.close()
	return CurModInfo


def ModFile2Makefile(FilePath, ModInfo):
	if ModInfo.Type == "Net protocol":
		return #TODO:
	with codecs.open(os.path.dirname(FilePath) + "/Makefile", 'w+', "utf-8") as makefile:
    		makefile_template  = "include $(SCRIPTS_DIR)/autoconf\n\n"
		for item in ModInfo.SrcList:
	    		if re.match("(\w+)[.].", item):
	        		makefile_template += "OBJS-$({0}) += {1}\n".format(ModInfo.MDef, re.sub(r'[.].', '.o', item))
	    		else:
	            		makefile_template += "SUBDIRS-$({0}) += {1}\n".format(ModInfo.MDef, item)
	        makefile_template += "\ninclude $(SCRIPTS_DIR)/common.mk\n"
		makefile.write(makefile_template)
	makefile.close()


def ScanAndParse(RootDir):
	""" Scan dirs recursively, find all module info files, and call parse routine
	    organize returned info structures into dict """

	# ModulesDict holds info about all found modules
	# it acts as a dict(aka map) { str : ModInfoStruct_t }, which associates module name with info
	ModulesDict = mcmodinfo.ModulesDict_t()

	for (CurDir, SubDirs, Files) in os.walk(RootDir):
		if '.git' in SubDirs: # don't scan .git dirs
			SubDirs.remove('.git')

		for CurFileName in Files:
			if not re.search('.mmod$', CurFileName):
				continue

			CurFilePath = CurDir+'/'+CurFileName;

			try:
				# Call parse routine
				CurModInfo = ParseModFile(CurFilePath)
				# Generate Makefile
				ModFile2Makefile(CurFilePath, CurModInfo)

				CurModName = CurModInfo.ModName

				if CurModName in ModulesDict:
					raise mcerrors.ModulesDuplicatedErr(ModulesDict[CurModName],CurModInfo)
				else:
					ModulesDict[CurModName] = CurModInfo

			except mcerrors.ScanErr as Err:
				AskForIgnoreMsgBox(str(Err))

	for CurModInfo in ModulesDict.values():
	# for each module check dependencies, and disable them if any errors found
		if CurModInfo.ModName in ModulesDict:
			try:
				DepStack = [] # dependency stack used for detecting circular deps
				CheckDepTree(CurModInfo, ModulesDict, DepStack)
			except mcerrors.DepsErr as Err:
				AskForIgnoreMsgBox(str(Err))
				# DepStack = Err[0]
				print DepStack
				for CurModName in DepStack:
					del ModulesDict[CurModName]

	for CurModInfo in ModulesDict.values():
		# append this modname to the dependants list of each module, on which it depends
		for ReqModName in CurModInfo.DependsOnList:
			if CurModInfo.ModName not in ModulesDict[ReqModName].RequiredByList:
				ModulesDict[ReqModName].RequiredByList.append(CurModInfo.ModName)

		# fill KnownArchs and KnownPersistents lists
		if CurModInfo.Type == 'Arch':
			ModulesDict.KnownArchs.append(CurModInfo.ModName)
		elif CurModInfo.Type == 'Persistent':
			ModulesDict.KnownPersistents.append(CurModInfo.ModName)

	return ModulesDict
