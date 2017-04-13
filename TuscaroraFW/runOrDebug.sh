#!/bin/bash 
# set -u
set -e
# set -x

# check if a piped command fails, see http://stackoverflow.com/a/19804002/268040
set -o pipefail


echo "Executing $0 $@"

function join { perl -e '$s = shift @ARGV; print join("$s", @ARGV);' "$@"; }

# code borrowed from http://stackoverflow.com/questions/17016007/bash-getopts-optional-arguments
function Die()
{
    echo -e "$*"
    exit 1
}

# export BAKE_HOME=${HOME}/bake PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:${HOME}/bake:${HOME}/bake PYTHONPATH=:${HOME}/bake:${HOME}/bake
# export NS3_DCE=${HOME}/dce/source/ns-3-dce
# export DCE_DIR=${HOME}/dce
# export DCE_RAW_DIR=${HOME}/dce/source/ns-3-dce/raw
export TUS="$(pwd)"

# check if the dce environment variables are set, if not to try to fix.
#env | grep -i dce > /dev/null || (echo dce env missing; )
function CheckDCE(){
	env | grep -i dce > /dev/null || Die "DCE environment variables are not set in this shell. Please install DCE or restart bash. "
}



# Do arguments 
# http://stackoverflow.com/questions/17016007/bash-getopts-optional-arguments

# This script collects the available tests.
# all_dce_tests=( $(${TUS}/Scripts/Tests/findTests . --names ) ) 
# all_dce_dc_tests="$(${TUS}/Scripts/Tests/findTests --names | perl -lane 'for (@F) {print if /\bDC/;}')"
find_all_dce_tests(){ echo "${all_dce_tests:=$(Scripts/Tests/findTests . --names )}"; } 
find_all_dce_dc_tests(){ echo "${all_dce_dc_tests:=$(find_all_dce_tests | perl -lane 'for (@F) {print if /\bDC/;}')}"; }
# echo "dcTests = ${dcTests}"

find_all_ns3_tests() { echo "${all_ns3_tests:="$(Scripts/Tests/findNS3Tests)"}"; }
default_ns3_test="tuscarora-test"

# storage directory for simulations
sim_storage=~/storage/simulations

findDebugFlags(){ echo "${debugFlags:=$(ack -h --output='$1' '\bDBG_(\w+)' Lib/Misc/Debug.h | sort | uniq)}"; }

# test if debugFlags are defined
testDebugFlag() {
    dbgFlag=$1
    # pwd
    # use --nofilter to play nice with parallel http://superuser.com/questions/747303/gnu-parallel-and-ack-not-playing-nicely-due-to-stdin-pipe
    if ! ack --nofilter "^#define\s+DBG_${dbgFlag}\s+\S" > /dev/null;
    then echo "Debug flag $dbgFlag is not defined."
    # else echo "Debug flag $dbgFlag is defined."
    fi
}
export -f testDebugFlag
testDebugFlags() { parallel testDebugFlags {} ::: $(findDebugFlags); }

function columns() {
    # Print the arguments in columns.
    # The first three arguments are for left-offset, width, number in a row. Provided here.
    perl -E 'my $l = shift; my $f = shift; my $n = shift; $col = 0; for (@ARGV) { if ($col == 0) { print " " x $l, $_; } elsif ( $col + length $_ > $f * $n ) { $col = 0; print "\n", " " x $l, $_; } else { print; } $col += length $_; do {print " ";} while ++$col % $f; } print "\n";' 8 18 4 "$@"
}

