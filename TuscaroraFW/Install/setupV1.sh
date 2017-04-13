echo ""
echo ""
echo "=============================================================================================="
echo "NOTE:You need to configure this script before running it. The local network/subnet mask needs to be set for nfs server"
echo "On the V1 device, the ip address of the server needs to be set using the environment variable"
echo "Script will sleep for 10 sec, exit now (Cntr-C) if you havent configured the script"
echo "=============================================================================================="
echo ""

sleep 10;

echo " -------------------  "
echo "Installing required packages...."

sudo apt-get update
sudo apt-get install libc6:i386 libncurses5:i386 libstdc++6:i386 zlib1g:i386 libc-dev:i386
sudo apt-get install xinetd tftpd nfs-kernel-server

echo " -------------------  "
echo "Installed required packages. Extracting server files..."

sudo 7z x ../Toolchains/arago-2013.12/srv.7z.001 -o/  
#sudo tar -zxvf ../Toolchains/arago-2013.12/srv.tgz -C / 

sudo chown -R nobody /srv
sudo chmod -R 777 /srv


echo " -------------------  "
echo "Extracted Server files.  Configuring up the services..."

#back up existing config file
sudo mv /etc/xinetd.d/tftp /etc/xinetd.d/tftp.old
sudo mv /etc/exports /etc/exports.old

echo "/srv/rootfs     192.168.2.0/24(rw,nohide,insecure,no_subtree_check,async)" | sudo tee /etc/exports

echo "service tftp
{
protocol        = udp
port            = 69
socket_type     = dgram
wait            = yes
user            = nobody
server          = /usr/sbin/in.tftpd 
server_args     = -s /srv/
disable         = no
}
" |sudo tee /etc/xinetd.d/tftp


echo " -------------------  "
echo "Finnished configuration of nfs and tftp server. Rebooting the services.."

sudo service xinetd restart
sudo service nfs-kernel-server restart
sudo update-rc.d xinetd enable 2

