
if [ $# -ne 2 ]; then
    echo "Two parameters required. Usage: $0 path_to_search file_extension(no .)"
    echo " Example: $0 /home pyc"
    exit
fi

echo "Removing following files..."
#First list all files
find $1 -name "*.$2" -exec ls -l "{}" \;

echo ""
#Then remove
find $1 -name "*.$2" -exec rm "{}" \;

