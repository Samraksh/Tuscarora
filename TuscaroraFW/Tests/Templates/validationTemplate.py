#!/usr/bin/python

##############Imports
import sys
import os
sys.path.append(os.environ.get('TUS')+'/Scripts/Tests/')
from validateLib import *

##########platform specific validation starts here functions start here

def validateDce():
	print "Validating for DCE"
	
	

def validateX86():
	print "Validating for X86"
	
	

def validateArm():
	print "Validating for ARM"

##########Execution starts here

args= PargeArgs(sys.argv)

if args.arch == 'x86_64':
	validateX86()
elif args.arch == 'arm':
	validateArm()
else:
	validateDce()
