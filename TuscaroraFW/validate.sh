#!/bin/bash 
# set -u # error if var is not initialized
# set -e # exit on error
# set -x # echo on

#echo "Executing $0 $@ in $(pwd)" 1>&2

# Run in parent dir, on exit go back to original dir
pushd "$(dirname $0)" > /dev/null
function cleanup { popd > /dev/null; }
trap cleanup EXIT

# Do arguments 
# http://stackoverflow.com/questions/17016007/bash-getopts-optional-arguments

# This script collects the available tests.
#TESTS=( $(${TUS}/Scripts/Tests/findValidationScripts --names ) )
export TUS="$(pwd)"
TESTS=( $(Scripts/Tests/findValidationScripts --names ) ) 

function join { perl -e '$s = shift @ARGV; print join("$s", @ARGV);' "$@"; }

# code borrowed from http://stackoverflow.com/questions/17016007/bash-getopts-optional-arguments
function Die()
{
    echo "$*"
    exit 1
}

function Usage()
{
cat <<-ENDOFMESSAGE
$0 [OPTION]          Run all tests.
$0 [OPTION] test     Run test.

options:
    -h --help                Display this message.
    -s --silent              Run silently.
    -c --clean               Clean before making.
    -k --keep-going          Don't stop on error (default)
    -a --all all             Run all the tests.
    -p|--arch|--platform     Specify the platform to validate for. Note: Individual test sctips/validations should
                             support the platform to successful validation.
                             Supported platforms: dce, x86_64, arm
    --duty-cycle             Run in duty cycle mode.
    --output-dir             Directory in which to place result files. Defaults to /tmp.
    --report-dir             Directory in which to place report. Default is stdout.
    --html                   Output in html. Takes html base directory as argument.

test-name: 
    all (runs all tests)
    $(echo -e "$(join "\n    " ${TESTS[@]})")

ENDOFMESSAGE
    exit 1
}

keep_going="1"

function GetOpts() {
	#arch="x86_64"
	arch="dce"
	clean=""
	silent=""
	tests=()
	tmpdir="/tmp"
	while [ $# -gt 0 ]
	do
		opt=$1
		shift
		case ${opt} in

			#general options
			-h|--help)
				Usage
				;;
				
	    -s|--silent)
				silent="1"
				;;
				
	    -c|--clean)
				clean="1"
				;;
				
	    -k|--keep-going)
				keep_going="1"
				;;
			
			-p|--arch|--platform)
				arch=$1;
				shift
				;;
	    # test selection
	    -a|--all|all)
				# silent="1"
				for t in "${TESTS[@]}"; do
						tests+=(${t})
				done
				;;
				
	    --output-dir)
				if [ $# -eq 0 -o "${1:0:1}" = "-" ]; then
												Die "The ${opt} option requires an argument."
										fi
				# see http://stackoverflow.com/a/1848456/268040
				tmpdir=${1%%/}
				shift
				;;
				
	    --report-dir)
				if [ $# -eq 0 -o "${1:0:1}" = "-" ]; then
												Die "The ${opt} option requires an argument."
										fi
				rptdir=${1%%/}
				shift
				;;
				
	    --html)
				html="1"
				;;
					dc|--duty-cycle)
				Die "Duty-cycle testing is not implemented yet"
				mode="duty-cycle"
				;;
				
	    ao) 
				# this is the default
				mode="always-on"
				;;
				
	    --)
				testArgs="$@"
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
				if echo "${TESTS[@]}" | grep -qw "${opt}"; then
												tests+=( ${opt} )
				else
												Die "${test}: unknown test."
				fi
				;;
			esac
		done 
}

GetOpts $*

if [ ${silent} ]; then 
    outputfile="/dev/null"; 
else 
    outputfile="/dev/stdout" 
fi

###Clean and compile asnp stack first
if [[ -n ${clean} ]]; then 
    make clean &> ${outputfile}
fi

# echo "tests = ${tests[@]}"
if [[ ${#tests[@]} -eq 0 ]]; then 
    tests=("${TESTS[@]}") 
    # echo -e "No tests given. Usage: \n" &> ${outputfile}
    # Usage;
    # Die "Exiting because no tests were given."
fi

timestamp() { date +%Y-%m-%d_%H:%M:%S; }
branch() {
    echo "$(git rev-parse --abbrev-ref HEAD 2> /dev/null)";
}
commit () { echo "$(git rev-parse HEAD 2> /dev/null)"; }


report() {
    if [[ -n "$html" ]]; then
	echo -e "<p> $(timestamp) $(branch) $@ </p>" >> "$reportfile";
    else
	echo -e "$(timestamp) $(branch) $@" >> "$reportfile";
    fi
}

passedTests=0
failedTests=0

if [[ -z "$rptdir" ]]
then reportfile="/dev/stdout";
else reportfile="$rptdir/$(timestamp)_$(branch)_report";
     mkdir -p $(dirname "$reportfile");
fi

echo "Validating on Platform: ${arch}, Branch: $(branch), Commit: ($(commit))" >> "$reportfile"

for test in "${tests[@]}"; do
	testout="$tmpdir/$(timestamp)_$(branch)_test-$test.txt"
	mkdir -p $(dirname "$testout");

	if [[ -n "$html" ]] && [[ -n "$rptdir" ]]; then
		htmltestout="$(python -c "import os.path; print os.path.relpath('${testout}', '${rptdir}')")"
		# echo "htmltestout=${htmltestout}, testout=${testout}, reportdir=${rptdir}"
	fi

	testArgs=( "-p ${arch}" ${testArgs} )
	echo "Going to validate ${test} with args ${testArgs}" >> $testout
	valscript=$(Scripts/Tests/findValidationScripts $test)
	cmd="${valscript} ${testArgs}"
	echo "Launching with commmand: ${cmd}" >> $testout
	
	#result=$(${cmd}) &> $testout
	$cmd &>> $testout
	ret_code=$?
	
	if [ ${ret_code} -eq 0 ] ; then
		#rm "$testout"
		if [[ -z "$html" ]]; then
				report PASSED $test $testout
		else
				report "<a style='color:green;' href='${htmltestout}'>PASSED ${test}</a>"
		fi
		(( passedTests += 1 ))
	else
		if [[ -z "$html" ]]; then
				report FAILED $test $testout
		else
				report "<a style='color:red;' href='${htmltestout}'>FAILED ${test}</a>"
		fi
		(( failedTests += 1 ))
		[ "${keep_going}" ] || break
	fi
done

if [[ -z "$rptdir" ]]; then
    echo "Passed $passedTests. Failed $failedTests."
elif [[ -n "$html" ]]; then
    mv "$reportfile" "${reportfile}_Passed-${passedTests}_Failed-${failedTests}.html"
else
    mv "$reportfile" "${reportfile}_Passed-${passedTests}_Failed-${failedTests}.txt"

fi
exit "$failedTests"
