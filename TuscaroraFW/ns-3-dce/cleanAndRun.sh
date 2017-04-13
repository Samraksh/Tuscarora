echo 'deleting files...'
rm -r files-*
echo 'Starting  run...'
./waf --run asnp-test
