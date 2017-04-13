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

echo "Replace unicast"
./runOrDebug.sh FI -- Size $nodes RunTime $time PatternArgs replace WFConfig $path InterferenceModel RangePropagationLossModel > /dev/null
cd Scripts/Masahiro
./replace.out 
cd ../..
echo "Replace broadcast"
./runOrDebug.sh FI -- Size $nodes RunTime $time PatternArgs breplace WFConfig $path InterferenceModel RangePropagationLossModel > /dev/null
cd Scripts/Masahiro
./replace.out 
cd ../..
echo "Cancel unicast"
./runOrDebug.sh FI -- Size $nodes RunTime $time PatternArgs cancel WFConfig $path InterferenceModel RangePropagationLossModel > /dev/null
cd Scripts/Masahiro
./cancel.out 
cd ../..
echo "Cancel broadcast"
./runOrDebug.sh FI -- Size $nodes RunTime $time PatternArgs bcancel WFConfig $path InterferenceModel RangePropagationLossModel > /dev/null
cd Scripts/Masahiro
./cancel.out 
cd ../..
echo "Add destination"
./runOrDebug.sh FI -- Size $node  RunTime $time PatternArgs add WFConfig $path  >/dev/null
cd Scripts/Masahiro
./add
cd ../..


