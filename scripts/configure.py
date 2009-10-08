#!/usr/bin/env python
# -*- coding: utf-8 -*-

import traceback, getopt, sys
from ConfigBuilder.QtGui import mcmain
from ConfigBuilder.TkGui import confmain

if __name__=='__main__':
        try:
    		opts, args = getopt.getopt(sys.argv[1:], "m:", ["mode="])
            	for o, a in opts:
                	if o in ("-m", "--mode"):
                    		if a == "tk":
                    			confmain.main()
                    		elif a == "qt":
                    			mcmain.main()
                        else:
                        	assert False, "unhandled option"
        except:
        	traceback.print_exc()
