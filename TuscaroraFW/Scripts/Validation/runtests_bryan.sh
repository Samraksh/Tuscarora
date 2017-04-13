testInstallLocation() {
    assertTrue "Install DCE in ~/dce/source/ns-3-dce" "[ -d '${HOME}/dce/source/ns-3-dce' ]"
    assertTrue "Install FixedWireless in ~/FixedWireless" "[ -f '${HOME}/FixedWireless/Tuscarora/Scripts/Validation/runtests' ]"
}

testCompilation() {
    oldd=`pwd`
    cd ~/dce/source/ns-3-dce
    assertTrue "NS3/DCE did not compile" "./waf"
    cd $oldd

    cd ../../
    #make clean &> /dev/null
    assertTrue "Tuscarora did not compile" "make dce-test"
}

testMobility() {
    oldd=`pwd`
    cd ~/dce/source/ns-3-dce

    assertTrue "Cannot run mobility test for RandomWaypointMobilityModel" "./waf --run \"mobility-test --SpeedMin=10 --SpeedMax=10 --Range=10000 --Mobility=RandomWaypointMobilityModel\" > /tmp/mobilityWaypoint"
    assertTrue "Cannot run mobility test for RandomWalk2dMobilityModel" "./waf --run \"mobility-test --SpeedMin=10 --SpeedMax=10 --Range=10000 --Mobility=RandomWalk2dMobilityModel\" > /tmp/mobilityWalk"
    assertTrue "Cannot run mobility test for GaussMarkovMobilityModel" "./waf --run \"mobility-test --SpeedMin=10 --SpeedMax=10 --Range=10000 --Mobility=GaussMarkovMobilityModel\" > /tmp/mobilityGauss"
    
    assertSame "Error, node moved too slowly for Gauss" "10.000000" "`cat /tmp/mobilityGauss | sort | tail -n 1`"
    assertSame "Error, node moved too quickly for Gauss" "10.000000" "`cat /tmp/mobilityGauss | sort | head -n 1`"
    assertSame "Error, node moved too slowly for Waypoint" "10.000000" "`cat /tmp/mobilityWaypoint | sort | tail -n 1`"
    assertSame "Error, node moved too quickly for Waypoint" "10.000000" "`cat /tmp/mobilityWaypoint | sort | head -n 1`"
    assertSame "Error, node moved too slowly for Random Walk" "10.000000" "`cat /tmp/mobilityWalk | sort | tail -n 1`"
    assertSame "Error, node moved too quickly for Random Walk" "10.000000" "`cat /tmp/mobilityWalk | sort | head -n 1`"
    
    rm /tmp/mobilityGauss /tmp/mobilityWaypoint /tmp/mobilityWalk
    
    cd $oldd
}

testPeriodicBeaconing() {
    oldd=`pwd`
    cd ~/dce/source/ns-3-dce
    
    
    
    cd $oldd
}