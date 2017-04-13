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
TestName='Timer'
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
		
#
def valDataElement(arch, datafile):
	print "Validating file: ", datafile
	cmd1 = "cat " + datafile + " | grep \"Timer 1 Handler Exit\" | grep \"shared counter: 0\" | wc -l"
	cmd2 = "cat " + datafile + " | grep \"Timer 2 Handler Exit\" | grep \"shared counter: 0\" | wc -l"
	(status,tim1Count)=TestLib.RunCommandWait(cmd1, True);
	(status,tim2Count)=TestLib.RunCommandWait(cmd2, True);
	cnt1=float(tim1Count)
	cnt2=float(tim2Count)
	
	ratio = cnt2/cnt1
	
	print "Timer 1 fired: ", cnt1, "times and Timer 2 fired: ", cnt2
	print "Ratio is ", ratio
	
	if (ratio > 4.5 and ratio <= 5.5):
		print "UNITPASS"
		return "UNITPASS"
	else:
		print "UNITFAIL"
		return "UNITFAIL"


def validateDceDir(dceDir):
	dceOutputDir = dceDir + '/files-*'
	print "Validating DCE dir: ", dceDir
	
	ret=""
	for d in glob.glob(dceOutputDir):
		print 'Directory: ', d  
		outputFile = d + '/var/log/*/stdout'
		print "Checking file: ", outputFile
		r=valDataElement('dce', outputFile)
		ret=ret+ " " + r
	return ret


##########Execution starts here

###########Test writer usually does not have to modify lines below

#run the test
TestLib.RunTestAndVerify(TestName, TestParams, CheckOutputData)


	
