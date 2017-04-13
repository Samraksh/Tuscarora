set terminal pdf
set yrange [0:*]
set offsets 0.5, 0.5, 0, 0
set boxwidth 0.5

hz(h) = sprintf("%.1f", 1000000.0/h)
#candlesticks x, 25, 5, 95, 75

#beacon charts
set style data candlesticks
set xtic ("" 1)
set style fill empty

n_f = words(beacons)
set for [i=1:n_f] xtics add (hz(word(beacons,i)) i)
set ylabel "Time (seconds)"
set xlabel "Beacon hz"

set output 'time-to-discover-new-'.type.'-beacon.pdf'
set title "Time To Discover New Links - ".name."\n100 nodes, density 10, speed 50mps, range 560m"
plot for [i=1:n_f] word(beaconfiles, i).'/tdn-q' using (i):($25/10/hz(word(beacons,i))):($5/10/hz(word(beacons,i))):($95/10/hz(word(beacons,i))):($75/10/hz(word(beacons,i))) notitle whiskerbars,\
     for [i=1:n_f] word(beaconfiles, i).'/tdn-q' using (i):($50/10/hz(word(beacons,i))):($50/10/hz(word(beacons,i))):($50/10/hz(word(beacons,i))):($50/10/hz(word(beacons,i))) notitle lt -1

set output 'time-to-discover-failed-'.type.'-beacon.pdf'
set title "Time To Discover Failed Links - ".name."\n100 nodes, density 10, speed 50mps, range 560m"
plot for [i=1:n_f] word(beaconfiles, i).'/tdf-q' using (i):($25/10/hz(word(beacons,i))):($5/10/hz(word(beacons,i))):($95/10/hz(word(beacons,i))):($75/10/hz(word(beacons,i))) notitle whiskerbars,\
     for [i=1:n_f] word(beaconfiles, i).'/tdf-q' using (i):($50/10/hz(word(beacons,i))):($50/10/hz(word(beacons,i))):($50/10/hz(word(beacons,i))):($50/10/hz(word(beacons,i))) notitle lt -1

set output 'link-durration-'.type.'-beacon.pdf'
set title "Link Durration - ".name."\n100 nodes, density 10, speed 50mps, range 560m"
plot for [i=1:n_f] word(beaconfiles, i).'/ld-q' using (i):($25/10/hz(word(beacons,i))):($5/10/hz(word(beacons,i))):($95/10/hz(word(beacons,i))):($75/10/hz(word(beacons,i))) notitle whiskerbars,\
     for [i=1:n_f] word(beaconfiles, i).'/ld-q' using (i):($50/10/hz(word(beacons,i))):($50/10/hz(word(beacons,i))):($50/10/hz(word(beacons,i))):($50/10/hz(word(beacons,i))) notitle lt -1

set style data boxes
set ylabel "Percentage of Links"
set style fill solid

set output 'false-positive-'.type.'-beacon.pdf'
set title "Percentage of False Positive Links - ".name."\n100 nodes, density 10, speed 50mps, range 560m"
plot for [i=1:n_f] word(beaconfiles, i).'/abcd' using (i):(100*$3/($2+$5)) notitle

set output 'false-negative-'.type.'-beacon.pdf'
set title "Percentage of False Negative Links - ".name."\n100 nodes, density 10, speed 50mps, range 560m"
plot for [i=1:n_f] word(beaconfiles, i).'/abcd' using (i):(100*$5/($2+$5)) notitle


#density charts
set style data candlesticks
set xtic ("" 1)
set style fill empty

n_f = words(densities)
set for [i=1:n_f] xtics add (word(densities,i) i)
set ylabel "Time (seconds)"
set xlabel "Density"

set output 'time-to-discover-new-'.type.'-density.pdf'
set title "Time To Discover New Links - ".name."\n100 nodes, beacon 5hz, speed 50mps, range 560m"
plot for [i=1:n_f] word(densityfiles, i).'/tdn-q' using (i):($25/10/5):($5/10/5):($95/10/5):($75/10/5) notitle whiskerbars,\
     for [i=1:n_f] word(densityfiles, i).'/tdn-q' using (i):($50/10/5):($50/10/5):($50/10/5):($50/10/5) lt -1 notitle

set output 'time-to-discover-failed-'.type.'-density.pdf'
set title "Time To Discover Failed Links - ".name."\n100 nodes, beacon 5hz, speed 50mps, range 560m"
plot for [i=1:n_f] word(densityfiles, i).'/tdf-q' using (i):($25/10/5):($5/10/5):($95/10/5):($75/10/5) notitle whiskerbars,\
     for [i=1:n_f] word(densityfiles, i).'/tdf-q' using (i):($50/10/5):($50/10/5):($50/10/5):($50/10/5) notitle lt -1 

set output 'link-durration-'.type.'-density.pdf'
set title "Link Durration - ".name."\n100 nodes, beacon 5hz, speed 50mps, range 560m"
plot for [i=1:n_f] word(densityfiles, i).'/ld-q' using (i):($25/10/5):($5/10/5):($95/10/5):($75/10/5) notitle whiskerbars,\
     for [i=1:n_f] word(densityfiles, i).'/ld-q' using (i):($50/10/5):($50/10/5):($50/10/5):($50/10/5) notitle lt -1 

set style data boxes
set ylabel "Percentage of Links"
set style fill solid

set output 'false-positive-'.type.'-density.pdf'
set title "Percentage of False Positive Links - ".name."\n100 nodes, beacon 5hz, speed 50mps, range 560m"
plot for [i=1:n_f] word(densityfiles, i).'/abcd' using (i):(100*$3/($2+$5)) notitle

