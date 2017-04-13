beacons="1000000 200000 100000 50000"
LETypes="1 2 3"

for beacon in $beacons; do
    for LEType in $LETypes; do
	if [ -e "./beacon$beacon-LEType$LEType.mmap" ]; then
	    continue
	fi
	pushd ~/FixedWireless/Tuscarora > /dev/null
	./runOrDebug.sh -i ~/tmp/beacon core -- Size 100 LinkEstimationPeriod $beacon RunTime 120 DeadNeighborPeriod 1 Range 100 LinkEstimationType $LEType || exit
	popd > /dev/null
	mv ~/tmp/beacon/output.mmap ./beacon$beacon-LEType$LEType.mmap
    done
done

pushd ../../MPP > /dev/null
make > /dev/null || exit
popd > /dev/null
echo -n "" > output
for LEType in $LETypes; do
    for beacon in $beacons; do
	pushd ../../MPP > /dev/null
	./MPP ../trials/beacon/beacon$beacon-LEType$LEType.mmap details "$beacon" >> ../trials/beacon/output
	popd > /dev/null
    done
    echo >> output
    echo >> output
done
gnuplot gnu.plot
