#!/usr/bin/env bash 

# set -x # debug script
set -e # stop on error

# functions
Usage() {
 echo "./install.h [option]"
 echo "    -h : Display options";
 echo "    -u : uninstall current Tuscarora specific patches from ns3/dce installation";
 echo "    -r : Remove dce installtion"; 
}

Die() { echo "$*"; exit 1; }

checkLastAptUpdate() {
    # check last git update, see http://stackoverflow.com/a/8903280/268040
    secondsSinceLastUpdate=$(( $(date +%s) - $(stat -c %Y /var/lib/apt/periodic/update-success-stamp) ))
    secondsInADay="$(( 3600*24 ))"
    # echo seconds in a day = $secondsInADay
    # echo seconds since last update = $secondsSinceLastUpdate
    if (( $secondsSinceLastUpdate > $secondsInADay * 7 )); then
	echo "You should update because it's been more than a week. Run: sudo apt-get update."
    fi
}



findRootDir() { # find the directory of this script: http://stackoverflow.com/a/246128/268040
    SOURCE="${BASH_SOURCE[0]}"
    while [ -h "$SOURCE" ]; do # resolve $SOURCE until the file is no longer a symlink
	TARGET="$(readlink "$SOURCE")"
	if [[ $SOURCE == /* ]]; then
	    # echo "SOURCE '$SOURCE' is an absolute symlink to '$TARGET'"
	    SOURCE="$TARGET"
	else
	    DIR="$( dirname "$SOURCE" )"
	    # echo "SOURCE '$SOURCE' is a relative symlink to '$TARGET' (relative to '$DIR')"
	    SOURCE="$DIR/$TARGET" # if $SOURCE was a relative symlink, we need to resolve it relative to the path where the symlink file was located
	fi
    done
    # echo "SOURCE is '$SOURCE'"
    RDIR="$( dirname "$SOURCE" )"
    DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"
    # if [ "$DIR" != "$RDIR" ]; then
    # 	echo "DIR '$RDIR' resolves to '$DIR'"
    # fi
    # echo "DIR is '$DIR'"
    echo "$DIR"
}
# script starts here

checkLastAptUpdate


export BAKE_HOME=~/bake 
export TUS="$(findRootDir)"
export PATH="$(echo $PATH:${BAKE_HOME} | perl -F: -nale 'print join ":", grep { ! $s{$_}++ } @F;')"
export PYTHONPATH="$(${TUS}/Install/appendPaths.sh "$PYTHONPATH" "${BAKE_HOME}")"

find-cmd-in-path() { [[ -n $(echo "source $HOME/.bashrc && which $1" | env -i bash) ]]; }
find-dir-in-path() { echo "source $HOME/.bashrc; echo :$PATH: | grep -q :$1:" | env -i bash; }

find-dir-in-path "${BAKE_HOME}" ||
    ( echo "Adding bake and python path to your path via .bashrc";
      echo "$(echo -e "export PATH='$PATH' PYTHONPATH='$PYTHONPATH'" | \
        cat - ~/.bashrc)" > ~/.bashrc;
      find-dir-in-path "${BAKE_HOME}" || ( echo failed to update path; exit 1 );
    )


rm -rf ${TUS}/Install/t

if [ "$#" -gt 0 ]; then
    ###Display help
    if [ $1 = "-h" ]; then
        Usage;
        exit;
    fi;


    ###Uninstall previous installatio
    if [ $1 = "-u" ]; then
        echo "uninstalling the previous Tuscarora patches...";
        make -C "${TUS}/Install" t/uninstall;
        echo "Remove Tuscarora patches from ns3/dce installation";
        exit;
    fi;

    ###clean installation from scratch
    if [ $1 = "-r" ]; then
        echo "uninstalling the previous Tuscarora version....";
        make -C "${TUS}/Install" remove-dce;
        echo "Removed preivous installation";
        exit;
    fi;

fi;

make -C "${TUS}/Install" "$@" && (
    source ~/.bashrc
if [[ -z "$@" ]]; then # no arguments given, general case
echo "Testing by running gossip . . ."
./runOrDebug.sh -c -p dce Gossip
#echo "Skipping Gossip Test since it has not been ported yet"
fi; )