function Usage() {
cat <<-ENDOFMESSAGE
$0 [OPTIONS] test-name [-- test options ]

options:
    -h --help        Display this message. If a pattern is specified, 
                     the arguments that the pattern expects are given.
    --arch|-p|--platform    Compile and run on specified platform
                     Options: dce or x86_64 or arm
    -s --silent      Run silently.
    -v --verbose     Run verbosely. Useful for debugging this script.
    -c --clean       Clean before making.
    --keep-sim       Keep simulation results even if they've been copied
                     to storage.
    -k --keep-going  Don't stop if a test fails, keep going.
    -i               Run in isolation mode, allows multiple runs to 
                     happen simulatniously. Isolation dir is set automatically.
    --isolate        Like -i, but expects an argument to use as the isolation dir.
    --store          Try to store results in $sim_storage. Also sets isolate.
    --storage-dir    Specify a different storage directory than $sim_storage. 
                     Also sets store and isolate.
    -d --debug       Debug with gdb.
    -DDBG_FLAG[=val] Set DBG_* flag to tagging Debug_Printf statements. Available flags are:
$(columns $(findDebugFlags))
    --valgrind       Run with valgrind.
    --co --compile-only   Just compile the test, but don't run.
    --no-compile     Don't compile the test, just run it.
    -f --force       Force a repeat simulation.

    --all all     Run all the tests.
    -dc --duty-cycle Run in duty cycle mode.

   --ns3 test        Run a different ns3 test. 
                     The default is ${default_ns3_test}. 
                     Permissable values are: 
$(columns $(find_all_ns3_tests))
    --ns3-only       Run only in ns3. Don't require a test-name.
    --pure-ns3 test  Like --ns3 but also sets the ns3-only option.
    
    --suffix         Suffix to add to the tar result files (useful 
                     for adding time and user info.)
    tests            (Use test name "all" to run all tests.) Individual tests are:
$(columns $(find_all_dce_tests))
    -- [args]        Arguments given after '--' are passed to the 
                     ns3/dce program unchanged.

Examples:

# Print this help message
$0 -h

# Run the Cop test with 3 nodes for 6 seconds with the Braadcast
# option. Broadcast is a Pattern-level option so it is set after the
# second double-hyphen.
$0 Cop -- Size 3 RunTime 6 -- Broadcast

ENDOFMESSAGE

}

MAKE() {
    # Parallelize make to the number of cores. 
    # This makes compilation on my server 6 times faster. 
    # See the following for the magic:
    #  - number of cores - http://www.howtogeek.com/howto/ubuntu/display-number-of-processors-on-linux/
    #  - rounding - http://stackoverflow.com/questions/19322233/rounding-the-result-using-dc-desk-calculator
    cores="$(cat /proc/cpuinfo | grep processor | wc -l)"
    cores="$(echo "${cores} 1.5 * 0k1/ p" | dc)"
    
	#mainfile=$(./Scripts/Tests/findTests . --filename $@)
    mainfile=$(find "./Tests" -name "*run$@.cpp")
    testDir=$(dirname $mainfile)
    echo "Test sources in $testDir main in file $mainfile";

    cmakeoptions="-DTarget=${@} -DTestDir=$testDir"
	
    if [[ -n $debug ]]
    then cmakeoptions="-DCMAKE_BUILD_TYPE=Debug ${cmakeoptions} "
    fi

    #if [ "${TUS_CUR_ARCH}" != "$platform" ]; then 
    #    rm CMakeCache.txt
    #    rm -r CMakeFiles
    #	echo "Change is target architecture detected. Removed CMake cache files."
    #else
    #	echo "Same target architecture detected. Reusing cmake cache."
    #fi
    if [[ "$platform" == "x86_64" ]]; then
			#rm CMakeCache.txt
			#rm -r CMakeFiles
			cmakeoptions="-Darch=x86_64 -DPLATFORM=linux ${cmakeoptions}"
			echo "cmake ${cmakeoptions}"
 			cmake ${cmakeoptions}; 	make -j4
			#make -f Makefile.mp -j${cores} USER_DEFINES="${USER_DEFINES}" "$@"
    elif [[ "$platform" == "arm" ]]; then 
			#rm CMakeCache.txt
			#rm -r CMakeFiles
			cmakeoptions="-Darch=arm -DPLATFORM=linux ${cmakeoptions}"
			echo "cmake ${cmakeoptions}"
			cmake ${cmakeoptions}; 	make -j4
    elif [[ "$platform" == "rpi" ]]; then
                        #rm CMakeCache.txt
                        #rm -r CMakeFiles
                        cmakeoptions="-Darch=rpi -DPLATFORM=linux ${cmakeoptions}"
                        echo "cmake ${cmakeoptions}"
                        cmake ${cmakeoptions};  make -j4
    elif [[ "$platform" == "dce" ]]; then 
			#rm CMakeCache.txt
			#rm -r CMakeFiles
			cmakeoptions="-Darch=dce -DPLATFORM=dce ${cmakeoptions}"
			echo "cmake ${cmakeoptions}"
			cmake ${cmakeoptions}; make -j4
			#make -f Makefile.dce -j${cores} USER_DEFINES="${USER_DEFINES}" "$@"
    else  
    	if [[ -n $debug ]];
    	then make --debug -j${cores} USER_DEFINES="${USER_DEFINES}" "$@"
    	else make -j${cores} USER_DEFINES="${USER_DEFINES}" "$@"
    	fi
    fi
}

