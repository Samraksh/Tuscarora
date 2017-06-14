#!/bin/bash

#Author: Mukundan Sridharan, The Samraksh company
#Monitors the memory usage of a process and writes it to a output file

if [ "$#" -ne 3 ]; then
  echo "Illegal number of arguments"
  echo "Usage: $0 process-name-to-monitor period(in secs) Output-file"
  exit
fi
echo 'Starting memory monitoring with', $#,  'arguments'

echo 'PID ElapseTime smap-Rss smap-Pss ps-vsz ps-rss'>> $3 
sleep 2
echo 'Memory Usage Statistics:' 
echo 'PID ElapseTime smap-Rss smap-Pss ps-vsz ps-rss' 
pid=`pidof $1`
while [ $pid ]; do
  elapseTime=`ps -p $pid -o pid,etime | sed 1d | cut -c 1-17`
  rss=`echo 0 $(cat /proc/$pid/smaps  | grep Rss | awk '{print $2}' | sed 's#^#+#') | bc`
  pss=`echo 0 $(cat /proc/$pid/smaps  | grep Pss | awk '{print $2}' | sed 's#^#+#') | bc`
  vsz=`ps  -p $pid -o vsz |grep -Eo '[0-9]{0,9}'`
  ps_rss=`ps  -p $pid -o rss |grep -Eo '[0-9]{0,9}'`
  echo $elapseTime $rss $pss $vsz $ps_rss
  echo $elapseTime $rss $pss $vsz $ps_rss>> $3 
  sleep $2 
  pid=`pidof $1`
done
#echo 'Simulation finished'

