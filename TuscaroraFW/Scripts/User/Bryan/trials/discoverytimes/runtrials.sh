pushd ../../MPP
make || exit
popd

types="periodic scheduled avoidance"

for t in $types; do
    if [ ! -f "$t.mmap" ]; then
	pushd ../../../../
	if [ $t = "periodic" ]; then
	    ./runOrDebug.sh -i ~/tmp/periodic core -- Size 100 LinkEstimationPeriod 1000000 RunTime 120 DeadNeighborPeriod 1 Range 100 LinkEstimationType 1  || exit
	elif [ $t = "scheduled" ]; then
	    ./runOrDebug.sh -i ~/tmp/scheduled core -- Size 100 LinkEstimationPeriod 1000000 RunTime 120 DeadNeighborPeriod 1 Range 100 LinkEstimationType 2  || exit
	elif [ $t = "avoidance" ]; then
	    ./runOrDebug.sh -i ~/tmp/avoidance core -- Size 100 LinkEstimationPeriod 1000000 RunTime 120 DeadNeighborPeriod 1 Range 100 LinkEstimationType 3  || exit
	fi
	popd
	mv ~/tmp/$t/output.mmap $t.mmap
    fi

    pushd ../../MPP
    ./MPP ../trials/discoverytimes/$t.mmap debug > ../trials/discoverytimes/$t.txt
    ./MPP ../trials/discoverytimes/$t.mmap linksadded > ../trials/discoverytimes/linksAdded
    ./MPP ../trials/discoverytimes/$t.mmap linksremoved > ../trials/discoverytimes/linksRemoved
    popd

    gnuplot -e "type='$t'" -e "beaconrate='1000000'" gnu.plot

    mv link-subtractions.pdf $t-link-subtractions.pdf
    mv link-additions.pdf $t-link-additions.pdf
done
