make || exit

mkdir -p ~/tmp/GTC

cp $1 ~/tmp/GTC/file.tar.gz
pushd ~/tmp/GTC > /dev/null
tar xf file.tar.gz
popd > /dev/null
./GTC ~/tmp/GTC/files-%d/configuration.bin ~/tmp/GTC/CourseChangeData.txt

rm -rf ~/tmp/GTC
