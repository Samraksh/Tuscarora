#!/bin/bash
# set -x

# Call as: $0 destinationDirectory patchfile

dir=$1
if [[ ! -d "$dir" ]]; then dir="$(dirname "$dir")"; fi
if [[ ! -d "$dir" ]]; then
    echo "$dir is not a directory."; exit 1
fi

patchfile=$2
if patch --dry-run -s -N -d "${dir}" < "${patchfile}"; then
    # safe to patch
    patch  -Nbd "${dir}" < "${patchfile}";
    cp ${patchfile} ${dir}
elif patch --dry-run -NR --fuzz=2 -s -d "${dir}" < "${patchfile}"; then
    echo "Patch $patchfile is already applied."
    cp ${patchfile} ${dir}
else
    echo "Patch $patchfile failed."
    exit 1
fi
echo
