# -*- coding: utf-8 -*-
import sys, os, os.path
scriptdir = os.environ['DirScriptsRoot']
sys.path.append(scriptdir)
projectPath = sys.path[0]
import buildProject
buildFile = os.path.join(projectPath,'build')
buildFile = os.path.join(buildFile,'NgoUnit')
out,err = buildProject.buildSolutionAllPlatform(buildFile)
print out, err
raw_input()