clean() { # Clean the dce binaries and run make clean.
    echo "Cleaning . . . "
    rm -f "${DCE_DIR}/source/ns-3-dce/build/bin_dce/link-estimation-test"
    rm -f "${DCE_DIR}/source/ns-3-dce/build/bin_dce/tuscarora-test"
    make clean > /dev/null
}


function GetOpts() { # Get commandline options.
   # perl -pe 'print "Perl Perl\n"'
    branch=""
    platform="dce"
    #platform="x86_64"
    valgrind=""
    debug=""
    debugddd=""
    pure_ns3="0"
    mode="always-on"
    mobility="RandomWalk2dMobilityModel"
    silent=""
    isolate=""
    store=""
    #asnpargs=""
    ns3_tests=()
    dce_tests=()
    help=""
    outFile="";
    link_files=
    arm_srv_root="/srv/rootfs"
    arm_localDeployment="/home/Tuscarora/bin/"
    arm_deployment="${arm_srv_root}/${arm_localDeployment}"
    
    while [ $# -gt 0 ]
    do
	# save the first argument in $@ and then shift $@
	# example before: $@ = arg1 arg2 arg3
	#         after: $opt = arg1, $@ = arg2 arg3
	opt=$1
	shift
	case ${opt} in
	    
		# general options
		-h|--help)
			help=1;
			;;
		-s|--silent|-q|--quiet)
			silent="1"
			;;
		-v|--verbose)
			set -x
			;;
		-c|--clean)
			clean
			;;
		--keep-sim)
			keep_sim=1
			;;
		-k|--keepgoing|--keep-going)
			keep_going=true
			;;
		--arch|-p|--platform|-platform)
			platform=$1
			shift
			;;
		--storage-dir)
			if [ $# -eq 0 -o "${1:0:1}" = "-" ]; then
				Die "The ${opt} option requires an argument."
			fi
			sim_storage=$1
			shift
			store=1
			isolate=1
			;;
		--doNotCopy)
			# noOp
			;;
		--store)
			store=1
			isolate=1
			;;
		-i)
			isolate=1
			;;
		--isolate|--isolation-dir)
			if [ $# -eq 0 -o "${1:0:1}" = "-" ]; then
											Die "The ${opt} option requires an argument."
			fi
			isolationDir="$1"
			shift
			isolate=1
			;;
		-o|--output)
			outFile="$1"
			shift
			;;
		# debugging options
		debug|-d|--debug)
			if [ -e stl-views.gdb ]; then 
					gdb_options="-x $(pwd)/stl-views.gdb"
			fi
			debug="1"
			;;
		ddd|--ddd)
			debugddd="1"
			;;
		valgrind|--valgrind) 
			valgrind="1"
			;;
		--co|--compile-only)
			compile_only="1"
			;;
		--noc|--no-compile)
			no_compile="1"
			;;
		-f|--force)
			force="1"
			;;
		run)
			# this is a vestige of older versions of runOrDebug
			debug=""
			valgrind=""
			;;
		-D*) # debug flag
			# grab the value if set
				# add a trailing =
			val="${opt}="
				# strip off all characters left of =
			val="${val#*=}";
				# strip off trailing =, val now holds whatever value was passed.
			val="${val%=}";
			
			# remove -D prefix and optional DBG_ prefix and trailing set
			df="${opt#-D}"
			df="${df#DBG_}"
			df="${df%=*}"
			if ! echo "$(findDebugFlags)" | egrep -qw "${df}"; then
					echo "Could not find $df in: $(findDebugFlags)"
					exit
			elif [[ -n ${USER_DEFINES} ]]; then
					USER_DEFINES+=" -DDBG_${df}_=${val:=1}"
			else
					USER_DEFINES="-DDBG_${df}_=${val:=1}"
			fi
			# Debug flags are defined in
			# Platform/dce/PAL/Debug.cpp (not
			# Lib/Misc/Debug.h). Touch it to force make to
			# recompile.
			# touch Lib/Misc/Debug.h
			touch Platform/dce/PAL/Debug.cpp
			echo "USER_DEFINES=${USER_DEFINES}"
			;;
			
	    # test selection
		-a|--all|all)
			# silent="1"
			for t in $(find_all_dce_tests); do
					dce_tests+=(${t})
			done
			;;
		dc|--duty-cycle)
			mode="duty-cycle"
			;;
		ao) 
			# this is the default
			mode="always-on"
			;;
		ns3|--ns3)
			echo "Running a custom ns3 script"
			# check if there is another argument and that the argument doesn't begin with -.
			if [ $# -eq 0 -o "${1:0:1}" = "-" ]; then
											Die "The ${opt} option requires an argument. One of $(find_all_ns3_tests)"
									fi
			ns3_test_source="$1"
			ns3_tests+=( "$1" )
			shift
			find_all_ns3_tests | grep -w ${ns3_test_source} || \
					Die "NS3 test must be one of $(find_all_ns3_tests). You gave ${ns3_test_source}"
			;;
		pure-ns3|purens3|--pure-ns3|--purens3)
			if [ $# -eq 0 -o "${1:0:1}" = "-" ]; then
											Die "The ${opt} option requires an argument. One of $(find_all_ns3_tests)"
									fi
			pure_ns3="1"
			ns3_test_source="$1"
			ns3_tests+=( "$1" )
			shift
			find_all_ns3_tests | grep -w ${ns3_test_source} || \
					Die "NS3 test must be one of $(find_all_ns3_tests). You gave ${ns3_test_source}"
			# echo "Setting ns3_test_source to ${ns3_test_source}"
			;;
		--ns3-only|--ns3only)
			pure_ns3="1"
			;;
		--suffix)
			if [ $# -eq 0 ]; then
											Die "The ${opt} option requires an argument."
									fi
			suffix="$1"
			shift
			;;	    
		*=)
			# Trailing = means missing argument
			Die "Unset argument ${opt1}"
			;;
		--) 
			# echo "stop parsing at --"
			ns3_args="$@"
			echo "ns3_args=${ns3_args}"
			break
			;;
		--*=*|-?=*)
			echo "A)number of arguments $#"
			# echo "set an argument to a value with =, separate
			# arg from val, push both back on $@, and go through
			# the loop again."
			opt1=`echo "${opt}" | perl -pe 's/(.+)=.+/$1/'`
			opt2=`echo "${opt}" | perl -pe 's/.+=(.+)/$1/'`
			set -- "${opt1}" "${opt2}" "$@"
			;;

		--*) 
			# We've already parsed all the valid long arguments
			Die "Invalid long argument ${opt}"
			;;
		-??*)
			# We've already checked for --*, so these are multiple
			# arguments Split them in two, push them on $@ and go
			# through the loop again.  echo "multiple arguments"
			opt1=`echo "${opt}" | perl -pe 's/-(.).+/-$1/'`
			opt2=`echo "${opt}" | perl -pe 's/-.(.+)/-$1/'`
			set -- "${opt1}" "${opt2}" "$@"
			# echo "@ = $@"
			;;

		-*)
			Die "${opt}: unknown option."
			;;
		*)
			if find_all_dce_tests | grep -qw "$opt"; then
					dce_tests+=( "$opt" )
			elif find_all_ns3_tests | grep -qw "$opt"; then
					echo "adding $opt to ns3 tests."
					ns3_tests+=( "$opt" )
			else
					Die "$0: exiting because ${opt} is an unknown test."
			fi
			;;
		esac
	done 
}

