#!/usr/bin/env bash
# set -x
export setup="$@"

(
    # Quit if we can't get the exclusivelock (fd 200), this means another process is running
    if ! flock -x --nonblock 200; then
	# echo exiting
	exit
    fi

    adjust_files() {
	# save log file (jobs.done) to the permanent log file (jobs.log)
	# because jobs.done gets overwritten every time parallel restarts.
	if [ -f jobs.done ]; then
	    cat jobs.done >> jobs.log
	    rm jobs.done
	fi

	# Refresh the jobs queue:
        # for each job (line in the jobs file),
	# check if it has been completed (if it's found in jobs.log),
	# if it hasn't been completed, then echo it to the tmp file,
	# then copy the tmp file back to the jobs file.
	if [ -f jobs ] && [ -f jobs.log ]; then
	    tmp="$(mktemp -p .)"
	    cat jobs | parallel 'grep -wq {} jobs.log || echo {}' > "$tmp"
	    mv "$tmp" jobs
	fi
    }

    runParallel() {
	# run jobs in parallel, this will overwrite the jobs.done
	# file, so we need to have saved it (in adjust_files).
	parallel --retries 12 --verbose --joblog jobs.done \
     		 --sshloginfile servers --tag "$@"
    }

    if [ -n "$setup" ]; then
	echo $setup | runParallel --onall
    fi
    
    adjust_files
    cat jobs | runParallel
    adjust_files
    echo runJobs.sh is done 
    
) 200>runJobs.sh.exclusivelock >> jobs.out 2>> jobs.err

