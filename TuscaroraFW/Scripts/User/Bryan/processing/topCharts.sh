bgxupdate() {
    bgxoldgrp=${bgxgrp}
    bgxgrp=""
    ((bgxcount = 0))
    bgxjobs=" $(jobs -pr | tr '\n' ' ')"
    for bgxpid in ${bgxoldgrp} ; do
        echo "${bgxjobs}" | grep " ${bgxpid} " >/dev/null 2>&1
        if [[ $? -eq 0 ]] ; then
            bgxgrp="${bgxgrp} ${bgxpid}"
            ((bgxcount = bgxcount + 1))
        fi
    done
}

bgxlimit() {
    bgxmax=$1 ; shift
    bgxupdate
    while [[ ${bgxcount} -ge ${bgxmax} ]] ; do
        sleep 1
        bgxupdate
    done
    if [[ "$1" != "-" ]] ; then
        $* &
        bgxgrp="${bgxgrp} $!"
	echo $(date | awk '{print $4}') '[' ${bgxgrp} ']'
	sleep 10
    fi
}

density=10
time=120
mobility='RandomWalk2dMobilityModel'
density=10
range=560
speed=50
interference='RangePropagationLossModel'
pushd ${TUS}

size=100
speedmin=2
speedmax=4
beacon=1000000
bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 1 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType periodic                     DiscoveryType global        SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduleAwarePeriodic        DiscoveryType longlink2hop  SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduleAwarePeriodicDC      DiscoveryType longlink2hop  SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduleAwarePeriodicNoDisco DiscoveryType nulldiscovery SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduledPeriodicDC          DiscoveryType longlink2hop  SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduledPeriodic            DiscoveryType longlink2hop  SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
beacon=200000
bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 1 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType periodic                     DiscoveryType global        SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduleAwarePeriodic        DiscoveryType longlink2hop  SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduleAwarePeriodicDC      DiscoveryType longlink2hop  SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduleAwarePeriodicNoDisco DiscoveryType nulldiscovery SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduledPeriodicDC          DiscoveryType longlink2hop  SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduledPeriodic            DiscoveryType longlink2hop  SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
beacon=100000
bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 1 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType periodic                     DiscoveryType global        SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduleAwarePeriodic        DiscoveryType longlink2hop  SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduleAwarePeriodicDC      DiscoveryType longlink2hop  SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduleAwarePeriodicNoDisco DiscoveryType nulldiscovery SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduledPeriodicDC          DiscoveryType longlink2hop  SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduledPeriodic            DiscoveryType longlink2hop  SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 

speedmin=25
speedmax=75
beacon=1000000
bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 1 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType periodic                     DiscoveryType global        SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduleAwarePeriodic        DiscoveryType longlink2hop  SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduleAwarePeriodicDC      DiscoveryType longlink2hop  SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduleAwarePeriodicNoDisco DiscoveryType nulldiscovery SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduledPeriodicDC          DiscoveryType longlink2hop  SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduledPeriodic            DiscoveryType longlink2hop  SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
beacon=200000
bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 1 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType periodic                     DiscoveryType global        SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduleAwarePeriodic        DiscoveryType longlink2hop  SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduleAwarePeriodicDC      DiscoveryType longlink2hop  SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduleAwarePeriodicNoDisco DiscoveryType nulldiscovery SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduledPeriodicDC          DiscoveryType longlink2hop  SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduledPeriodic            DiscoveryType longlink2hop  SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
beacon=100000
bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 1 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType periodic                     DiscoveryType global        SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduleAwarePeriodic        DiscoveryType longlink2hop  SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduleAwarePeriodicDC      DiscoveryType longlink2hop  SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduleAwarePeriodicNoDisco DiscoveryType nulldiscovery SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduledPeriodicDC          DiscoveryType longlink2hop  SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduledPeriodic            DiscoveryType longlink2hop  SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
echo "ing on 100's"

size=200
speedmin=2
speedmax=4
beacon=1000000
bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 1 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType periodic                     DiscoveryType global        SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduleAwarePeriodic        DiscoveryType longlink2hop  SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduleAwarePeriodicDC      DiscoveryType longlink2hop  SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduleAwarePeriodicNoDisco DiscoveryType nulldiscovery SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduledPeriodicDC          DiscoveryType longlink2hop  SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduledPeriodic            DiscoveryType longlink2hop  SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
beacon=200000
bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 1 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType periodic                     DiscoveryType global        SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduleAwarePeriodic        DiscoveryType longlink2hop  SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduleAwarePeriodicDC      DiscoveryType longlink2hop  SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduleAwarePeriodicNoDisco DiscoveryType nulldiscovery SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduledPeriodicDC          DiscoveryType longlink2hop  SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduledPeriodic            DiscoveryType longlink2hop  SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
beacon=100000
bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 1 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType periodic                     DiscoveryType global        SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduleAwarePeriodic        DiscoveryType longlink2hop  SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduleAwarePeriodicDC      DiscoveryType longlink2hop  SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduleAwarePeriodicNoDisco DiscoveryType nulldiscovery SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduledPeriodicDC          DiscoveryType longlink2hop  SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduledPeriodic            DiscoveryType longlink2hop  SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 

