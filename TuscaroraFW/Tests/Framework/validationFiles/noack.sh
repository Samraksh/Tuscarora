#!/bin/sh

#pwd
cd ../..
#pwd
echo "Enter node size" 
read nodes
echo "Enter simmtime"
read time
echo "Enter wf number"
read wf
echo "Enter absolute path to config file.(including file name). eg. /home/user/Tuscarora/TuscaroraFW/sample.cnf"
read path


echo "Run noack test: unicast"
./runOrDebug.sh FI -- Size $nodes RunTime $time PatternArgs normal WFConfig $path InterferenceModel RangePropagationLossModel > /dev/null
cd Scripts/Masahiro
./noack.out $nodes $time $wf 1 
cd ../..
echo "Run noack test: broadcast"
./runOrDebug.sh FI -- Size $nodes RunTime $time PatternArgs bnormal WFConfig $path InterferenceModel RangePropagationLossModel > /dev/null
cd Scripts/Masahiro
./noack.out $nodes $time $wf 2
cd ../..


