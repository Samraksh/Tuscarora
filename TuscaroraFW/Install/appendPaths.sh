#!/bin/bash
# appendPaths() { # append a group of paths together, leaving out redundancies
    # use as: export PATH="$(appendPaths "$PATH" "dir1" "dir2")
    # start at the end:
    #  - join all arguments with :,
    #  - split the result on :,
    #  - pick out non-empty elements which haven't been seen and which are directories,
    #  - join with :,
    #  - print
perl -le 'print join ":", grep /\w/ && !$seen{$_}++ && -d $_, split ":", join ":", @ARGV;' "$@"
#}

