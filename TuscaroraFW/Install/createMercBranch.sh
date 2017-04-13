#!/bin/bash
# set -x

# Call as: $0 destinationDirectory patchfile

TUSC_VERSION=$1
DCE_DIR=$2
NS3_DIR=$3


echo "Checking ns3"; 
cd ${NS3_DIR}
hg --config ui.username=TuscaroraInstaller-${TUSC_VERSION} tag Tusc-BeforeTusc-${TUSC_VERSION}; 
hg --config ui.username=TuscaroraInstaller-${TUSC_VERSION} branch -f Tuscarora-${TUSC_VERSION}


echo "Checking ns3-dce"; 
cd ${DCE_DIR}
hg --config ui.username=TuscaroraInstaller-${TUSC_VERSION} tag Tusc-BeforeTusc-${TUSC_VERSION}; 
hg --config ui.username=TuscaroraInstaller-${TUSC_VERSION} branch -f Tuscarora-${TUSC_VERSION}

