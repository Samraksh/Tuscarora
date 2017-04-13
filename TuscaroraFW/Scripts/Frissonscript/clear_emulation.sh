#!/bin/bash 
# set -u
set -e
# set -x
cd $NS3_DCE
for i in `seq 0 49`
do
    pwd
    cd files-$i/
    rm -rf emulation*.txt
    #mkdir /home/masahiro/sample_program/files-$i/
    #cp emulation42.txt /home/masahiro/sample_program/files-$i/
    cd ..
done
