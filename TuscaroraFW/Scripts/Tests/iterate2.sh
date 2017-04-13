#!/usr/bin/env bash

# iterate through all combinations of arguments.

lines=( $1 )
shift
for arg in "$@"; do
    lines2=()
    for variant in $arg; do
	for line in "${lines[@]}"; do
	    lines2+=( "$line $variant" )
	    # echo "line=$line, variant=$variant"
	done
    done
    # echo "arg=$arg, lines=${lines[@]}, lines2=${lines2[@]}"
    lines=( "${lines2[@]}" )
done

# for line in "${lines[@]}"; do echo "$line"; done

for line in "${lines[@]}"; do $line; done

