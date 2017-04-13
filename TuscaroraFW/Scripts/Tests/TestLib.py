#!/usr/bin/python

import sys
import os, errno
import argparse
import tempfile 
import subprocess
import shutil
import glob

import TestUtils

global scriptFile
scriptFile='None'

def RunCommandWait(cmd, silent):
	return TestUtils.RunCommandWait(cmd,silent)

def ParseArgs(_args):
	#common place to define args for validation
	parser = argparse.ArgumentParser(description='Process arguments for validate scripts.')
	#parser.add_argument('testname', nargs='+')
	parser.add_argument('--execDir', '-e', dest='execDir', help='Execution directory. If this option is provided temp directory will not be created')
	parser.add_argument('--outputFile', '-o', dest='outFile', help='Test has been run and output stored in this file/dir, just validate output')
	parser.add_argument('--valFilePath','-vf', dest='validationFile', help='The path to find the output validation script')
	parser.add_argument('--arch', '-p', '--platform', dest='arch', type=str, default='x86_64',
										help='Platform architecture to validate on')
	parser.add_argument('--verbose', dest='verbose', action='store_true',
										help='Output details to test receipts')

	#parser.add_argument('tests', nargs='+', 
	#                   help='List of tests to validate')


	#args = parser.parse_args(_args)
	args = parser.parse_args()

	#print "Validating for architecture: " +  args.arch
	return args


TusDir = os.environ.get('TUS')
args= ParseArgs(sys.argv)

def CopyDceOutput (tus, dst):
	tusOutput = tus + '/dceln/files-*'
	dest = dst + '/dceOutput/'
	print 'Tus output dir: ', tusOutput
	
	for d in glob.glob(tusOutput):
		#print 'Directory: ', d                                                                                                                                
		cmd = 'mkdir -p ' + dest +'; cp -r ' + d + " " +dest
		TestUtils.RunCommandWait(cmd, 0)
	
	TestUtils.CopyAnything(tus + '/dceln/CourseChangeData.txt', dest)
	


def UnknownPlatformError():
  print "Unknown arch: " + arch
  print "Supported architectures: x86_64, arm, dce . Quiting. "
  exit()
  

def MakeRunnables(arch, runDir, test, testArgs, finalValScript):
	curpath=os.getcwd()
	#print "Current execution path: " , curpath
	global TusDir
	print "Bulding for :", arch
	print ("Changing to " + TusDir)
	os.chdir(TusDir)
	launchFile=""
	TestUtils.Mkdir_p("Run")
	if(runDir == "None"):
		runDir = TestUtils.MakeTempDir(test,"")
		runDir = TusDir+"/"+runDir
		
	print "Binaries will be copied to : ", runDir
	if(arch == 'dce'):
	  cmd="./runOrDebug.sh --arch " + arch + " " + test + testArgs
	  (status,out) = TestUtils.RunCommandWait(cmd,0)
	  
	  #copy output files to runDir
	  CopyDceOutput(TusDir, runDir + "/")
	  #shutil.copy(TusDir+ "/dceln/*", )  
	else:
	  outFile = runDir + "/" + test + "Output.txt"
	  #cmd="./runOrDebug.sh --arch x86_64 -o " + outFile + " Timer -- RunTime 10 "
	  cmd="./runOrDebug.sh --arch " + arch + " --compile-only " + test
	  
	  (status,out) = TestUtils.RunCommandWait(cmd,0)
	    
	  #print "Test exit status: ", status
	  #if status != 0 :
		  #print "Couldn't compile successfully. Compilation exited with status: ", status, "Not sure what happened. Try ./runOrDebug.sh "
		  #exit
		  
	  #copy binary to run
	  shutil.copy(TusDir+ "/" + test, runDir + "/")  
	  launchFile=CreateLaunchScript(arch, runDir, test, testArgs, finalValScript)

	if finalValScript is not None and finalValScript:
		shutil.copy(finalValScript, runDir)

	#switch back to original directory
	os.chdir(curpath)
	return runDir, launchFile

