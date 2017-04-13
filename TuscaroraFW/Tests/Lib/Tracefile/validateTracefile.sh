#!/usr/bin/env bash
# set -x # Debug script
set -e # Break on error.

function Die()
{
    # Print exit message and exit shell
    # also print the trailing file as a convenience
    # example: Die "There was an error! Exiting." diagnosis_File
    echo "$*"

    # check if the 2nd argument is a file, if so, print it out
    [[ -f $2 ]] && echo "$2:" && cat "$2"
    exit 1
}

echo "Executing $0"
#export TUS="$(pwd)"
# echo "TUS=$TUS"

export VAL="$(dirname $0)"
# echo "validation dir = $VAL"

generateTrace() {
    ${VAL}/generateTraces.pl $@
    # ${TUS}/Tests/Lib/Tracefile/generateTraces.pl $@
}

export nodes=4
export seconds=4

# tracefile="$(mktemp -p ${TUS} tracefileXXXX)"
tracefile="$(mktemp)"
#testdir="${TUS}/dceln/tracetest"

testdir=$(mktemp -dtp ${TUS}/dceln/ "Tracefile.XXXXX")

if ! generateTrace $nodes $seconds > "$tracefile"; then
    echo "generateTrace Error: Could not generate trace."
    cat "$tracefile"
    exit 1
fi
[[ -f "$testdir" ]] && rm -r "$testdir"

runOutput="$(mktemp)"

pushd ${TUS}
if ./runOrDebug.sh -p dce --isolate "$testdir" FI -- \
		   Size $nodes RunTime $seconds \
		   Mobility TracefileMobilityModel \
		   Tracefile "$tracefile" >& "$runOutput"
then
    # no errors, remove tempfile holding output
    rm "$runOutput"
else
    Die "$0 Error: runOrDebug error: see simulation directory at $testdir, tracefile at $tracefile, and run output at $runOutput" \
	"$runOutput"
fi
    
# echo Tracefile
# cat "$tracefile"
# echo
# cat "${testdir}/CourseChangeData.txt"
popd

if ${VAL}/_validateCourseChanges.pl "${testdir}/CourseChangeData.txt"; then
    rm "$tracefile"
    rm -r "${testdir}"
    echo "UNITPASS"
else
	echo "UNITFAIL"
    Die "$0 Error: validation error."
fi
