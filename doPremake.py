# -*- coding: utf-8 -*-
import subprocess, sys, os, os.path
toolsdir = os.environ['DirToolsRoot']
premake = os.path.join(toolsdir,'premake')
premake = os.path.join(premake,'premake4')
if 'win' in sys.platform:
   premake = premake + '.exe' 
   target = 'vs2008'
else:
   target = 'codeblocks'
cmd = '%s %s'%(premake,target)
subprocess.call(cmd)
#raw_input()