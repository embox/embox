# -*- coding: utf-8 -*-

from PyQt4 import QtGui, QtCore, uic

from . import mcmsg
from ..Parser import mcglobals
from ..Misc import mcerrors

# Ui_ModuleFrame_t represents a module frame (allows to stage and get module info)
(Ui_ModuleFrame_t, Ui_ModuleFrameBase_t) = uic.loadUiType('scripts/ConfigBuilder/QtGui/ui/ModuleFrame.ui')

# Ui_ModTypeFrame_t represents a frame in which modules of specific type are shown and can be staged
(Ui_ModTypeFrame_t, Ui_ModTypeFrameBase_t) = uic.loadUiType('scripts/ConfigBuilder/QtGui/ui/ModTypeFrame.ui')


class ModTypeFrame_t (Ui_ModTypeFrame_t, Ui_ModTypeFrameBase_t):

	def __init__(self, ModType):
		Ui_ModTypeFrameBase_t.__init__(self)

		self.setupUi(self) # Set up the user interface from Designer.

		self.TogVisButton.clicked.connect(self.TogVisButtonClickedSlot)

		self.TypeLabel.setText(ModType+'s')

		self.ModulesLayout.setAlignment(QtCore.Qt.AlignTop)

		self.verticalLayout.setAlignment(QtCore.Qt.AlignTop)

		self.isCollapsed = False


	def TogVisButtonClickedSlot(self):
		if self.isCollapsed:
			self.ModulesFrame.show()
			self.TypeLabel.show()
			self.TogVisButton.setText('-')
		else:
			self.ModulesFrame.hide()
			self.TypeLabel.hide()
			self.TogVisButton.setText('+')
		self.isCollapsed = not self.isCollapsed


	def paintEvent(self,evnet):
		Ui_ModTypeFrameBase_t.paintEvent(self,evnet)
		if self.isCollapsed:
			MyPainter = QtGui.QPainter(self)
			MyPainter.save()
			MyPainter.rotate(90)
			MyPainter.drawText(25,-5,self.TypeLabel.text())
			MyPainter.restore()



class ModuleFrame_t (Ui_ModuleFrame_t, Ui_ModuleFrameBase_t):

	MInfoClickedSig = QtCore.pyqtSignal(unicode)
	MEnabledClickedSig = QtCore.pyqtSignal(unicode, int)

	def __init__(self, ModName):
		Ui_ModuleFrameBase_t.__init__(self)

		self.setupUi(self) # Set up the user interface from Designer.

		self.InfoButton.clicked.connect(self.MInfoClickedSlot)
		self.EnabledChBox.clicked.connect(self.MEnabledClickedSlot)

		self.EnabledChBox.setText(ModName)

	def MInfoClickedSlot(self):
		self.MInfoClickedSig.emit(self.EnabledChBox.text())

	def MEnabledClickedSlot(self, State):
		self.MEnabledClickedSig.emit(self.EnabledChBox.text(), State)



class ModViewFrame_t (QtGui.QFrame):

	def __init__(self):
		QtGui.QFrame.__init__(self)
		self.ModTypeFramesLayout = QtGui.QHBoxLayout()
		#self.ModTypeFramesLayout.setContentsMargins(0,0,0,0)
		self.ModTypeFramesLayout.setAlignment(QtCore.Qt.AlignLeft)

		self.ModTypeToMTFrameDict = {}
		self.ModNameToModFrameDict = {}

		self.ModTypeToMTFrameDict['Persistent'] = ModTypeFrame_t('Persistent')
		self.ModTypeToMTFrameDict['Arch'] = ModTypeFrame_t('Arch')
		self.ModTypeFramesLayout.addWidget(self.ModTypeToMTFrameDict['Persistent'])
		self.ModTypeFramesLayout.addWidget(self.ModTypeToMTFrameDict['Arch'])

		for CurModName in sorted(mcglobals.gModulesDict.keys()):
		# initialize modules view
			CurModType = mcglobals.gModulesDict[CurModName].Type
			CurModFrame = ModuleFrame_t(CurModName)
			CurModFrame.MEnabledClickedSig.connect(self.ModEnableClicked)

			if CurModType in ['Arch','Persistent']:
				CurModFrame.EnabledChBox.setEnabled(False)

			# insert Frame in corresponding dict to provide access by ModName
			self.ModNameToModFrameDict[CurModName] = CurModFrame

			if CurModType in self.ModTypeToMTFrameDict:
				CurModTypeFrame = self.ModTypeToMTFrameDict[CurModType]
			else:
				# create a frame for this new module type
				# a frame, in which checkboxes of modules of one particular type are grouped
				CurModTypeFrame = ModTypeFrame_t(CurModType)

				# insert Frame in corresponding layout in the main gui dialog
				self.ModTypeFramesLayout.addWidget(CurModTypeFrame)

				# insert Frame in corresponding dict to provide access by ModType
				self.ModTypeToMTFrameDict[CurModType] = CurModTypeFrame

			CurModTypeFrame.ModulesLayout.addWidget(CurModFrame)


		self.setLayout(self.ModTypeFramesLayout)
		self.IsAllModsVisible = True


	def ModEnableClicked(self, ModName, State):
		ModName = unicode(ModName)
		CurPreset = mcglobals.gConfig.PresetsDict[mcglobals.gConfig.CurPresetName]
		if State:
			StagedMods = CurPreset.SafeStageModule(ModName, mcglobals.gModulesDict)
			if len(StagedMods)==0:
				self.ModNameToModFrameDict[ModName].EnabledChBox.setChecked(False)
				return

			for StagedModName in StagedMods:
				self.ModNameToModFrameDict[StagedModName].EnabledChBox.setChecked(True)

		else:
			UnstagedMods = CurPreset.SafeUnstageModule(ModName, mcglobals.gModulesDict)
			if len(UnstagedMods)==0:
				self.ModNameToModFrameDict[ModName].EnabledChBox.setChecked(True)
				return

			for UnstagedModName in UnstagedMods:
				self.ModNameToModFrameDict[UnstagedModName].EnabledChBox.setChecked(False)


	def ShowCurPresetMods(self):
		CurPreset = mcglobals.gConfig.PresetsDict[mcglobals.gConfig.CurPresetName]
		CurArch = CurPreset.ArchsToArchSettingsDict[CurPreset.CurrentArchName]

		# modules
		for CurModName in mcglobals.gModulesDict.keys():
			isStaged = CurPreset.StagedModulesDict[CurModName]
			self.ModNameToModFrameDict[CurModName].EnabledChBox.setChecked(isStaged)
			if CurModName in CurArch.ExcludedModules or mcglobals.gModulesDict[CurModName].Type in ['Persistent','Arch']:
				self.ModNameToModFrameDict[CurModName].EnabledChBox.setEnabled(False)
			else:
				self.ModNameToModFrameDict[CurModName].EnabledChBox.setEnabled(True)


	def ToggleAllModsVis(self):
		self.IsAllModsVisible = not self.IsAllModsVisible

		for CurMTFrame in self.ModTypeToMTFrameDict.values():
			if CurMTFrame.isCollapsed == self.IsAllModsVisible:
				CurMTFrame.TogVisButtonClickedSlot()