speedmin=25
speedmax=75
beacon=1000000
bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 1 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType periodic                     DiscoveryType global        SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduleAwarePeriodic        DiscoveryType longlink2hop  SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduleAwarePeriodicDC      DiscoveryType longlink2hop  SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduleAwarePeriodicNoDisco DiscoveryType nulldiscovery SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduledPeriodicDC          DiscoveryType longlink2hop  SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduledPeriodic            DiscoveryType longlink2hop  SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
beacon=200000
#bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 1 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType periodic                     DiscoveryType global        SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduleAwarePeriodic        DiscoveryType longlink2hop  SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduleAwarePeriodicDC      DiscoveryType longlink2hop  SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduleAwarePeriodicNoDisco DiscoveryType nulldiscovery SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduledPeriodicDC          DiscoveryType longlink2hop  SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduledPeriodic            DiscoveryType longlink2hop  SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
beacon=100000
bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 1 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType periodic                     DiscoveryType global        SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduleAwarePeriodic        DiscoveryType longlink2hop  SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduleAwarePeriodicDC      DiscoveryType longlink2hop  SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduleAwarePeriodicNoDisco DiscoveryType nulldiscovery SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduledPeriodicDC          DiscoveryType longlink2hop  SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduledPeriodic            DiscoveryType longlink2hop  SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
echo "ing on 200's"


beacon=200000
size=100
density=10
speedmin=25
speedmax=75
for density in 3 7 10 20 30; do
    bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 1 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType periodic                     DiscoveryType global        SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
    bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduleAwarePeriodic        DiscoveryType longlink2hop  SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
    bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduleAwarePeriodicDC      DiscoveryType longlink2hop  SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
    bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduleAwarePeriodicNoDisco DiscoveryType nulldiscovery SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
    bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduledPeriodicDC          DiscoveryType longlink2hop  SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
    bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduledPeriodic            DiscoveryType longlink2hop  SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
done
density=10

speedmin=5
speedmax=15
bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 1 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType periodic                     DiscoveryType global        SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduleAwarePeriodic        DiscoveryType longlink2hop  SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduleAwarePeriodicDC      DiscoveryType longlink2hop  SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduleAwarePeriodicNoDisco DiscoveryType nulldiscovery SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduledPeriodicDC          DiscoveryType longlink2hop  SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduledPeriodic            DiscoveryType longlink2hop  SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 

speedmin=13
speedmax=37
bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 1 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType periodic                     DiscoveryType global        SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduleAwarePeriodic        DiscoveryType longlink2hop  SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduleAwarePeriodicDC      DiscoveryType longlink2hop  SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduleAwarePeriodicNoDisco DiscoveryType nulldiscovery SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduledPeriodicDC          DiscoveryType longlink2hop  SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduledPeriodic            DiscoveryType longlink2hop  SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 

speedmin=38
speedmax=112
bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 1 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType periodic                     DiscoveryType global        SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduleAwarePeriodic        DiscoveryType longlink2hop  SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduleAwarePeriodicDC      DiscoveryType longlink2hop  SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduleAwarePeriodicNoDisco DiscoveryType nulldiscovery SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduledPeriodicDC          DiscoveryType longlink2hop  SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduledPeriodic            DiscoveryType longlink2hop  SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
speedmin=25
speedmax=75
echo "ing on densities and speeds"


for size in 50 150 250; do
    bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 1 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType periodic                     DiscoveryType global        SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
    bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduleAwarePeriodic        DiscoveryType longlink2hop  SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
    bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduleAwarePeriodicDC      DiscoveryType longlink2hop  SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
    bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduleAwarePeriodicNoDisco DiscoveryType nulldiscovery SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
    bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduledPeriodicDC          DiscoveryType longlink2hop  SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
    bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduledPeriodic            DiscoveryType longlink2hop  SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
done
size=100
for beacon in 50000 600000; do
    bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 1 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType periodic                     DiscoveryType global        SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
    bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduleAwarePeriodic        DiscoveryType longlink2hop  SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
    bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduleAwarePeriodicDC      DiscoveryType longlink2hop  SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
    bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduleAwarePeriodicNoDisco DiscoveryType nulldiscovery SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
    bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduledPeriodicDC          DiscoveryType longlink2hop  SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
    bgxlimit 12 ./runOrDebug.sh   --clean-sim --suffix bryan PI -- DeadNeighborPeriod 2 Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduledPeriodic            DiscoveryType longlink2hop  SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility 
done
echo "ing on sizes and beacons"

popd
echo "Done"
