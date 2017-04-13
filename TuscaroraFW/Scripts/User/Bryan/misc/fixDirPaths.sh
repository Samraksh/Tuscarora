pushd ../MPP
for file in `ls ~/storage/*/*/*/*/*/*/*/*.tar.gz`; do
    echo $file
    mkdir -p ~/tmp/fixit
    cp $file ~/tmp/fixit/
    pushd ~/tmp/fixit
    tar xzf `basename $file`
    popd
    dead=`./MPP 1 ~/tmp/fixit/files-%d/configuration.bin 1 config | cut -d\  -f 3`
    mkdir -p "`dirname $file`/$dead"
    mv $file `dirname $file`/$dead/
    rm -rf ~/tmp/fixit
done
popd
