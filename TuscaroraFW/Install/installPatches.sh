#!/bin/bash
# set -x

if [ "$#" -ne 3 ]; then
	echo "Illegal number of parameters"
	echo "Usage: $0 TUS-dir dce-dir ns3-dir"
	exit 1
fi

TUS=$1
DCE_DIR=$2
NS3_DIR=$3

NS3_PATCH_DIR="$NS3_DIR/src"
DCE_PATCH_DIR="$DCE_DIR/source/ns-3-dce/"

echo $NS3_PATCH_DIR
echo $DCE_PATCH_DIR


dcePatchSrc="$TUS/Patchs"
ns3PatchSrc="$TUS/Patchs.ns3"
DCE_PATCHES="$(cd $dcePatchSrc; find . -name '*.patch')"
NS3_PATCHES="$(cd $ns3PatchSrc; find . -name '*.patch')"

#echo $DCE_PATCHES
#echo $NS3_PATCHES
echo "Installing ns3 patches..."

src_dir=$NS3_PATCH_DIR
for p in $NS3_PATCHES
do
  p_src="$ns3PatchSrc/$p"
  p_t="$src_dir/$p"
  echo "Patch target: $p_t"
  
  p_d=$(dirname $p_t)
  #patch --dry-run -Nd $p_d < $p_src
  ./safepatch.sh $p_d $p_src
  #sleep 
done

echo "Instaing dce patches.."
src_dir=$DCE_PATCH_DIR
for p in $DCE_PATCHES
do
  p_src="$dcePatchSrc/$p"
  p_t="$src_dir/$p"
  echo "Patch target: $p_t"
  
  p_d=$(dirname $p_t)
  #patch --dry-run -Nd $p_d < $p_src
  ./safepatch.sh $p_d $p_src
  #sleep 
done
