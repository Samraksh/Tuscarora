#!/bin/bash
# set -x

# Call as: $0 destinationDirectory patchfile

TUSC_VERSION=$1
DCE_DIR=$2
NS3_DIR=$3


echo "Checking ns3"; 
cd ${NS3_DIR}
if (hg --config ui.username=TuscaroraInstaller-${TUSC_VERSION} branch | grep Tuscarora-${TUSC_VERSION}); then 
	echo "Adding a commit in ${NS3_DIR}"
	hg add  || true
	hg --config ui.username=TuscaroraInstaller-${TUSC_VERSION} commit -m "Updates for Tuscarora installation for ${TUSC_VERSION}" || true
else
	echo "Branch name does not match the version number"; 
	exit 1
fi


echo "Checking ns3-dce"; 
cd ${DCE_DIR}
if (hg --config ui.username=TuscaroraInstaller-${TUSC_VERSION} branch | grep Tuscarora-${TUSC_VERSION}); then 
	echo "Adding a commit in ${DCE_DIR}"
	hg add  || true
	hg --config ui.username=TuscaroraInstaller-${TUSC_VERSION} commit -m "Updates for Tuscarora installation for ${TUSC_VERSION}"  || true
else
	echo "Branch name does not match the version number"; 
	exit 1
fi

