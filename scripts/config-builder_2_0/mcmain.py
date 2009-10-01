#!/usr/bin/env python
# -*- coding: utf-8 -*-
# requirement: python >= 2.6
#              sip >= 4.9
#              PyQt >= 4.6
#              Qt >= 4.5

import os, re, sys, codecs, pickle, copy

from PyQt4 import QtGui, QtCore

import mcgui, mcmsg, mcerrors, mcglobals

if __name__=='__main__':

	app = QtGui.QApplication(sys.argv)

	mcglobals.gConfig = mcglobals.MonitorConfig_t()

	appMainCfgDlg = mcgui.MainCfgDlg_t()
	appMainCfgDlg.show()


	sys.exit(app.exec_())

