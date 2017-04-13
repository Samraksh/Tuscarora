runit() {
    echo "Speed: $speedmin $speedmax Nodes: $size Period: $beacon"
    parameters="-e speed='$speed' -e nodes='$size' -e hz='`expr 1000000 / $beacon`' -e beacon='$beacon' -e seconds='$time'"
	
    periodic1=`ls /storage/simulations/Test_PI/Size_$size/Density_$density/RunTime_$time/Mobility_$mobility/DeadNeighborPeriod_1/LinkEstimationPeriod_$beacon/Range_$range/SpeedMin_$speedmin/SpeedMax_$speedmax/LinkEstimationType_periodic/InterferenceModel_RangePropagationLossModel/DiscoveryType_global/Verbose_0/PatternArgs_/Capacity_DsssRate11Mbps/RngSeed_1/RngRun_1/DisplayTextOutput_0/LogText_0/*.tar.gz | head -n 1`
    periodic2=`ls /storage/simulations/Test_PI/Size_$size/Density_$density/RunTime_$time/Mobility_$mobility/DeadNeighborPeriod_2/LinkEstimationPeriod_$beacon/Range_$range/SpeedMin_$speedmin/SpeedMax_$speedmax/LinkEstimationType_periodic/InterferenceModel_RangePropagationLossModel/DiscoveryType_global/Verbose_0/PatternArgs_/Capacity_DsssRate11Mbps/RngSeed_1/RngRun_1/DisplayTextOutput_0/LogText_0/*.tar.gz | head -n 1`
    periodic3=`ls /storage/simulations/Test_PI/Size_$size/Density_$density/RunTime_$time/Mobility_$mobility/DeadNeighborPeriod_3/LinkEstimationPeriod_$beacon/Range_$range/SpeedMin_$speedmin/SpeedMax_$speedmax/LinkEstimationType_periodic/InterferenceModel_RangePropagationLossModel/DiscoveryType_global/Verbose_0/PatternArgs_/Capacity_DsssRate11Mbps/RngSeed_1/RngRun_1/DisplayTextOutput_0/LogText_0/*.tar.gz | head -n 1`
    
    scheduled=`ls /storage/simulations/Test_PI/Size_$size/Density_$density/RunTime_$time/Mobility_$mobility/DeadNeighborPeriod_?/LinkEstimationPeriod_$beacon/Range_$range/SpeedMin_$speedmin/SpeedMax_$speedmax/LinkEstimationType_scheduledPeriodic/InterferenceModel_RangePropagationLossModel/DiscoveryType_longlink2hop/Verbose_0/PatternArgs_/Capacity_DsssRate11Mbps/RngSeed_1/RngRun_1/DisplayTextOutput_0/LogText_0/*.tar.gz | head -n 1`
    scheduleddc=`ls /storage/simulations/Test_PI/Size_$size/Density_$density/RunTime_$time/Mobility_$mobility/DeadNeighborPeriod_?/LinkEstimationPeriod_$beacon/Range_$range/SpeedMin_$speedmin/SpeedMax_$speedmax/LinkEstimationType_scheduledPeriodicDC/InterferenceModel_RangePropagationLossModel/DiscoveryType_longlink2hop/Verbose_0/PatternArgs_/Capacity_DsssRate11Mbps/RngSeed_1/RngRun_1/DisplayTextOutput_0/LogText_0/*.tar.gz | head -n 1`
    
    saca=`ls /storage/simulations/Test_PI/Size_$size/Density_$density/RunTime_$time/Mobility_$mobility/DeadNeighborPeriod_?/LinkEstimationPeriod_$beacon/Range_$range/SpeedMin_$speedmin/SpeedMax_$speedmax/LinkEstimationType_scheduleAwarePeriodic/InterferenceModel_RangePropagationLossModel/DiscoveryType_longlink2hop/Verbose_0/PatternArgs_/Capacity_DsssRate11Mbps/RngSeed_1/RngRun_1/DisplayTextOutput_0/LogText_0/*.tar.gz | head -n 1`
    sacadc=`ls /storage/simulations/Test_PI/Size_$size/Density_$density/RunTime_$time/Mobility_$mobility/DeadNeighborPeriod_?/LinkEstimationPeriod_$beacon/Range_$range/SpeedMin_$speedmin/SpeedMax_$speedmax/LinkEstimationType_scheduleAwarePeriodicDC/InterferenceModel_RangePropagationLossModel/DiscoveryType_longlink2hop/Verbose_0/PatternArgs_/Capacity_DsssRate11Mbps/RngSeed_1/RngRun_1/DisplayTextOutput_0/LogText_0/*.tar.gz | head -n 1`
    sacano=`ls /storage/simulations/Test_PI/Size_$size/Density_$density/RunTime_$time/Mobility_$mobility/DeadNeighborPeriod_?/LinkEstimationPeriod_$beacon/Range_$range/SpeedMin_$speedmin/SpeedMax_$speedmax/LinkEstimationType_scheduleAwarePeriodicNoDisco/InterferenceModel_RangePropagationLossModel/DiscoveryType_nulldiscovery/Verbose_0/PatternArgs_/Capacity_DsssRate11Mbps/RngSeed_1/RngRun_1/DisplayTextOutput_0/LogText_0/*.tar.gz | head -n 1`
    
    #echo `dirname $periodic1`
    #echo `dirname $periodic2`
    #echo `dirname $periodic3`
    #echo `dirname $scheduled`
    #echo `dirname $saca`
    parameters="$parameters -e su1='`dirname $periodic1`' -e su2='`dirname $periodic2`'    -e su3='`dirname $periodic3`'"
    parameters="$parameters -e saca='`dirname $saca`'     -e sacadc='`dirname $sacadc`'    -e sacano='`dirname $sacano`'"
    parameters="$parameters -e sa='`dirname $scheduled`'  -e sadc='`dirname $scheduleddc`'"
    
    gnuplot $parameters final.plot
}
rm *.pdf