GetOpts $*

if [ "${platform}" == "dce" ]; then
	echo "${platform}"
	CheckDCE
fi

if [ ${silent} ]; then 
	outputfile="/dev/null"; 
elif [ ${outFile} ]; then
	outputfile="${outFile}";
else 
	outputfile="/dev/stdout" 
fi

escapeSlash() { perl -le 'foreach (@ARGV) {s:/:__:g; print;}' $@; }
gitBranch() {
    pushd ${TUS} >& /dev/null
    git rev-parse --abbrev-ref HEAD
    popd >& /dev/null
}

simname () {
    # Echo a name for this simulation using a timestamp and
    # the ns3 test, the dce test, and the given arguments.
    local ns3_test=$1
    local dce_test=$2
    local ns3_args=$3
    local r="simdir.$(date +%Y%m%d-%H%M%S).branch_$(gitBranch).ns3_${ns3_test}.dce_${dce_test}"
    if [[ -n $ns3_args ]]; then
	local r="$r.${ns3_args// /_}";
    fi
    echo "$(escapeSlash "$r")"
}


compile() {
	#Compile the dce tests. If called with a none-zero argument,
	#then compile and run a test for its help output.
	local test="$1"
	#local platform="$2"
	for t in "${dce_tests[@]}"; do
	makeOutput="$(mktemp)"
	if MAKE ${t} 2>&1 | tee "$makeOutput"; then 
	  if [ -n "$test" ]; then
			echo "Running ./${t} --help"
			env NODEID=0 "./${t}" --help
			# gdb --args "./${t}" --help
			echo "Removing executable."
			rm -v "${t}"
	  else
			rm "$makeOutput"
			echo "Compilation Success!" >> ${outputfile}
			if [[ "$platform" == "dce" ]]; then
				mkdir -p $DCE_DIR/build/bin_dce/ 
				cp "${t}" "${DCE_DIR}/source/ns-3-dce/build/bin_dce/"
				cp "${t}" "${DCE_DIR}/build/bin_dce/"
				echo "Copied file ${t} into DCE bin folder" >> ${outputfile}
			fi 
	  fi
	else
		# cat "$makeOutput" >> "${outputfile}"
		echo "Compilation failed!!" >> ${outputfile}
		rm "$makeOutput"
		exit 1
	fi
  done
}


