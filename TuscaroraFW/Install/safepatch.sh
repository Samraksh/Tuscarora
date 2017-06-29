#!/bin/bash
# set -x

# Call as: $0 destinationDirectory patchfile
if [ "$#" -ne 2 ]; then
        echo "Illegal number of parameters"
        echo "Usage: $0 patch-dir patch-file"
        exit 1
fi

dir=$1
if [[ ! -d "$dir" ]]; then dir="$(dirname "$dir")"; fi
if [[ ! -d "$dir" ]]; then
    echo "$dir is not a directory."; exit 1
fi

patchfile=$2
if patch --dry-run -s -F 3 -N -d "${dir}" < "${patchfile}"; then
    # safe to patch
    patch -N -s -F 3 -b -d "${dir}" < "${patchfile}";
    cp ${patchfile} ${dir}
    echo "Patching is DONE."
    exit 0
fi
echo "Forward Dry-run failed, not applying patch. Let me try reversing.."

if patch --dry-run -NR --fuzz=3 -s -d "${dir}" < "${patchfile}"; then
    echo "Patch $patchfile is already applied."
    cp ${patchfile} ${dir}
else
    echo "Reversing dry-run also failed.. Something wrong with Patch $patchfile"
    exit 1
fi
echo