beacons="1000000 200000 100000"

for beacon in $beacons; do
    density=10
    time=120
    mobility='RandomWalk2dMobilityModel'
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
    #runit
done

runit2() {
    letype=$1
    disco=$2
    filename=$3
    name=$4

    sizefiles=`find    /storage/simulations/Test_PI/Size_*/Density_10/RunTime_120/Mobility_RandomWalk2dMobilityModel/DeadNeighborPeriod_?/LinkEstimationPeriod_200000/Range_560/SpeedMin_25/SpeedMax_75/LinkEstimationType_$letype/InterferenceModel_RangePropagationLossModel/DiscoveryType_$disco/Verbose_0/PatternArgs_/Capacity_DsssRate11Mbps/RngSeed_1/RngRun_1/DisplayTextOutput_0/LogText_0/ -type d | tr [:space:] "\n" | sort -n -t_ -k3,3`
    sizes=''
    for size in $sizefiles; do
	sizes="$sizes `echo $size | cut -d/ -f5 | cut -d_ -f2`"
    done

    beaconfiles=`find  /storage/simulations/Test_PI/Size_100/Density_10/RunTime_120/Mobility_RandomWalk2dMobilityModel/DeadNeighborPeriod_?/LinkEstimationPeriod_*/Range_560/SpeedMin_25/SpeedMax_75/LinkEstimationType_$letype/InterferenceModel_RangePropagationLossModel/DiscoveryType_$disco/Verbose_0/PatternArgs_/Capacity_DsssRate11Mbps/RngSeed_1/RngRun_1/DisplayTextOutput_0/LogText_0/ -type d | tr [:space:] "\n" | sort -n -t_ -k8,8`
    beacons=''
    for beacon in $beaconfiles; do
	beacons="$beacons `echo $beacon | cut -d/ -f10 | cut -d_ -f2`"
    done

    densityfiles=`find /storage/simulations/Test_PI/Size_100/Density_*/RunTime_120/Mobility_RandomWalk2dMobilityModel/DeadNeighborPeriod_?/LinkEstimationPeriod_200000/Range_560/SpeedMin_25/SpeedMax_75/LinkEstimationType_$letype/InterferenceModel_RangePropagationLossModel/DiscoveryType_$disco/Verbose_0/PatternArgs_/Capacity_DsssRate11Mbps/RngSeed_1/RngRun_1/DisplayTextOutput_0/LogText_0/ -type d | tr [:space:] "\n" | sort -n -t_ -k4,4`
    densities=''
    for density in $densityfiles; do
	densities="$densities `echo $density | cut -d/ -f6 | cut -d_ -f2`"
    done

    speedfiles=`find   /storage/simulations/Test_PI/Size_100/Density_10/RunTime_120/Mobility_RandomWalk2dMobilityModel/DeadNeighborPeriod_?/LinkEstimationPeriod_200000/Range_560/SpeedMin_*/SpeedMax_*/LinkEstimationType_$letype/InterferenceModel_RangePropagationLossModel/DiscoveryType_$disco/Verbose_0/PatternArgs_/Capacity_DsssRate11Mbps/RngSeed_1/RngRun_1/DisplayTextOutput_0/LogText_0/ -type d`
    speedfiles=`echo $speedfiles | tr [:space:] '\n' | sort -n -t_ -k10,10`
    tspeeds=`echo "$speedfiles" | cut -d/ -f12,13 | tr [:space:] ' '`
    speeds=""
    echo
    echo $name
    for speed in $tspeeds; do
	speeds="$speeds $(expr $(expr `echo $speed | tr "SpeedMin_" " " | tr "SpeedMax_" " " | cut -d/ -f 1` + `echo $speed | tr "SpeedMin_" " " | tr "SpeedMax_" " " | cut -d/ -f 2` ) / 2)"
    done
    echo $sizes
    echo $beacons
    echo $densities
    echo $speeds
    
    gnuplot -e speeds="'$speeds'" -e speedfiles="'$speedfiles'" -e densityfiles="'$densityfiles'" -e densities="'$densities'" -e type="'$filename'" -e name="'$name'" -e sizes="'$sizes'" -e sizefiles="'$sizefiles'" -e beacons="'$beacons'" -e beaconfiles="'$beaconfiles'" final2.plot
}
runit2 "scheduleAwarePeriodic"        "longlink2hop"  "saca-longlink"    "SACA Long Link"
runit2 "scheduleAwarePeriodic"        "longlink2hop"  "saca-longlink"    "SACA Long Link"
runit2 "scheduleAwarePeriodicDC"      "longlink2hop"  "saca-longlink-dc" "SACA Long Link Duty Cycle"
runit2 "scheduledPeriodicDC"          "longlink2hop"  "sa-longlink-dc"   "SA Long Link Duty Cycle"
runit2 "scheduledPeriodic"            "longlink2hop"  "sa-longlink"      "SA Long Link"
runit2 "scheduleAwarePeriodicNoDisco" "nulldiscovery" "saca-nodisco"     "SACA No Discovery"

rm charts.zip
zip charts.zip *.pdf > /dev/null