set output 'false-negative-'.type.'-density.pdf'
set title "Percentage of False Negative Links - ".name."\n100 nodes, beacon 5hz, speed 50mps, range 560m"
plot for [i=1:n_f] word(densityfiles, i).'/abcd' using (i):(100*$5/($2+$5)) notitle


#size charts
set style data candlesticks
set xtic ("" 1)
set style fill empty

n_f = words(sizes)
set for [i=1:n_f] xtics add (word(sizes,i) i)
set ylabel "Time (seconds)"
set xlabel "Network Size"

set output 'time-to-discover-new-'.type.'-sizes.pdf'
set title "Time To Discover New Links - ".name."\ndensity 10, beacon 5hz, speed 50mps, range 560m"
plot for [i=1:n_f] word(sizefiles, i).'/tdn-q' using (i):($25/10/5):($5/10/5):($95/10/5):($75/10/5) notitle whiskerbars,\
     for [i=1:n_f] word(sizefiles, i).'/tdn-q' using (i):($50/10/5):($50/10/5):($50/10/5):($50/10/5) notitle lt -1 

set output 'time-to-discover-failed-'.type.'-sizes.pdf'
set title "Time To Discover Failed Links - ".name."\ndensity 10, beacon 5hz, speed 50mps, range 560m"
plot for [i=1:n_f] word(sizefiles, i).'/tdf-q' using (i):($25/10/5):($5/10/5):($95/10/5):($75/10/5) notitle whiskerbars,\
     for [i=1:n_f] word(sizefiles, i).'/tdf-q' using (i):($50/10/5):($50/10/5):($50/10/5):($50/10/5) notitle lt -1 

set output 'link-durration-'.type.'-sizes.pdf'
set title "Link Durration - ".name."\ndensity 10, beacon 5hz, speed 50mps, range 560m"
plot for [i=1:n_f] word(sizefiles, i).'/ld-q' using (i):($25/10/5):($5/10/5):($95/10/5):($75/10/5) notitle whiskerbars,\
     for [i=1:n_f] word(sizefiles, i).'/ld-q' using (i):($50/10/5):($50/10/5):($50/10/5):($50/10/5) notitle lt -1 

set style data boxes
set ylabel "Percentage of Links"
set style fill solid

set output 'false-positive-'.type.'-sizes.pdf'
set title "Percentage of False Positive Links - ".name."\ndensity 10, beacon 5hz, speed 50mps, range 560m"
plot for [i=1:n_f] word(sizefiles, i).'/abcd' using (i):(100*$3/($2+$5)) notitle

set output 'false-negative-'.type.'-sizes.pdf'
set title "Percentage of False Negative Links - ".name."\ndensity 10, beacon 5hz, speed 50mps, range 560m"
plot for [i=1:n_f] word(sizefiles, i).'/abcd' using (i):(100*$5/($2+$5)) notitle


#speed charts
set style data candlesticks
set xtic ("" 1)
set style fill empty

n_f = words(speeds)
set for [i=1:n_f] xtics add (word(speeds,i) i)
set ylabel "Time (seconds)"
set xlabel "Avg Node Speed"

set output 'time-to-discover-new-'.type.'-speeds.pdf'
set title "Time To Discover New Links - ".name."\ndensity 10, beacon 5hz, 100 nodes, range 560m"
plot for [i=1:n_f] word(speedfiles, i).'/tdn-q' using (i):($25/10/5):($5/10/5):($95/10/5):($75/10/5) notitle whiskerbars,\
     for [i=1:n_f] word(speedfiles, i).'/tdn-q' using (i):($50/10/5):($50/10/5):($50/10/5):($50/10/5) notitle lt -1 
 
set output 'time-to-discover-failed-'.type.'-speeds.pdf'
set title "Time To Discover Faliled Links - ".name."\ndensity 10, beacon 5hz, 100 nodes, range 560m"
plot for [i=1:n_f] word(speedfiles, i).'/tdf-q' using (i):($25/10/5):($5/10/5):($95/10/5):($75/10/5) notitle whiskerbars,\
     for [i=1:n_f] word(speedfiles, i).'/tdf-q' using (i):($50/10/5):($50/10/5):($50/10/5):($50/10/5) notitle lt -1 
 
set output 'link-durration-'.type.'-speeds.pdf'
set title "Link Durration - ".name."\ndensity 10, beacon 5hz, 100 nodes, range 560m"
plot for [i=1:n_f] word(speedfiles, i).'/ld-q' using (i):($25/10/5):($5/10/5):($95/10/5):($75/10/5) notitle whiskerbars,\
     for [i=1:n_f] word(speedfiles, i).'/ld-q' using (i):($50/10/5):($50/10/5):($50/10/5):($50/10/5) notitle lt -1 

set style data boxes
set ylabel "Percentage of Links"
set style fill solid

set output 'false-positive-'.type.'-speeds.pdf'
set title "Percentage of False Positive Links - ".name."\ndensity 10, beacon 5hz, 100 nodes, range 560m"
plot for [i=1:n_f] word(speedfiles, i).'/abcd' using (i):(100*$3/($2+$5)) notitle

set output 'false-negative-'.type.'-speeds.pdf'
set title "Percentage of False Negative Links - ".name."\ndensity 10, beacon 5hz, 100 nodes, range 560m"
plot for [i=1:n_f] word(speedfiles, i).'/abcd' using (i):(100*$5/($2+$5)) notitle
