qReadMe for Applying the Patches to add Samraksh features and fixes
Author: Mukundan Sridhran

Apply the following four patches to dce. No need to apply the ns3-socket-fd-factory.cc.patch

1. wscript.patch
purpose: To add 'mobility' module to the dce scripts
Author: Mukundan Sridharan

$ ln -s ~/FixedWireless/Tuscarora/Patchs/wscript ~/dce/source/ns-3-dce/wscript


2. dce-env.cc.path
Purpose: Adds env variables to dce for node locations
Author: Bryan Lemon

$ ln -s ~/FixedWireless/Tuscarora/Patchs/dce-env.cc ~/dce/source/model/ns-3-dce/dce-env.cc



3. Sigio patch
Purpose: Created to implement SIGIO kernel signal:
Author: Mukundan Sridharan
Tested on: DCE1.1, DCE1.2
Created on: DCE1.2
Applies patch to models directory. Affects unix-fd.h, unix-fd.cc, unix-socket-fd.cc
Example Usage:

$ ln -s ~/FixedWireless/Tuscarora/Patchs/dce-env.cc
~/dce/source/model/ns-3-dce/

ln -s ~/FixedWireless/Tuscarora/Patchs/unix-fd.h ~/dce/source/ns-3-dce/model
ln -s ~/FixedWireless/Tuscarora/Patchs/unix-fd.cc ~/dce/source/ns-3-dce/model  
ln -s ~/FixedWireless/Tuscarora/Patchs/unix- socket.h ~/dce/source/ns-3-dce/model  
ln -s ~/FixedWireless/Tuscarora/Patchs/unix-  socket.cc ~/dce/source/ns-3-dce/model  

4. Memleak.patch (outdated)
Purpose: Create to fix the memory leak problem when running simulations using setitimer in dce
Author: Mukundan Sridharan
Tested on: DCE1.1, DCE1.2
Created on: DCE1.2
Applies patch to model/dce.cc
Example usage:

$ cd ns-3-dce
$ patch -p1 < Memleak.patch

