set terminal pdf
set style data boxes
set style fill solid
set xlabel "Length of Link Durration"
set ylabel "Frequency"
hist(x,width)=width*floor(x/width)+width/2.0

set boxwidth 0.5
set yrange[0:2750]
set xrange[0:60]

set output 'link-durration-line-periodic-speed-'.speed.'-nodes-'.nodes.'.pdf'
set title "Frequency of Link Durrations Periodic Link Estimation\n".nodes." nodes, density 10, speed ".speed."mps, range 560m, 5hz period"
plot \
p1200000."/ld" u ((hist($1*20.0,2000))/1000-.5):(1.0) smooth freq w boxes t "1 Beacon",\
p2200000."/ld" u (hist($1*20.0,2000))/1000:(1.0) smooth freq w boxes t "2 Beacons",\
p3200000."/ld" u ((hist($1*20.0,2000)))/1000+.5:(1.0) smooth freq w boxes t "3 Beacons"

set output 'link-durration-line-sa-speed-'.speed.'-nodes-'.nodes.'.pdf'
set title "Frequency of Link Durrations Schedule Aware Link Estimation\n".nodes." nodes, density 10, speed ".speed."mps, range 560m, 5hz period"
plot \
s200000."/ld" u (hist($1*20.0,2000))/1000:(1.0) smooth freq w boxes notitle

set output 'link-durration-line-saca-speed-'.speed.'-nodes-'.nodes.'.pdf'
set title "Frequency of Link Durrations Conflict Avoidance Link Estimation\n".nodes." nodes, density 10, speed ".speed."mps, range 560m, 5hz period"
plot \
c1200000."/ld" u ((hist($1*20.0,2000)/1000-0.5)):(1.0) smooth freq w boxes t "Global",\
c2200000."/ld" u (hist($1*20.0,2000)/1000):(1.0) smooth freq w boxes t "Oracle",\
c3200000."/ld" u ((hist($1*20.0,2000)/1000+0.5)):(1.0) smooth freq w boxes t "Long Link"


set boxwidth 0.005
set yrange [0:750]
set xrange [0:0.5]

set output 'time-to-discover-new-periodic-speed-'.speed.'-nodes-'.nodes.'.pdf'
set xlabel "Seconds"
set title "Time To Discovery New Links Periodic Link Estimation\n".nodes." nodes, density 10, speed ".speed."mps, range 560m, 5hz period"
plot \
p1200000."/tdn" u (hist($1*20.0,20)/1000-0.005):(1.0) smooth freq w boxes t "1 Beacon",\
p2200000."/tdn" u (hist($1*20.0,20)/1000):(1.0) smooth freq w boxes t "2 Beacons",\
p3200000."/tdn" u (hist($1*20.0,20)/1000+0.005):(1.0) smooth freq w boxes t "3 Beacons"

set output 'time-to-discover-new-sa-speed-'.speed.'-nodes-'.nodes.'.pdf'
set xlabel "Seconds"
set title "Time To Discovery New Links Schedule Aware Link Estimation\n".nodes." nodes, density 10, speed ".speed."mps, range 560m, 5hz period"
plot \
s200000."/tdn" u (hist($1*20.0,20)/1000+0.005):(1.0) smooth freq w boxes notitle

set output 'time-to-discover-new-saca-speed-'.speed.'-nodes-'.nodes.'.pdf'
set xlabel "Seconds"
set title "Time To Discovery New Links Conflict Avoidance Link Estimation\n".nodes." nodes, density 10, speed ".speed."mps, range 560m, 5hz period"
plot \
c1200000."/tdn" u (hist($1*20.0,20)/1000-0.005):(1.0) smooth freq w boxes t "Global",\
c2200000."/tdn" u (hist($1*20.0,20)/1000):(1.0) smooth freq w boxes t "Oracle",\
c3200000."/tdn" u (hist($1*20.0,20)/1000+0.005):(1.0) smooth freq w boxes t "Long Link"


set boxwidth 0.005
set yrange[0:750]
set xrange[0:1.2]

set output 'time-to-discover-failed-periodic-speed-'.speed.'-nodes-'.nodes.'.pdf'
set xlabel "Seconds"
set title "Time To Discovery Failed Links Periodic Link Estimation\n".nodes." nodes, density 10, speed ".speed."mps, range 560m, 5hz period"
plot \
p1200000."/tdf" u (hist($1*20.0,20)/1000-.005):(1.0) smooth freq w boxes t "1 Beacon",\
p2200000."/tdf" u (hist($1*20.0,20)/1000):(1.0) smooth freq w boxes t "2 Beacons",\
p3200000."/tdf" u (hist($1*20.0,20)/1000+.005):(1.0) smooth freq w boxes t "3 Beacons"

set output 'time-to-discover-failed-sa-speed-'.speed.'-nodes-'.nodes.'.pdf'
set xlabel "Seconds"
set title "Time To Discovery Failed Links Schedule Aware Link Estimation\n".nodes." nodes, density 10, speed ".speed."mps, range 560m, 5hz period"
plot \
s200000."/tdf" u (hist($1*20.0,20)/1000+.005):(1.0) smooth freq w boxes notitle

set output 'time-to-discover-failed-saca-speed-'.speed.'-nodes-'.nodes.'.pdf'
set xlabel "Seconds"
set title "Time To Discovery Failed Links Conflict Avoidance Link Estimation\n".nodes." nodes, density 10, speed ".speed."mps, range 560m, 5hz period"
plot \
c1200000."/tdf" u (hist($1*20.0,20)/1000-.005):(1.0) smooth freq w boxes t "Global",\
c2200000."/tdf" u (hist($1*20.0,20)/1000):(1.0) smooth freq w boxes t "Oracle",\
c3200000."/tdf" u (hist($1*20.0,20)/1000+.005):(1.0) smooth freq w boxes t "Long Link"
