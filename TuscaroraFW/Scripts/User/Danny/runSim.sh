#!/usr/bin/env bash
set -x
echo "Running $0 $@"

jobsdir=FixedWireless/Tuscarora
jobsfile=jobs

suffix="-$(date +%F_%T | sed s/[-:]//g;)-$(whoami)"
echo "suffix = $suffix"
./bin/iterate.pl echo "$@" | \
    parallel -kq echo "cd $jobsdir; ./runOrDebug.sh --suffix $suffix {}" | \
    cat - >> $jobsfile
cat $jobsfile
# to run ssh in background, see http://stackoverflow.com/a/2831449/268040
# ssh -n -f c2e@l1 "cd $jobsdir; nohup ./runJobs.sh >> runJobs.out 2>&1 &"
./runJobs.sh >> runJobs.out 2>&1 &
