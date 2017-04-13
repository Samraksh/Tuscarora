#!/usr/bin/env bash
# set -x # for debugging

usage() {
    cat <<EOF
Usage: $0 ns3_program [args]
EOF
}

if [[ "$#" -lt "1" ]]; then
    echo "Must give an ns3 program."
    exit 1
fi

ns3_program="$1"
ns3_programs="$(${TUS}/bin/findNS3Tests)"

if ! echo "${ns3_programs}" | grep -qx "${ns3_program}"; then
    echo "${ns3_program} must be one of ${ns3_programs}"
    exit 1
fi

shift;

pushd $DCE_DIR/source/ns-3-dce > /dev/null
uid="$(${TUS}/bin/uniqid.sh)"
args="$@ Uid=${uid}"
./waf --run "${ns3_program} ${args}" > /dev/null 2>&1 
find files-0/ -name "${uid}" | sed "s/files-0//" | sed "s/${uid}//" | tail -n1

popd > /dev/null
