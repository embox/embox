# -*- coding: utf-8 -*-
from PyQt4 import QtGui, QtCore
import sys

def WarnMsgBox(MsgText):
	msgBox = QtGui.QMessageBox();
	msgBox.setWindowTitle("Monitor Configurator")
	msgBox.setText(MsgText);
	msgBox.setIcon(QtGui.QMessageBox.Warning)
	msgBox.setStandardButtons(QtGui.QMessageBox.Ok);
	msgBox.exec_();


def AskForIgnoreMsgBox(MsgText):
	msgBox = QtGui.QMessageBox();
	msgBox.setWindowTitle("Monitor Configurator")
	msgBox.setText(MsgText);
	msgBox.setIcon(QtGui.QMessageBox.Warning)
	msgBox.setStandardButtons(QtGui.QMessageBox.Ignore | QtGui.QMessageBox.Abort);
	reply = msgBox.exec_();
	if reply == QtGui.QMessageBox.Abort:
		sys.exit(MsgText)


def QuestionMsgBox(MsgText):
	msgBox = QtGui.QMessageBox();
	msgBox.setWindowTitle("Monitor Configurator")
	msgBox.setText(MsgText);
	msgBox.setIcon(QtGui.QMessageBox.Question)
	msgBox.setStandardButtons(QtGui.QMessageBox.Ok | QtGui.QMessageBox.Cancel);

	answer = msgBox.exec_();
	if answer == QtGui.QMessageBox.Ok:
		return True
	else:
		return False


def SaveOnQuitMsgBox():
	msgBox = QtGui.QMessageBox();
	msgBox.setWindowTitle("Monitor Configurator")
	msgBox.setText("Do you want to save your changes?");
	msgBox.setIcon(QtGui.QMessageBox.Warning)
	msgBox.setStandardButtons(QtGui.QMessageBox.Save | QtGui.QMessageBox.Discard | QtGui.QMessageBox.Cancel);
	msgBox.setDefaultButton(QtGui.QMessageBox.Save);
	return msgBox.exec_();
