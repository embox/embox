#!/usr/bin/env python
# -*- coding: utf-8 -*-
import pickle

from PyQt4 import QtGui, QtCore, uic

from ..Parser import mcglobals
from ..CodeGen import mcgen
from ..Misc import mcerrors
from . import mcmsg
from .mcmodview import ModViewFrame_t
from .mcppdefview import PPDefViewFrame_t


(Ui_MainCfgDlg_t, Ui_MainCfgDlgBase_t) = uic.loadUiType('scripts/ConfigBuilder/QtGui/ui/MainDlg.ui')

class MainCfgDlg_t (Ui_MainCfgDlgBase_t, Ui_MainCfgDlg_t):
	def __init__(self):
		Ui_MainCfgDlgBase_t.__init__(self)

		# Set up the user interface from Designer.
		self.setupUi(self)

		self.Inited = False

		self.SaveAndExitButton.clicked.connect(self.SaveAndExit)
		self.QuitButton.clicked.connect(QtGui.qApp.quit)

		self.ModViewFrame = ModViewFrame_t()
		self.ModViewScrollArea.setWidget(self.ModViewFrame)
		self.ShowHideAllMTButton.clicked.connect(self.ModViewFrame.ToggleAllModsVis)

		self.ModDirLineEdit.setText(mcglobals.gConfig.SrcDir)
		self.OutDirLineEdit.setText(mcglobals.gConfig.OutDir)

		self.PPDefViewFrame = PPDefViewFrame_t()
		self.PPDefsScrollArea.setWidget(self.PPDefViewFrame)
		self.AddPreprocDefButton.clicked.connect(self.PPDefViewFrame.CallAddPreprocDefDlg)

		for LinkscriptName in mcglobals.constLinkScripts.keys():
			self.LinkScriptComBox.addItem(LinkscriptName)
		self.LinkScriptComBox.currentIndexChanged[unicode].connect(self.SelectLinkScript)

		for ArchName in mcglobals.gModulesDict.KnownArchs:
			self.ArchComBox.addItem(ArchName)
		self.ArchComBox.currentIndexChanged[unicode].connect(self.ActivateArchSlot)

		for Compiler in mcglobals.defCompilersToSettingsDict:
			self.CompilerComBox.addItem(Compiler)
		self.CompilerComBox.currentIndexChanged[unicode].connect(self.ActivateCompilerSlot)

		for PresetName in mcglobals.gConfig.PresetsDict:
			self.PresetComBox.addItem(PresetName)
		self.PresetComBox.currentIndexChanged[unicode].connect(self.ActivatePresetSlot)

		CurPresetIndex = self.PresetComBox.findText(mcglobals.gConfig.CurPresetName)
		self.PresetComBox.setCurrentIndex(CurPresetIndex)

		self.ShowCurPreset()

		self.Inited = True


	def SaveCompilerSettings(self):
		if self.Inited:
			CurPreset = mcglobals.gConfig.PresetsDict[mcglobals.gConfig.CurPresetName]
			CurArch = CurPreset.ArchsToArchSettingsDict[CurPreset.CurrentArchName]
			CurCompilerSettings = CurArch.CompilersToSettingsDict[CurArch.CurrentCompilerName]

			#print '   SaveCompilerSettings: ['+mcglobals.gConfig.CurPresetName+']['+CurPreset.CurrentArchName+']['+CurArch.CurrentCompilerName+']'

			CurCompilerSettings.Path = unicode(self.GccPathLineEdit.text())
			CurCompilerSettings.CFLAGS = unicode(self.CFLAGSLineEdit.text())
			CurCompilerSettings.LDFLAGS = unicode(self.LDFLAGSLineEdit.text())


	def ActivateArchSlot(self, NewArchName):
		NewArchName = unicode(NewArchName)
		CurPreset = mcglobals.gConfig.PresetsDict[mcglobals.gConfig.CurPresetName]

		#print 'ActivateArch', NewArchName, '(curarch', CurPreset.CurrentArchName+')'
		if CurPreset.CurrentArchName == NewArchName:
			#print ' ... no need'
			return

		self.SaveCompilerSettings()

		if CurPreset.AskChangeArch(NewArchName, mcglobals.gModulesDict) == False:
			#print ' ... canceled'
			CurArchIndex = self.ArchComBox.findText(CurPreset.CurrentArchName)
			self.ArchComBox.setCurrentIndex(CurArchIndex)
			return

		self.ShowCurPreset()


	def ActivateCompilerSlot(self, NewCompilerName):
		NewCompilerName = unicode(NewCompilerName)
		CurPreset = mcglobals.gConfig.PresetsDict[mcglobals.gConfig.CurPresetName]
		CurArch = CurPreset.ArchsToArchSettingsDict[CurPreset.CurrentArchName]

		if NewCompilerName == CurArch.CurrentCompilerName:
			return

		self.SaveCompilerSettings()

		CurArch.CurrentCompilerName = NewCompilerName

		self.ShowCurPreset()


	def ActivatePresetSlot(self, NewPresetName):
		NewPresetName = unicode(NewPresetName)

		self.SaveCompilerSettings()

		mcglobals.gConfig.CurPresetName = NewPresetName

		self.ShowCurPreset()


	def ShowCurPreset(self):
		CurPreset = mcglobals.gConfig.PresetsDict[mcglobals.gConfig.CurPresetName]
		CurArch = CurPreset.ArchsToArchSettingsDict[CurPreset.CurrentArchName]
		CurCompilerSettings = CurArch.CompilersToSettingsDict[CurArch.CurrentCompilerName]

		# preprocessor definitions
		self.PPDefViewFrame.ShowCurPresetPPDefs()

		# modules
		self.ModViewFrame.ShowCurPresetMods()

		# compiler settings
		self.GccPathLineEdit.setText(CurCompilerSettings.Path)
		self.CFLAGSLineEdit.setText(CurCompilerSettings.CFLAGS)
		self.LDFLAGSLineEdit.setText(CurCompilerSettings.LDFLAGS)

		CompilerIndex = self.CompilerComBox.findText(CurArch.CurrentCompilerName)
		self.CompilerComBox.setCurrentIndex(CompilerIndex)

		# linkscript
		CurLinkScriptIndex = self.LinkScriptComBox.findText(CurPreset.CurrentLinkScript)
		self.LinkScriptComBox.setCurrentIndex(CurLinkScriptIndex)

		# arch
		CurArchIndex = self.ArchComBox.findText(CurPreset.CurrentArchName)
		self.ArchComBox.setCurrentIndex(CurArchIndex)



	def SelectLinkScript(self, LinkScript):
		CurPreset = mcglobals.gConfig.PresetsDict[mcglobals.gConfig.CurPresetName]
		CurPreset.CurrentLinkScript = unicode(LinkScript)


	def closeEvent(self, event):
		reply = mcmsg.SaveOnQuitMsgBox()

		if reply == QtGui.QMessageBox.Save:
			CfgDumpFlie = open(CfgDumpFileName, 'w+')
			pickle.dump(GlobalConfig, CfgDumpFlie)
			event.accept()
		elif reply == QtGui.QMessageBox.Discard:
			event.accept()
		elif reply == QtGui.QMessageBox.Cancel:
			event.ignore()


	def SaveAndExit(self):
		self.SaveCompilerSettings()
		print 'dir ',  mcglobals.gConfig.SrcDir
		CfgDumpFile = open(mcglobals.gCfgDumpFileName, 'w+')
		pickle.dump(mcglobals.gConfig, CfgDumpFile)
		gen = mcgen.ConfigGenerator()
		gen.generate()
		QtGui.qApp.quit()


