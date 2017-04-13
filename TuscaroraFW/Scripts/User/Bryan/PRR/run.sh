#/le/disco/speed/density/size/beacon/interference/file.tar.gz

density() {
    for ARG in "$@"
    do
	echo "Density $ARG `ls /storage/storage/1/1/1/$ARG/100/200000/FriisPropagationLossModel/*.tar.gz`"
	if [ ! -f d-$ARG ]; then
	    getit `ls /storage/storage/1/1/1/$ARG/100/200000/FriisPropagationLossModel/*.tar.gz` > d-$ARG
	fi
    done
}

size() {
    for ARG in "$@"
    do
	echo "Size $ARG `ls /storage/storage/1/1/1/8/$ARG/200000/FriisPropagationLossModel/*.tar.gz`"
	if [ ! -f s-$ARG ]; then
	    getit `ls /storage/storage/1/1/1/8/$ARG/200000/FriisPropagationLossModel/*.tar.gz` > s-$ARG
	fi
    done
}

getit() {
    mkdir ~/tmp/PRR
    cp $1 ~/tmp/PRR/file.tar.gz
    pushd ~/tmp/PRR/ > /dev/null
    tar xf file.tar.gz
    popd > /dev/null
    ./PRR ~/tmp/PRR/files-%d/configuration.bin ~/tmp/PRR/files-%d/linkestimation.bin ~/tmp/PRR/CourseChangeData.txt 10
    rm -rf ~/tmp/PRR
}

mkdir -p ~/tmp/PRR
rm -rf ~/tmp/PRR

density $1;
size $2;

gnuplot gnu.plot