def CreateLaunchScript(arch, runDir, test, testArgs, finalValScript):
	
	print "Creating the scripts to run from target directory for the test: ", test
	filename=runDir + "/launchTest_" + test + ".py"
	with open(filename, "w+") as f:
		if testArgs is not None:
			cmd = "./" + test + " Test " + test + " " + testArgs
		else: 
			cmd = "./" + test + " Test " + test 
		logfile = test + "Output.txt"
		f.write("#!/usr/bin/python\n\
import subprocess\n\
import sys\n\
\n\
loghandle=open(\""+logfile+"\", \"w+\")\n\
\
print \"Launching the test, this could take some time, depending on how long the test is being run\"\n\
sys.stdout.flush()\n\
p = subprocess.Popen(\""+cmd+"\", shell=True, universal_newlines=True, stdout=loghandle, stderr=loghandle)\n\
ret_code = p.wait()\n\
loghandle.flush()\n\
")
		f.write("print \"Running final validation on the output file(s)...\" \nsys.stdout.flush()\n")
		if finalValScript is not None: 
			cmd=finalValScript+ " -o " + logfile
			f.write("p = subprocess.Popen(\""+cmd+"\", shell=True, universal_newlines=True)\nret_code = p.wait()\n");		
		f.close()
		
	os.chmod(filename, 0755)
	return filename


def RunRunnables(arch, runDir, test, testArgs, launchScript):
	#print "Bulding for :", arch
	
	if(arch == 'arm'):
		print "\n\nMake sure you launch the test and validation from the ARM device."
		#exit()
	try:	
		if (os.path.isdir(runDir)):
			print ("Changing to execution dir:  " + runDir)
	except (FileNotFoundError, IOError):
		print "Run directory does not exist, quiting"
		exit()
		
	os.chdir(runDir)

	lanuchScript= "launchTest_"+ test + ".py"
	if (arch == 'x86_64'):
	  print "\n\nLaunch test ....."
	  #cmd = lanuchScript +" " +test+ "Output.txt 2>&1"
	  cmd = "./"+ lanuchScript
	elif (arch == 'arm'):
		print "preparing scripts for arm"
	elif (arch == 'dce'):
		print "preparing to run on dce"
	else:
		UnknownPlatformError()
			
	status,out = TestUtils.RunCommandWait(cmd,1)
	
	return out

	
	#print "Executing file: " + scriptFile

def RunTestAndVerify(TestName, TestParams, DataCheckerFuction):
	global scriptFile
	if args.outFile is not None:
		if(os.path.exists(args.outFile)):
			print "Output file: ",  args.outFile ," already exists. Just running validation on it"
			DataCheckerFuction(args.arch, args.outFile)
			exit()
		else:
			print "Something wrong, -o option provided, but the output file/ directory does not exist. Quiting"
			exit()

	#Step 1: Create the binary and other support files
	execDir=""
	if args.execDir is not None:
		if(os.path.isdir(args.execDir)):
			(execDir, lanuchFile)= MakeRunnables(args.arch,args.execDir,TestName, TestParams, scriptFile)
		else:
			print "Something wrong: Execution Directory (-e) option passed, but the directory doesnt exist. Quiting"
			exit()
	else:
		(execDir, lanuchFile) = MakeRunnables(args.arch,"None",TestName, TestParams, scriptFile)

	#Step 2: Run the test, If you are on dce, you can skip this.
	result=""
	if(args.arch == 'arm' or args.arch == 'x86_64'):
		print "Validating output"
		result=RunRunnables(args.arch,execDir,TestName, TestParams, scriptFile)
	elif(args.arch == 'dce'):
		#you dont have to run on dce. MakeRunnables would have already run the test and created output
		#so call the checkoutput directly
		result+=DataCheckerFuction('dce', execDir + '/dceOutput')

	print "Result string: ", result

	if(result.find("UNITPASS") > -1):
		TestUtils.Success("PASS")
	else:
		TestUtils.Die("FAIL")


def RunTest(TestName, TestParams, DataCheckerFuction, ExecDir):

	#Step 1: Create the binary and other support files
	execDir=ExecDir
	if execDir is not None:
		if(os.path.isdir(ExecDir)):
			(execDir, lanuchFile)= MakeRunnables(args.arch,execDir,TestName, TestParams, scriptFile)

	else:
		print "Something wrong: Test writer wants to just run a test, but has not provided a valid Execution Directory. Quiting"
		exit()

	#Step 2: Run the test, If you are on dce, you can skip this.
	result=""
	if(args.arch == 'arm' or args.arch == 'x86_64'):
		print "Validating output"
		result=RunRunnables(args.arch,execDir,TestName, TestParams, scriptFile)
	elif(args.arch == 'dce'):
		print "Finished running test :", TestName, " ", TestParams