if [ -n "$help" ]; then
    if [ ${#dce_tests[@]} -gt 0 ]; then
	echo "running help on ${dce_tests[@]}"
	compile 1 # compile and test
    else
	Usage
    fi
    exit 0
fi

get_description() {
    # Put the output dir in the global variable "simdir".
    local ns3_test=$1
    local ns3_args=$2
    local name=$3

    # echo "get_description ns3_test=$ns3_test ns3_args=$ns3_args name=$name"
    
    if [ ! -e "$ns3_test" ]; then
	Die "Can't find $ns3_test"
    fi
    ./${ns3_test} WriteDescription ${ns3_args} &>> ${outputfile}
    local descriptionFile=files-0/simulation_description
    if [ -f "${descriptionFile}" ]; then
	cp "${descriptionFile}" simulation_description
	export simdir="$(cat ${descriptionFile})"
	echo ; echo "simulation parameters = $simdir"; echo
	if [[ -n "$store" ]]; then
	    export tarball="$name.tar"
	    echo "tarball=$tarball"; echo
	    export compressed_simulation="${tarball}.gz"
	    # echo "compressed_simulation=${compressed_simulation}"; echo
	fi
    else
	echo "Error getting description: No description written."
	return 1
    fi
}

move_results_to_storage() { 
    if [ ! -d "${sim_storage}" ]; then
	echo "Can't access storage."
	echo
	return
    elif [ ! -f "${compressed_simulation}" ]; then
    	echo "No compressed simulation to move."
    else
	mkdir -p "${sim_storage}/${simdir}"
	cp "${compressed_simulation}" "${sim_storage}/${simdir}"
	ln -f "${sim_storage}/${simdir}/${compressed_simulation}" "${sim_storage}"
	cp "files-0/simulation_description" "${sim_storage}/${simdir}"
	copiedToStorage=1
	echo copied to storage
	echo
    fi
}

compress_simulation() {
    # Compress a simulation that has already run.
    # Use the global variable "tarball" which has already been set
    # to the name of the resulting tarball.
    if [ "$tarball" ]; then
	echo "compressing simulation"
	for f in simulation_description time.output raw CourseChangeData.txt elf-cache SimMeasure raw files-*; do
	    if [[ -e "$f" ]]; then filesToSave="$filesToSave $f"; fi
	done
	tar cf "$tarball" $filesToSave
	gzip -f "$tarball"
    fi
}

runArm() { # $1 = test, $2 = args, $3 = isolation_name
  local test=$1
  local args=$2
  local name=$3
  # echo "runLinux ns3_test=$ns3_test ns3_args=$ns3_args name=$name"

	mkdir -p {arm_deployment}
	if ! cp ${test} ${arm_deployment} ; then
		echo "Copying binary to device failed. Check if device is connected to VM."
		exit 1
	fi
	scriptFile="${arm_deployment}/run_${test}.sh"
	echo "pushd ${arm_deployment}" > ${scriptFile}
	echo "/usr/bin/time ./${test} ${args} 2>&1 & "  >> ${scriptFile}
	echo "echo \"Started the ${test} as background process.\n\n\" "  >> ${scriptFile}
	echo "popd" >> ${scriptFile}
	chmod 755 ${scriptFile}
	
	echo "Copied the binary and run script file to device at ${arm_localDeployment}. Run the script file on the device"
} # end runArm

runLinux() { # $1 = test, $2 = args, $3 = isolation_name
  # Run ns3 test with arguments.
  if [[ -n ${isolate} ]] && [[ "$isolate" != "." ]]; then
		if [[ -n ${debug} ]] || [[ -n ${valgrind} ]]; then
	    echo "Cannot call runOrDebug with -isolate (=$isolate) and -debug (=$debug) or -valgrind (=$valgrind)" &> ${outputfile}
	    exit 1
		fi
	fi

  local test=$1
  local args=$2
  local name=$3
  # echo "runLinux ns3_test=$ns3_test ns3_args=$ns3_args name=$name"

	if [ ${debugddd} ]; then
		ddd ${test} ${args} &>> ${outputfile}
	elif [ ${debug} ]; then
		gdb ${test} ${args} &>> ${outputfile}
	elif [ ${valgrind} ]; then
		valgrind -v --tool=memcheck --leak-check=full --show-reachable=yes %s ${args} &>> ${outputfile}
	else
		echo "Running the test NATIVELY..."
		if ! /usr/bin/time -v ./${test} ${args} 2>&1 | \
			tee "time.output" &>> ${outputfile} ; then 
			echo "${test} failed."
			exit 11
		fi
	fi
	
} # end runLinux


runNS3() { # $1 = ns3_test, $2 = ns3_args, $3 = isolation_name
	# Run ns3 test with arguments.
	if [[ -n ${isolate} ]] && [[ "$isolate" != "." ]]; then
		if [[ -n ${debug} ]] || [[ -n ${valgrind} ]]; then
			echo "Cannot call runOrDebug with -isolate (=$isolate) and -debug (=$debug) or -valgrind (=$valgrind)" &> ${outputfile}
			exit 1
		fi
	fi

	local ns3_test=$1
	local ns3_args=$2
	local name=$3
	# echo "runNS3 ns3_test=$ns3_test ns3_args=$ns3_args name=$name"

	pushd "${DCE_DIR}/source/ns-3-dce" &> "${outputfile}"
	if [ ${debugddd} ]; then
		./waf --run ${ns3_test} --command-template="ddd --eval-command='handle SIGUSR1 noprint' --args %s ${ns3_args}" &>> "${outputfile}"
		elif [ ${debug} ]; then
		rm -rf files-*/
		./waf --run ${ns3_test} --command-template="gdb --eval-command='handle SIGUSR1 noprint' --args %s ${ns3_args}" &> "${outputfile}"
		echo "Creating links."
		parallel --no-notice 'test -e {}/stdout || ln $(find {} -name stdout | head -n1) {}' ::: files-*
		elif [[ -z ${isolate} ]]; then
		rm -rf files-*/
		./waf --run "${ns3_test} ${ns3_args}"
		RUN_STATUS=$?
		echo "Finished with stateus $RUN_STATUS"
		echo "Creating links."
		parallel --no-notice 'test -e {}/stdout || ln $(find {} -name stdout | head -n1) {}' ::: files-*
		elif [ ${valgrind} ]; then
		rm -rf files-*/
		./waf --run ${ns3_test} --command-template="valgrind -v --tool=memcheck --leak-check=full --show-reachable=yes %s ${ns3_args}" &>> "${outputfile}"
		echo "Creating links."
		parallel --no-notice  'test -e {}/stdout || ln $(find {} -name stdout | head -n1) {}' ::: files-*
		else
		echo "Building ns3/dce"
		if [ -z "${no_compile}" ]; then
			wafOutput=$(mktemp)
			if ./waf &> "$wafOutput"; then
			echo "Waf successful."
			else
			cat "$wafOutput" >> "${outputfile}"
			echo "Waf unsuccessful"
			rm "$wafOutput"
			exit 1
			fi
			rm "$wafOutput"
		fi

		if [[ ! -d "$isolate" ]]; then
			echo "Making isolation directory $isolate"
			mkdir -p "${isolate}"
		fi
		cp $DCE_DIR/source/ns-3-dce/build/myscripts/TuscTest/bin/${ns3_test} $isolate/${ns3_test}
		pushd "$isolate" > /dev/null
		rm -rf files-*/ raw/* SimMeasure/*
		mkdir -p raw
		mkdir -p SimMeasure

		# here we could check if the simulation has already been run.
		if get_description "${ns3_test}" "${ns3_args}" "$name" && [[ -n ${store} ]]; then
			if [[ -d "${sim_storage}/${simdir}" ]] && [[ -z "${force}" ]]; then
			echo "The results have already been stored. Use -f to override."
			echo "See previous results in ${sim_storage}/${compressed_simulation}."
			echo "Use --force to rerun. (${ns3_test} ${ns3_args})"
			exit
			fi
		fi

		if ! /usr/bin/time -v ./${ns3_test} ${ns3_args} 2>&1 | \
				tee "time.output" &>> ${outputfile} ; then 
			echo "${test} failed."
			exit 11
			# [[ "${keep_going}" ]] || exit 1
		fi

		# create links to stdout
		parallel --no-notice 'ln -f $(find {} -name stdout | head -n1) {}' ::: files-*

		compress_simulation
		if [[ -n $store ]]; then move_results_to_storage; fi

		if ack-grep -A 5 "Assert" files-*; then
			echo Assertion error
			exit 1;
		fi
		echo "$(hostname): Leaving simulation directory $(pwd)"
		popd > /dev/null # isolate
		if [ -n "$copiedToStorage" ] && [ -z "${keep_sim}" ] && [ -d "$isolate" ] && perl -e 'exit if $ARGV[0] =~ /(^|\/)simdir/; exit 1;' "$isolate" ; then
			echo "cleaning simulation (--clean-sim)"
			echo
			rm -r "$isolate"
		fi
	fi
	popd > /dev/null # dce dir
	# echo done runNS3
	echo
} # end runNS3

if [ ${pure_ns3} = 1 ]; then
    echo "This is a pure ns3 test, skipping tuscarora stack make..."
    if (( ${#dce_tests[@]} > 0 )); then
	Die "You shouldn't specify dce tests (${dce_tests[@]}) in pure ns3 mode. Exiting . . ."
    fi
    for t in "${ns3_tests[@]}"; do
	runNS3 $t;
    done
    exit
fi # end pure_ns3 case

# dce testing
if [ ${#dce_tests[@]} -eq 0 ]; then
    Usage
    Die "No tests given.\n" &>> ${outputfile}
fi

# check for duty cycle
# If the duty-cycle flag is set, we try to convert all the specified dce-tests to dc.
if [[ "$mode" = "duty-cycle" ]]; then
    tests=()
    for test in "${dce_tests[@]}"; do
	echo "processing test ${test}" >> ${outputfile}
	if find_all_dce_dc_tests | grep -qw "$test"; then
	    test+=( "$test" )
	elif find_all_dce_dc_tests | grep -qw "${test}.dc"; then
	    test+=( "${test}.dc" )
	else
	    echo "$test does not have a duty-cycle version."
	fi
    done
    dce_tests="${tests[@]}";
fi

# compile
if [ -z "${no_compile}" ]; then
    compile
fi

if [ "${compile_only}" ]; then
    echo "compile-only set, exiting"
    exit
fi


if (( "${#ns3_tests[@]}" == 0 )); then
    ns3_tests+=( "$default_ns3_test" )
fi

set_environment() {
    export DCE_DIR=~/dce
    export DCE=~/dce/source/ns-3-dce
    # export PNS=~/FixedWireless/PNS
    export NS3_MODULE_PATH=/usr/lib/gcc/x86_64-linux-gnu/4.6

    # I think these are needed:
    export DCE_PATH=${HOME}/dce/source/ns-3-dce/build/bin_dce:${HOME}/dce/build/sbin:${HOME}/dce/build/bin_dce:/usr/lib/gcc/x86_64-linux-gnu/4.6:${HOME}/dce/source/ns-3-dce/build/lib:${HOME}/dce/source/ns-3-dce/build/bin:${HOME}/dce/build/lib:${HOME}/dce/build/bin
    #NS3_EXECUTABLE_PATH=
    export LD_LIBRARY_PATH=/usr/lib/gcc/x86_64-linux-gnu/4.6:${HOME}/dce/source/ns-3-dce/build/lib:${HOME}/dce/source/ns-3-dce/build/bin:${HOME}/dce/build/lib:${HOME}/dce/build/bin
    export DCE_ROOT=${HOME}/dce/source/ns-3-dce/build:${HOME}/dce/build

    
    export PYTHONPATH=:${HOME}/dce/build/lib/python2.7/site-packages
    # NS3_MODULE_PATH
    # NS3_EXECUTABLE_PATH
    export NS3_DCE=${HOME}/dce/source/ns-3-dce
    export DCE_RAW_DIR=${HOME}/dce/source/ns-3-dce/raw
}

set_environment

for ns3_test in "${ns3_tests[@]}"; do
    for dce_test in "${dce_tests[@]}"; do
	simname="$(simname "$ns3_test" "$dce_test" "$ns3_args" )"
	if [[ -n $isolationDir ]]; then
	    isolate=$isolationDir;

	    # unset isolation dir, if this is run again, we don't want to overwrite.
	    isolationDir=

	elif [[ -n $isolate ]]; then
	    isolate="$simname"
	    echo "isolate = $isolate"
	else
	    # note that if multiple simulations are run, all the simulations except the first will be run in an isolated directory.
	    isolate="."
	    if [[ -d files-0 ]]; then
		echo Removing previous results . . .
		rm -r files-*
	    fi
	fi

	echo " "

	if [[ "$platform" == "dce" ]]; then
	echo "running simulation with ns3_test ${ns3_test}, dce_test ${dce_test}, arguments ${ns3_args}"
		runNS3 "${ns3_test}" "Test ${dce_test} ${ns3_args}" "$simname"
		echo "ran simulation with ns3_test ${ns3_test}, dce_test ${dce_test}, arguments ${ns3_args}"
	elif [[ "$platform" == "arm" ]]; then
		runArm "${dce_test}" "Test ${dce_test} ${ns3_args}" "$simname"
		echo "Created test files to run on ARM with ${dce_test}, arguments ${ns3_args}. Run the test manually on device."
	elif [[ "$platform" == "x86_64" ]]; then
		runLinux "${dce_test}" "${ns3_args}" "$simname"
		echo "Ran x86_64 Test with ${dce_test}, arguments ${ns3_args}"
	else 
		echo "Not running  the test. Created test binary. Initiate run seperately with ${dce_test} Binary on the target, wth arguments ${ns3_args}"
	fi
	
	echo
    done
done

#Set what we ran now, to check next time
export TUS_CUR_ARCH="$platform"
