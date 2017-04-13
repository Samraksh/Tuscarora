#!/usr/bin/python

##############Imports
import sys
import os
import inspect
import glob
import tempfile 

TusDir = os.environ.get('TUS')
if TusDir is None:
	print "TUS environmental variable is not set. Tuscarora framework installation not complete, can't run validation"
	sys.exit(1);
sys.path.append(TusDir+'/Scripts/Tests/')
#import testing library
import TestLib

TestLib.scriptFile = os.path.dirname(os.path.abspath(inspect.getfile(inspect.currentframe()))) 
basename= os.path.basename(inspect.getfile(inspect.currentframe())) # script filename (usually with path)
TestLib.scriptFile+= "/"+ basename 
###################Dont touch lines above this#########

#set test parameters
TestName='ScheduleMRG'
TestParams = " -- RunTime 10"

##########functions start here

#Every validation should have this function.
#This is incharge of overall output checking and return a string, that has one or many UNITPASS or UNITFAIL
def CheckOutputData(arch, dataObject):
	#dataObject is a file for arm and x86_64 and a directory for dce
	#check if dataObject is a file or dir
	print "Checking output files in : ", dataObject
	if (not os.path.exists(dataObject)): 
		Die("Data file/dir : " + dataObject +" does not exist: Quiting")
	elif (os.path.isdir(dataObject)):
		if(arch == "dce"):
			print "Directory has been provided for dce output"
			#dceDir = dataObject + '/dceOutput/'
			return validateDceDir(dataObject)
		else:
			Die("Confused: Data directory given, but arch is not dce. Are you sure? Quiting.")		
	else:
		return valDataElement(arch,dataObject);
		

def valDataElement(arch, datafile):
	print "UNITFAIL"
	return "UNITFAIL"



## The test runs the same schedule on all nodes, therefore their output should be exactly same.
## But initialization on each node prints a line with node id, which will cause four lines of diff output. 
## So a diffcount of 4 or less is good.
def validateDceDir(dceDir):
	print "Validating DCE dir: ", dceDir
	os.chdir(dceDir)
	cmd='diff files-0/var/log/*/stdout files-1/var/log/*/stdout | wc -l' 

	(status,diffCount)=TestLib.RunCommandWait(cmd, True);
	print 'Difference: ', float(diffCount)
	if(float(diffCount) < 5):
		print "UNITPASS"
		return "UNITPASS"
	else:
		print "UNITFAIL"
		return "UNITFAIL"


##########Execution starts here

###########Test writer usually does not have to modify lines below

#run the test
TestLib.RunTestAndVerify(TestName, TestParams, CheckOutputData)
