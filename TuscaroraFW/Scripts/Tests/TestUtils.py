#!/usr/bin/python

import sys
import os, errno
import argparse
import tempfile 
import subprocess
import shutil
import glob

def CopyAnything(src, dst):
	try:
		shutil.copytree(src, dst)
	except OSError as exc: # python >2.5
		if exc.errno == errno.ENOTDIR:
			shutil.copy(src, dst)
		else: raise


def Success(msg):
	print msg
	sys.exit(0)

def Die(msg):
	print msg
	sys.exit(1)

#Copy one dir to another
def MyCopyTree(src, dst, symlinks=False, ignore=None):
    for item in os.listdir(src):
        s = os.path.join(src, item)
        d = os.path.join(dst, item)
        if os.path.isdir(s):
            shutil.copytree(s, d, symlinks, ignore)
        else:
            shutil.copy2(s, d)

def RunCommandWithStdout(cmd, silent):
	#p = subprocess.Popen(cmd, stderr=subprocess.PIPE) 
	p = subprocess.Popen(cmd, shell=True)
	#p = subprocess.Popen(cmd)
	
	if(not silent):
		output, err = p.communicate()
	p_status = p.wait()
	#if p_status != 0 :
	#	print "Couldn't run. File not found or some such thing"
	
	return p_status

def RunCommandWait(cmd, silent):
	#p = subprocess.Popen(cmd, stderr=subprocess.PIPE) 
	curpath=os.getcwd()
	print "Current execution path: " , curpath
	
	if(silent):
	  p = subprocess.Popen(cmd, shell=True,stdout=subprocess.PIPE)
	  output, err = p.communicate()
	  p_status = p.wait()
	else:
	  p = subprocess.Popen(cmd, shell=True,stderr=subprocess.PIPE)
	  output, err = p.communicate()
	  p_status = p.wait()

	#if p_status != 0 :
	#	print "Couldn't run. File not found or some such thing"
	
	return p_status, output



def Mkdir_p(path):
	try:
		os.makedirs(path)
	except OSError as exc: # Python >2.5
		if exc.errno == errno.EEXIST and os.path.isdir(path):
			pass
		else: raise


def MakeTempDir(Prefix, Dir):
	if not Dir:
		tempDir=tempfile.mkdtemp(prefix=Prefix+"-")
	else:
		tempDir=tempfile.mkdtemp(prefix=Prefix+"-", dir=Dir)
		
		
	return tempDir
