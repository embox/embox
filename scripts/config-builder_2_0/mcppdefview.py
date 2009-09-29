# -*- coding: utf-8 -*-

from PyQt4 import QtGui, QtCore, uic

import mcglobals, mcerrors, mcmsg


# Ui_PreprocDefnFrame_t represents a preprocessor definition frame (allows to view, delete and call edit dialog)
(Ui_PreprocDefnFrame_t, Ui_PreprocDefnFrameBase_t) = uic.loadUiType('scripts/config-builder_2_0/ui/PreprocDefnFrame.ui')

# Ui_PreprocDefnEditDlg_t represents a dialog for editing a preprocessor definition
(Ui_PreprocDefnEditDlg_t, Ui_PreprocDefnEditDlgBase_t) = uic.loadUiType('scripts/config-builder_2_0/ui/PreprocDefnEditDlg.ui')


class PreprocDefnEditDlg_t (Ui_PreprocDefnEditDlg_t, Ui_PreprocDefnEditDlgBase_t):

	def __init__(self, DefnStr, Val):
		Ui_PreprocDefnFrameBase_t.__init__(self)

		self.setupUi(self) # Set up the user interface from Designer.

		self.buttonBox.accepted.connect(self.CheckOnAccept)


	def CheckOnAccept(self):
		DefnStr = unicode(self.DefnStrLineEdit.text())
		Val = unicode(self.ValLineEdit.text())
		if len(DefnStr) < 2:
			mcmsg.WarnMsgBox("Preprocessor Definitions of legth less than 2 symbols are not allowed")
			return

		CurPreset = mcglobals.gConfig.PresetsDict[mcglobals.gConfig.CurPresetName]
		CurPreset.EditPreprocDef(DefnStr, Val, True)
		self.accept()



class PreprocDefnFrame_t (Ui_PreprocDefnFrame_t, Ui_PreprocDefnFrameBase_t):

	MEditClickedSig = QtCore.pyqtSignal(unicode, unicode)
	MDelClickedSig = QtCore.pyqtSignal(unicode)
	MEnabledClickedSig = QtCore.pyqtSignal(unicode, int)

	def __init__(self, DefnStr, Val, isEnabled):
		Ui_PreprocDefnFrameBase_t.__init__(self)

		self.setupUi(self) # Set up the user interface from Designer.

		self.EditButton.clicked.connect(self.MEditClickedSlot)
		self.DelButton.clicked.connect(self.MDelClickedSlot)
		self.EnabledChBox.clicked.connect(self.MEnabledClickedSlot)

		if Val:
			self.NameValLabel.setText(DefnStr+'='+Val)
		else:
			self.NameValLabel.setText(DefnStr)

		self.EnabledChBox.setChecked(isEnabled)


	def MEditClickedSlot(self):
		(DefStr, Val) = self.SplitNameValLabel()
		self.MEditClickedSig.emit(DefStr, Val)


	def MDelClickedSlot(self):
		(DefStr, Val) = self.SplitNameValLabel()
		self.MDelClickedSig.emit(DefStr)


	def MEnabledClickedSlot(self, State):
		(DefStr, Val) = self.SplitNameValLabel()
		self.MEnabledClickedSig.emit(DefStr, State)


	def SplitNameValLabel(self):
		NameValStr = unicode(self.NameValLabel.text())
		(DefStr, Sep, Val) = NameValStr.partition('=')
		return (DefStr, Val)



class PPDefViewFrame_t (QtGui.QFrame):

	def __init__(self):
		QtGui.QFrame.__init__(self)

		self.DefnsLayout = QtGui.QVBoxLayout()
		#self.DefnsLayout.setContentsMargins(0,0,0,0)
		self.DefnsLayout.setAlignment(QtCore.Qt.AlignTop)
		self.setLayout(self.DefnsLayout)

		self.PreprocDefToFrameDict = {}


	def CallAddPreprocDefDlg(self):
		DefEditDlg = PreprocDefnEditDlg_t(DefnStr="", Val="")
		DefEditDlg.exec_()
		self.ShowCurPresetPPDefs()


	def CallEditPreprocDefDlg(self, DefnStr, Val):
		DefEditDlg = PreprocDefnEditDlg_t(DefnStr, Val)
		DefEditDlg.exec_()
		self.ShowCurPresetPPDefs()


	def AddDefnFrame(self, DefnStr, Val, isEnabled):
		self.PreprocDefToFrameDict[DefnStr] = PreprocDefnFrame_t(DefnStr,Val, isEnabled)
		self.PreprocDefToFrameDict[DefnStr].MEditClickedSig.connect(self.CallEditPreprocDefDlg)
		self.PreprocDefToFrameDict[DefnStr].MDelClickedSig.connect(self.DelDefn)
		self.PreprocDefToFrameDict[DefnStr].MEnabledClickedSig.connect(self.EnableDefn)
		self.DefnsLayout.addWidget(self.PreprocDefToFrameDict[DefnStr])


	def DelDefn(self, DefnStr):
		DefnStr = unicode(DefnStr)

		CurPreset = mcglobals.gConfig.PresetsDict[mcglobals.gConfig.CurPresetName]
		self.PreprocDefToFrameDict[DefnStr].deleteLater()
		del self.PreprocDefToFrameDict[DefnStr]
		del CurPreset.PreprocDefnsDict[DefnStr]


	def EnableDefn(self, DefnStr, State):
		DefnStr = unicode(DefnStr)
		CurPreset = mcglobals.gConfig.PresetsDict[mcglobals.gConfig.CurPresetName]
		CurPreset.PreprocDefnsDict[DefnStr].isEnabled = State


	def ShowCurPresetPPDefs(self):
		CurPreset = mcglobals.gConfig.PresetsDict[mcglobals.gConfig.CurPresetName]

		for DefnStr in self.PreprocDefToFrameDict:
			self.PreprocDefToFrameDict[DefnStr].deleteLater()

		self.PreprocDefToFrameDict = {}

		for CurDefStr in sorted(CurPreset.PreprocDefnsDict.keys()):
			Val = CurPreset.PreprocDefnsDict[CurDefStr].Value
			isEnabled = CurPreset.PreprocDefnsDict[CurDefStr].isEnabled
			self.AddDefnFrame(CurDefStr, Val, isEnabled)
