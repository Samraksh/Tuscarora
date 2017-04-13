speeds="3 7 10 20"
LETypes="1 2 3"

for speed in $speeds; do
    for LEType in $LETypes; do
	if [ -e "./speed$speed-LEType$LEType.mmap" ]; then
	    continue
	fi
	pushd ~/FixedWireless/Tuscarora > /dev/null
	./runOrDebug.sh core -i ~/tmp/speed -- Size 100 Density 10 Speed $speed LinkEstimationPeriod 200000 RunTime 120 DeadNeighborPeriod 1 Range 100 LinkEstimationType $LEType || exit
	popd > /dev/null
	mv ~/tmp/speed/output.mmap ./speed$speed-LEType$LEType.mmap
    done
done

pushd ../../MPP > /dev/null
make > /dev/null || exit
popd > /dev/null
echo -n "" > output
for LEType in $LETypes; do
    for speed in $speeds; do
	pushd ../../MPP > /dev/null
	./MPP ../trials/speed/speed$speed-LEType$LEType.mmap details "$speed" >> ../trials/speed/output
	popd > /dev/null
    done
    echo >> output
    echo >> output
done

gnuplot gnu.plot
