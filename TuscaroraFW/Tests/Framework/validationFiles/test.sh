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

cd $TUS

#echo "Run ack wifi test"
#./runOrDebug.sh FI -- Size $nodes RunTime $time PatternArgs normal WFConfig /home/vinod/Tuscarora/TuscaroraFW/14wf-100nodes-ack.cnf InterferenceModel RangePropagationLossModel 
#cd Scripts/Masahiro
#./ack.out $nodes $time $wf 1 
#cd ../..
#./runOrDebug.sh FI -- Size $nodes RunTime $time PatternArgs bnormal WFConfig /home/vinod/Tuscarora/TuscaroraFW/14wf-100nodes-ack.cnf InterferenceModel RangePropagationLossModel 
#cd Scripts/Masahiro
#./ack.out $nodes $time $wf 2
#cd ../..

echo "Run ack sw test"
./runOrDebug.sh FI -- Size $nodes RunTime $time PatternArgs normal WFConfig $TUS/Tests/Framework/2noack.cnf InterferenceModel RangePropagationLossModel LinkEstimationPeriod 1000000 
./Tests/Framework/validationFiles/ValidateFI $nodes $time $wf 1  $TUS/dceln

#./runOrDebug.sh FI -- Size $nodes RunTime $time PatternArgs bnormal WFConfig /home/vinod/Tuscarora/TuscaroraFW/14wf-100nodes-sw-ack.cnf InterferenceModel RangePropagationLossModel LinkEstimationPeriod 1000000 
#./Tests/Framework/validationFiles/ValidateFI $nodes $time $wf 2 $TUS/dceln

#echo "Run noack wifi test"
#./runOrDebug.sh FI -- Size $nodes RunTime $time PatternArgs normal WFConfig /home/vinod/Tuscarora/TuscaroraFW/14wf-100nodes-noack.cnf InterferenceModel RangePropagationLossModel LinkEstimationPeriod 1000000 
#./Tests/Framework/validationFiles/ValidateFI $nodes $time $wf 1 

#./runOrDebug.sh FI -- Size $nodes RunTime $time PatternArgs bnormal WFConfig /home/vinod/Tuscarora/TuscaroraFW/14wf-100nodes-noack.cnf InterferenceModel RangePropagationLossModel LinkEstimationPeriod 1000000 
#./Tests/Framework/validationFiles/ValidateFI $nodes $time $wf 2

#echo "Run noack sw test"
#./runOrDebug.sh FI -- Size $nodes RunTime $time PatternArgs normal WFConfig /home/vinod/Tuscarora/TuscaroraFW/14wf-100nodes-sw-noack.cnf InterferenceModel RangePropagationLossModel LinkEstimationPeriod 1000000 
#./Tests/Framework/validationFiles/ValidateFI $nodes $time $wf 1 

#./runOrDebug.sh FI -- Size $nodes RunTime $time PatternArgs bnormal WFConfig /home/vinod/Tuscarora/TuscaroraFW/14wf-100nodes-sw-noack.cnf InterferenceModel RangePropagationLossModel LinkEstimationPeriod 1000000 
#./Tests/Framework/validationFiles/ValidateFI $nodes $time $wf 2

