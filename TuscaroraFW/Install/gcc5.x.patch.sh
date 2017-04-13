sudo apt-get remove g++-5 gcc-5
sudo apt-get install g++-4.9 gcc-4.9

sudo rm /usr/bin/g++ /usr/bin/gcc
sudo ln -s /usr/bin/g++-4.9 /usr/bin/g++
sudo ln -s /usr/bin/gcc-4.9 /usr/bin/gcc

IPATH=$(pwd)
echo $IPATH
cd ~/dce/source/ns-3-dce/model

echo "Reversing the libc-ns3 patch...."
#patch --dry-run -R < $IPATH/../Patchs/model/libc-ns3.h.patch
patch -R < $IPATH/../Patchs/model/libc-ns3.h.patch
echo "Done."
echo "Applying new combined patch..."
#patch --dry-run -p1 < $IPATH/../Patchs/model/libc-ns3-dce.patch.notActive
patch -p1 < $IPATH/../Patchs/model/libc-ns3-dce.patch.notActive
echo "Done."
