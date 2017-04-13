#!/bin/bash
# set -x

TUSC_VERSION=$1
DCE_DIR=$2
NS3_DIR=$3

echo "Checking Tuscarora installation version $1"
echo "Checking ns3 in $3"; 
cd ${NS3_DIR}
if (hg --config ui.username=TuscaroraInstaller-${TUSC_VERSION} branch | grep Tuscarora-${TUSC_VERSION}); then 
	echo "You already have ${TUSC_VERSION} installed"; 
else if (hg --config ui.username=TuscaroraInstaller-${TUSC_VERSION} branches | grep Tuscarora-${TUSC_VERSION}); then 
		echo "Previous Tuscarora Installation detected with the same version number as the one you are installing ${TUSC_VERSION}"; 
		echo "Switching to previous installation"; 
		hg --config ui.username=TuscaroraInstaller-${TUSC_VERSION} update -C Tuscarora-$(TUSC_VERSION); 
	else 
		echo "No previous Tuscarora installation is detected for ${TUSC_VERSION}"; 
		echo "Proceeding with installation"; 
		if (hg --config ui.username=TuscaroraInstaller-${TUSC_VERSION} tags | grep "Tusc-BeforeTusc"); then 
			echo "Tuscarora restore point detected"; 
			echo "Setup will try reverting back to the point before previous installation and initate the new installation from that point"
			hg --config ui.username=TuscaroraInstaller-${TUSC_VERSION} update -C default
			echo "Branching off"; 
			hg --config ui.username=TuscaroraInstaller-${TUSC_VERSION} branch Tuscarora-${TUSC_VERSION};
		else 
			echo "No restore point is detected"; 
			echo "Marking current state as Tusc-BeforeTusc-${TUSC_VERSION}"; 
			hg --config ui.username=TuscaroraInstaller-${TUSC_VERSION} tag Tusc-BeforeTusc-${TUSC_VERSION}; 
			echo "Branching off"; 
			hg --config ui.username=TuscaroraInstaller-${TUSC_VERSION} branch Tuscarora-${TUSC_VERSION}
		fi;
	fi;
fi;

echo "Checking ns3-dce  in $2"; 
cd ${DCE_DIR}
if (hg --config ui.username=TuscaroraInstaller-${TUSC_VERSION} branch | grep Tuscarora-${TUSC_VERSION}); then 
	echo "You already have ${TUSC_VERSION} installed"; 
else if (hg --config ui.username=TuscaroraInstaller-${TUSC_VERSION} branches | grep Tuscarora-${TUSC_VERSION}); then 
		echo "Previous Tuscarora Installation detected with the same version number as the one you are installing ${TUSC_VERSION}"; 
		echo "Switching to previous installation"; 
		hg --config ui.username=TuscaroraInstaller-${TUSC_VERSION} update -C Tuscarora-$(TUSC_VERSION); 
	else 
		echo "No previous Tuscarora installa tion is detected for ${TUSC_VERSION}"; 
		echo "Proceeding with installation"; 
		if (hg --config ui.username=TuscaroraInstaller-${TUSC_VERSION} tags | grep "Tusc-BeforeTusc"); then 
			echo "Before Tuscarora restore point detected"; 
			echo "Setup will try reverting back to the point before previous installation and initate the new installation from that point"
			hg --config ui.username=TuscaroraInstaller-${TUSC_VERSION} update -C default
			echo "Branching off"; 
			hg --config ui.username=TuscaroraInstaller-${TUSC_VERSION} branch Tuscarora-${TUSC_VERSION};
		else 
			echo "No restore point is detected"; 
			echo "Marking current state as Tusc-BeforeTusc-${TUSC_VERSION}"; 
			hg --config ui.username=TuscaroraInstaller-${TUSC_VERSION} tag Tusc-BeforeTusc-${TUSC_VERSION}; 
			echo "Branching off"; 
			hg --config ui.username=TuscaroraInstaller-${TUSC_VERSION} branch Tuscarora-${TUSC_VERSION}
		fi;
	fi;
fi;
