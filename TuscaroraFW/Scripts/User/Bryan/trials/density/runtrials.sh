densities="3 5 10 15 20"
LETypes="1 2 3"

for density in $densities; do
    for LEType in $LETypes; do
	if [ -e "./density$density-LEType$LEType.mmap" ]; then
	    continue
	fi
	pushd ~/FixedWireless/Tuscarora > /dev/null
	./runOrDebug.sh -i ~/tmp/density core -- Size 100 Density $density LinkEstimationPeriod 200000 RunTime 120 DeadNeighborPeriod 1 Range 100 LinkEstimationType $LEType || exit
	popd > /dev/null
	mv ~/tmp/density/output.mmap ./density$density-LEType$LEType.mmap
    done
done

pushd ../../MPP > /dev/null
make > /dev/null || exit
popd > /dev/null
echo -n "" > output
for LEType in $LETypes; do
    for density in $densities; do
	pushd ../../MPP > /dev/null
	./MPP ../trials/density/density$density-LEType$LEType.mmap details "$density" >> ../trials/density/output
	popd > /dev/null
    done
    echo >> output
    echo >> output
done

gnuplot gnu.plot
