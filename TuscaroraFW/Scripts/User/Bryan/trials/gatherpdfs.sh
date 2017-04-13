rm -f ~/tmp/pdfs/*.pdf
rm -f ~/tmp/pdfs/pdfs.zip
for dir in *; do
    if [ -d $dir ]; then
	echo "Generating for $dir"
	pushd $dir > /dev/null
	if [ $dir != "detectconflict" ]; then
	    ./runtrials.sh > /dev/null
	fi

	echo -n "Gathering "
	for file in *.pdf; do
	    echo -n "$file, "
	    cp $file ~/tmp/pdfs/$dir-$file
	done
	echo ""
	popd > /dev/null
    fi
done

pushd ~/tmp/pdfs
zip pdfs.zip *.pdf
