runtrials() {
    for beacon in $period; do
	./runOrDebug.sh --clean-sim --suffix bryan PI -- Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon DeadNeighborPeriod 1 LinkEstimationType periodic DiscoveryType global SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility &
	sleep 5
	./runOrDebug.sh --clean-sim --suffix bryan PI -- Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon DeadNeighborPeriod 2 LinkEstimationType periodic DiscoveryType global SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility &
	sleep 5
	./runOrDebug.sh --clean-sim --suffix bryan PI -- Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon DeadNeighborPeriod 3 LinkEstimationType periodic DiscoveryType global SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility &
	sleep 5
	./runOrDebug.sh --clean-sim --suffix bryan PI -- Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduledPeriodic DiscoveryType oracle2hop SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility &
	sleep 5
	./runOrDebug.sh --clean-sim --suffix bryan PI -- Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduleAwarePeriodic DiscoveryType global SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility &
	sleep 5
	./runOrDebug.sh --clean-sim --suffix bryan PI -- Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduleAwarePeriodic DiscoveryType oracle2hop SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility &
	sleep 5
	./runOrDebug.sh --clean-sim --suffix bryan PI -- Size $size RunTime $time Range $range Density $density LinkEstimationPeriod $beacon LinkEstimationType scheduleAwarePeriodic DiscoveryType longlink2hop SpeedMin $speedmin SpeedMax $speedmax InterferenceModel $interference Mobility $mobility &
	sleep 5
    done
}
runit() {
    pushd ${TUS}
    #runtrials
    wait
    popd
    #./postprocess.sh
    rm d?-l? 2> /dev/null
    parameters="-e speed='$speed' -e nodes='$size'"
    for beacon in $period; do
	beacon2=$beacon
	
	periodic1=`ls /storage/simulations/Test_PI/Size_$size/Density_$density/RunTime_$time/Mobility_$mobility/DeadNeighborPeriod_1/LinkEstimationPeriod_$beacon/Range_$range/SpeedMin_$speedmin/SpeedMax_$speedmax/LinkEstimationType_periodic/InterferenceModel_RangePropagationLossModel/DiscoveryType_global/Verbose_0/PatternArgs_/Capacity_DsssRate11Mbps/RngSeed_1/RngRun_1/DisplayTextOutput_0/LogText_0/*.tar.gz | head -n 1`
	periodic2=`ls /storage/simulations/Test_PI/Size_$size/Density_$density/RunTime_$time/Mobility_$mobility/DeadNeighborPeriod_2/LinkEstimationPeriod_$beacon/Range_$range/SpeedMin_$speedmin/SpeedMax_$speedmax/LinkEstimationType_periodic/InterferenceModel_RangePropagationLossModel/DiscoveryType_global/Verbose_0/PatternArgs_/Capacity_DsssRate11Mbps/RngSeed_1/RngRun_1/DisplayTextOutput_0/LogText_0/*.tar.gz | head -n 1`
	periodic3=`ls /storage/simulations/Test_PI/Size_$size/Density_$density/RunTime_$time/Mobility_$mobility/DeadNeighborPeriod_3/LinkEstimationPeriod_$beacon/Range_$range/SpeedMin_$speedmin/SpeedMax_$speedmax/LinkEstimationType_periodic/InterferenceModel_RangePropagationLossModel/DiscoveryType_global/Verbose_0/PatternArgs_/Capacity_DsssRate11Mbps/RngSeed_1/RngRun_1/DisplayTextOutput_0/LogText_0/*.tar.gz | head -n 1`
    
	scheduled=`ls /storage/simulations/Test_PI/Size_$size/Density_$density/RunTime_$time/Mobility_$mobility/DeadNeighborPeriod_?/LinkEstimationPeriod_$beacon/Range_$range/SpeedMin_$speedmin/SpeedMax_$speedmax/LinkEstimationType_scheduledPeriodic/InterferenceModel_RangePropagationLossModel/DiscoveryType_oracle2hop/Verbose_0/PatternArgs_/Capacity_DsssRate11Mbps/RngSeed_1/RngRun_1/DisplayTextOutput_0/LogText_0/*.tar.gz | head -n 1`
    
	conflict1=`ls /storage/simulations/Test_PI/Size_$size/Density_$density/RunTime_$time/Mobility_$mobility/DeadNeighborPeriod_?/LinkEstimationPeriod_$beacon/Range_$range/SpeedMin_$speedmin/SpeedMax_$speedmax/LinkEstimationType_scheduleAwarePeriodic/InterferenceModel_RangePropagationLossModel/DiscoveryType_global/Verbose_0/PatternArgs_/Capacity_DsssRate11Mbps/RngSeed_1/RngRun_1/DisplayTextOutput_0/LogText_0/*.tar.gz | head -n 1`
	conflict2=`ls /storage/simulations/Test_PI/Size_$size/Density_$density/RunTime_$time/Mobility_$mobility/DeadNeighborPeriod_?/LinkEstimationPeriod_$beacon/Range_$range/SpeedMin_$speedmin/SpeedMax_$speedmax/LinkEstimationType_scheduleAwarePeriodic/InterferenceModel_RangePropagationLossModel/DiscoveryType_oracle2hop/Verbose_0/PatternArgs_/Capacity_DsssRate11Mbps/RngSeed_1/RngRun_1/DisplayTextOutput_0/LogText_0/*.tar.gz | head -n 1`
	conflict3=`ls /storage/simulations/Test_PI/Size_$size/Density_$density/RunTime_$time/Mobility_$mobility/DeadNeighborPeriod_?/LinkEstimationPeriod_$beacon/Range_$range/SpeedMin_$speedmin/SpeedMax_$speedmax/LinkEstimationType_scheduleAwarePeriodic/InterferenceModel_RangePropagationLossModel/DiscoveryType_longlink2hop/Verbose_0/PatternArgs_/Capacity_DsssRate11Mbps/RngSeed_1/RngRun_1/DisplayTextOutput_0/LogText_0/*.tar.gz | head -n 1`

	echo -n "`expr 1000000 / $beacon2` " >> d1-l1
	echo -n "`expr 1000000 / $beacon2` " >> d2-l1
	echo -n "`expr 1000000 / $beacon2` " >> d3-l1
	echo -n "`expr 1000000 / $beacon2` " >> d2-l2
	echo -n "`expr 1000000 / $beacon2` " >> d1-l3
	echo -n "`expr 1000000 / $beacon2` " >> d2-l3
	echo -n "`expr 1000000 / $beacon2` " >> d3-l3
	
	parameters="$parameters -e p1$beacon='`dirname $periodic1`' -e p2$beacon='`dirname $periodic2`' -e p3$beacon='`dirname $periodic3`'"
	parameters="$parameters -e c1$beacon='`dirname $conflict1`' -e c2$beacon='`dirname $conflict2`' -e c3$beacon='`dirname $conflict3`'"
	parameters="$parameters  -e s$beacon='`dirname $scheduled`'"
	
	cat $(dirname $periodic1)/abcd >> d1-l1
	cat $(dirname $periodic2)/abcd >> d2-l1
	cat $(dirname $periodic3)/abcd >> d3-l1
	cat $(dirname $scheduled)/abcd >> d2-l2
	cat $(dirname $conflict1)/abcd >> d1-l3
	cat $(dirname $conflict2)/abcd >> d2-l3
	cat $(dirname $periodic3)/abcd >> d3-l3
    done
    echo "Done"
    
    gnuplot $parameters link-discovery.plot
    gnuplot $parameters time-to.plot
    rm d?-l? > /dev/null
}

density=10
time=120
mobility='RandomWalk2dMobilityModel'
period="1000000 200000 100000"
range=560
speed=3
speedmin=2
speedmax=4
interference='RangePropagationLossModel'
size=100
runit

speed=50
speedmin=25
speedmax=75
runit

size=200
runit

size=300
runit

