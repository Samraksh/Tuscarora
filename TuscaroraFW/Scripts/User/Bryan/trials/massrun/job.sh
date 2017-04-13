le=$1
disco=1
dead=1

case $le in 
1)
le=periodic
disco=global
;;
2)
le=periodic
disco=global
dead=2
;;
3)
le=periodic
disco=global
dead=3
;;
4)
le=scheduledPeriodic
disco=oracle2hop
;;
5)
le=scheduleAwarePeriodic
disco=global
;;
6)
le=scheduleAwarePeriodic
disco=oracle2hop
;;
7)
le=scheduleAwarePeriodic
disco=longlink2hop
;;
esac

speed=$3
density=$4
beacon=$5
size=$6
jobnumber=$7
interference=$8

pushd ../../../../ > /dev/null
echo "LE $le, Disco $disco, speed $speed, density $density, beacon $beacon, size $size, $interference, dead $dead"
./runOrDebug.sh -i ~/tmp/massrun$jobnumber PI -- Size $size Range 560 Density $density Time 120 LinkEstimationPeriod $beacon DeadNeighborPeriod $dead LinkEstimationType $le DiscoveryType $disco SpeedMin $speed SpeedMax $speed InterferenceModel $interference  > /dev/null || exit 99
popd > /dev/null

pushd ~/tmp/massrun$jobnumber > /dev/null
rm -rf elf-cache raw SimMeasure tuscarora-test files-*/var
popd > /dev/null

tar -czf 1-$jobnumber.tar.gz -C ~/tmp/massrun$jobnumber . || exit 1;

rm -rf ~/tmp/massrun$7

mkdir -p ~/storage/$le/$disco/$speed/$density/$size/$beacon/$interference/$dead || exit 2;
cp 1-$jobnumber.tar.gz ~/storage/$le/$disco/$speed/$density/$size/$beacon/$interference/$dead || exit 3;

rm 1-$jobnumber.tar.gz
exit 0;
