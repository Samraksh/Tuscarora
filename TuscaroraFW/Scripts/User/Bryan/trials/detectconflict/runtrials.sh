beacons="1000000 200000 100000 50000"

for beacon in $beacons; do
    if [ -e "./beacon$beacon.output.mmap" ]; then
	continue
    fi
    pushd ~/FixedWireless/Tuscarora > /dev/null
    ./runOrDebug.sh -i ~/tmp/detectconflict -size 100 -beacon $beacon -time 120 -dead 1 -range 100 -LEType 1 core || exit
    popd > /dev/null
    #cat ~/tmp/detectconflict/files-*/var/log/*/stdout | grep "Added new neighbor" | cut -d \  -f "8 12 14" > hbReceived; 
    #cat ~/tmp/detectconflict/files-*/var/log/*/stdout | grep "Updating existing" | cut -d \  -f "8 10 12" >> hbReceived; 
    #cat ~/tmp/detectconflict/files-*/var/log/*/stdout | grep "link estimation packet" | cut -d \  -f "11 13 14 15" > hbSent;
    #cat ~/tmp/detectconflict/files-*/var/log/*/stdout | grep "Print State" | cut -d\  -f "2 7 8 9" | awk '{gsub(/:/, "", $1); gsub(/,/, "", $2);gsub(/,/, "", $3);gsub(/,/, "", $4);gsub(/node-/, "", $2); gsub(/X:/, "", $3); gsub(/Y:/, "", $4); print}' > locs
    mv ~/tmp/detectconflict/output.mmap beacon$beacon.output.mmap
    mv ~/tmp/detectconflict/linkestimation.mmap beacon$beacon.linkestimation.mmap
done

pushd ../../MPP > /dev/null
make
popd > /dev/null

echo -n "" > total_colisions

for beacon in $beacons; do
    #$cp $beacon.sent hbSent
    #$cp $beacon.received hbReceived
    #$cp $beacon.locs locs
    pushd ../../MPP > /dev/null
    ./MPP ../trials/detectconflict/beacon$beacon.output.mmap conflict ../trials/detectconflict/beacon$beacon.linkestimation.mmap > ../trials/detectconflict/colisions
    numreceived=`./MPP ../trials/detectconflict/beacon$beacon.output.mmap numreceived ../trials/detectconflict/beacon$beacon.linkestimation.mmap`
    numbeacons=`./MPP ../trials/detectconflict/beacon$beacon.output.mmap numbeacons ../trials/detectconflict/beacon$beacon.linkestimation.mmap`
    popd
    #$time ./detectconflict.out hbSent hbReceived locs > colisions
    echo "$beacon $numreceived $(expr $numreceived +  $(wc -l colisions | cut -d\  -f1))" >> total_colisions
    echo "With a beacon rate of `expr 1000000 / $beacon`Hz, there were:"
    echo "  `cat colisions | cut -d\  -f"1 3" | uniq | wc -l | cut -d\  -f1` beacons were not heard by 1 or more neighbors"
    echo "  $numbeacons beacons were sent"
    echo "  `wc -l colisions | cut -d\  -f1` instances of a node not hearing a beacon when it was within range"
    echo "  The beacon with the most missed messages had `cat colisions | cut -d\  -f"1 3" | uniq -c | sort -nr | head -n 1 | cut  -c5-8`"
    echo "  While the median number of missed messages when there is atleast 1 missed message for a beacon is is `cat colisions | cut -d\  -f"1 3" | uniq -c | sort -n | sed "$(expr $(cat colisions | cut -d\  -f"1 3" | uniq -c | sort -n | wc -l) / 2),$(expr $(cat colisions | cut -d\  -f"1 3" | uniq -c | sort -n | wc -l) / 2)$(echo \!)d" | cut -c5-8`"
    echo "  There were $numreceived out of $(expr $numreceived +  $(wc -l colisions | cut -d\  -f1)) possible beacons received"
    echo ""
    gnuplot -e "parameter='`expr 1000000 / $beacon`Hz beacon'" -e "binwidth='`expr $beacon / 20`'" gnu.plot
    mv schedule.pdf $beacon.schedule.pdf
done

gnuplot gnutotal.plot
