doit() {
    f="$1"
    if [[ -f "$2/ld" && -f "$2/abcd" && -f "$2/tdn" && -f "$2/tdf" && -f "$2/lc" && -f "$2/lc-g" ]]; then return 0; fi
    mkdir -p ~/tmp/MPP
    cp $f ~/tmp/MPP/file.tar.gz
    pushd ~/tmp/MPP > /dev/null
    tar xzf file.tar.gz
    popd > /dev/null
    pushd ${TUS}/Scripts/Bryan/MPP > /dev/null
    #echo $2
    #echo "."
    if [ ! -f "$2/ld" ]; then
	./MPP ~/tmp/MPP/files-%d/output.bin ~/tmp/MPP/files-%d/configuration.bin ~/tmp/MPP/files-%d/linkestimation.bin linkdurration > "$2/ld" &
    fi
    if [ ! -f "$2/abcd" ]; then
	./MPP ~/tmp/MPP/files-%d/output.bin ~/tmp/MPP/files-%d/configuration.bin ~/tmp/MPP/files-%d/linkestimation.bin details "data" > "$2/abcd" &
    fi
    if [ ! -f "$2/tdn" ]; then
	./MPP ~/tmp/MPP/files-%d/output.bin ~/tmp/MPP/files-%d/configuration.bin ~/tmp/MPP/files-%d/linkestimation.bin linksadded > "$2/tdn" &
    fi
    if [ ! -f "$2/tdf" ]; then
	./MPP ~/tmp/MPP/files-%d/output.bin ~/tmp/MPP/files-%d/configuration.bin ~/tmp/MPP/files-%d/linkestimation.bin linksremoved > "$2/tdf" &
    fi 
    if [ ! -f "$2/lc" ]; then
	./MPP ~/tmp/MPP/files-%d/output.bin ~/tmp/MPP/files-%d/configuration.bin ~/tmp/MPP/files-%d/linkestimation.bin displaylinkevents > "$2/linkevents" &
	wait
	expr `cat $2/tdf | wc -l` + `cat $2/tdn | wc -l` > "$2/lc"
    fi
    popd > /dev/null
    if [ ! -f "$2/lc-g" ]; then
	touch "$2/lc-g"
	echo "1" > "$2/lc-g"
	if [ -f ~/tmp/MPP/CourseChangeData.txt ]; then
	    pushd ${TUS}/Scripts/Bryan/GTC > /dev/null
	    make > /dev/null
	    ./GTC ~/tmp/MPP/files-%d/configuration.bin ~/tmp/MPP/CourseChangeData.txt > "$2/lc-ground" &
	    popd > /dev/null
	fi
    fi
    wait
    echo -n "."
    rm -rf ~/tmp/MPP
}

doit2() {
    f="$1"
    pushd "$2" > /dev/null
    doit3 tdn &
    doit3 tdf &
    doit3 ld &
    wait
    popd > /dev/null
}

doit3() {
    if [ ! -f ${1}-q ]; then
	echo -n "_"
	touch ${1}-q
	cat $1 | sort -nr > ${1}-sorted
	for n in {1..100}; do
	    tail -n $(expr $(wc -l $1 | cut -d\  -f 1) \* $n / 100) ${1}-sorted | head -n 1 | cut -d\  -f 1 | tr -d '\n' >> ${1}-q
	    echo -n " " >> ${1}-q
	done
    fi
}

echo -n "Checking for duplicate files ... "
duplicates=`find /storage/simulations/Test_PI/ -type d -exec bash -c "echo -ne '{} '; find '{}' -maxdepth 1 -name *.tar.gz -type f | wc -l" \; | awk -F" " '$NF>1{print $1}'`
for duplicate in $duplicates; do
    echo
    echo $duplicate
    ls -lah $duplicate | grep "tar.gz"
done

if [ -n "$duplicates" ]; then
    echo "Fix duplicates"
    exit "1"
fi

echo "Done"

#echo -n "Repairing strange periods ... "
#for file in `find /storage/simulations/Test_PI -name "*1e+06" -type d`; do
#    mv $file ${file//1e+06/1000000}
#    echo -n "."
#done
#echo "Done"

echo -n "Doing Post Processing ... "
error=''
for file in `find /storage/simulations/Test_PI -name *.tar.gz`; do
    doit $file `dirname $file`
    if [ -n "$(find "`dirname $file`" -size 0 -type f)" ]; then
	error='1'
	echo
	echo -n `dirname $file`
	echo 
	ls -lah `dirname $file`
	rm `dirname $file`/abcd `dirname $file`/ld `dirname $file`/tdn `dirname $file`/tdf `dirname $file`/lc `dirname $file`/lc-g
	#rm -rf `dirname $file`
	continue
    fi
    doit2 $file `dirname $file`
done
echo "Done"

if [ -n "$error" ]; then
    echo "Error in the above post processing"
    exit 1
fi
