 

#echo Script name: $0
#echo $# arguments 

if [ $# -ne 2 ]; then 
    echo "Two parameters required. Usage: $0 original_string new_string"
    exit
fi

grep -r "$1" *
grep -r -l "$1" * | xargs sed -i 's,'"$1"','"$2"',g'
