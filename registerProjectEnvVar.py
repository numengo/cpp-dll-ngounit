# -*- coding: utf-8 -*-
import sys, os
scriptdir = os.environ['DirScriptsRoot']
sys.path.append(scriptdir)
projectPath = sys.path[0]
import manageEnv
try:
   manageEnv.createUserEnvVar('DirProjectNgoUnitRoot',projectPath)
except Exception,er:
   print er
#raw_input()