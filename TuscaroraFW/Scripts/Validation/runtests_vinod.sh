testRNG() {
    oldd=`pwd`
    cd ~/dce/source/ns-3-dce
    assertTrue "Could not generate the NS3 number list" "./waf --run RandomValue > /tmp/randomValueNs3"
    cd ~/FixedWireless/Tuscarora
    export NODEID=0
    assertTrue "Could not generate the Tuscarora number list" "./ASNPTest-dce pseudorandMRG > /tmp/randomValueTuscarora"
    assertSame "NS3 and Tuscarora are conflicting on the RNG" `md5sum /tmp/randomValueNs3 | cut -d ' ' -f 1` `md5sum /tmp/randomValueTuscarora | cut -d ' ' -f 1`
    rm /tmp/randomValueNs3
    rm /tmp/randomValueTuscarora
}