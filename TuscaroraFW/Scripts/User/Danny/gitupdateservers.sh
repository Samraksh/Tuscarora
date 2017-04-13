#!/usr/bin/env bash
# set -x

# If the user passes in a branch as an argument, use it, otherwise use the current branch.
if [ -n "$1" ]; then export branch="$1"
else export branch="$(git branch | perl -lne 'print $1 if /^\* (.+)/;')"
fi

echo "Updating to branch $branch."
test="CopB"
echo "Preparing for $test."

p() {
    tmp="$(mktemp)"
    if parallel --retries 12 --tag --slf allservers --nonall "$@" &> "$tmp"; then
	echo OK
	rm "$tmp"
	return 0
    else
	echo Error
	cat "$tmp"
	rm "$tmp"
	return 1
    fi
}

echo "Getting latest git version ..."
if p "cd FixedWireless/Tuscarora; git checkout ${branch}; git pull"; then
    echo "git pull OK"
else
    echo "git pull FAIL ($!)"
    exit 1
fi

echo "Cleaning dce ..."
if p 'cd FixedWireless/Tuscarora/dceln;  ./waf clean'; then
    echo "waf clean OK"
else
    echo "waf clean FAIL ($!)"
    exit 1
fi

echo "Building dce ..."
if p 'cd FixedWireless/Tuscarora/dceln; ./waf'; then
    echo "waf OK"
else
    echo "waf FAIL ($!)"
    exit 1
fi

echo "Compiling Tuscarora $test ..."
if p "cd FixedWireless/Tuscarora; ./runOrDebug.sh --clean --compile-only --silent $test"; then
    echo "runOrDebug $test OK"
else
    echo "runOrDebug $test FAIL ($!)"
fi
