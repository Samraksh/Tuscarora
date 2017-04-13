set terminal pdf
set style data boxes
set style fill solid
set boxwidth 0.4
set xtics ("1" 1, "5" 5, "10" 10)

set yrange [0:10]

set xlabel "Beacon Period - Hz"
set ylabel "Percentage of Links"

set output 'links-discovery-periodic-speed-'.speed.'-nodes-'.nodes.'.pdf'
set title "False and Missing Links\n".nodes." nodes, density 10, speed ".speed."mps, range 560m, Periodic Link Estimtion"
plot \
"d1-l1" u ($1-1.2):(100*$4/($3+$6)) lt 1 t "1 missed beacon False Link" fillstyle pattern 3,\
"d1-l1" u ($1-0.8):(100*$6/($3+$6)) lt 2 t "1 missed beacon Missing Link" fillstyle pattern 3,\
"d2-l1" u ($1-0.2):(100*$4/($3+$6)) lt 1 t "2 False" fillstyle pattern 2,\
"d2-l1" u ($1+0.2):(100*$6/($3+$6)) lt 2 t "2 Missing" fillstyle pattern 2,\
"d3-l1" u ($1+0.8):(100*$4/($3+$6)) lt 1 t "3 False" fillstyle pattern 8,\
"d3-l1" u ($1+1.2):(100*$6/($3+$6)) lt 2 t "3 Missing" fillstyle pattern 8

set output 'links-discovery-sa-speed-'.speed.'-nodes-'.nodes.'.pdf'
set key left
set title "False and Missing Links\n".nodes." nodes, density 10, speed ".speed."mps, range 560m, Schedule Aware Link Estimtion"
plot \
"d2-l2" u ($1-0.2):(100*$4/($3+$6)) lt 1 t "1 missed beacon False Link",\
"d2-l2" u ($1+0.2):(100*$6/($3+$6)) lt 2 t "1 missed beacon Missing Link"

set output 'links-discovery-saca-speed-'.speed.'-nodes-'.nodes.'.pdf'
set title "False and Missing Links\n".nodes." nodes, density 10, speed ".speed."mps, range 560m, Conflict Avoidance Link Estimtion"
plot \
"d1-l3" u ($1-1.2):(100*$4/($3+$6)) lt 1 t "Global Discovery False Link" fillstyle pattern 3,\
"d1-l3" u ($1-0.8):(100*$6/($3+$6)) lt 2 t "Global Missing Link" fillstyle pattern 3,\
"d2-l3" u ($1-0.2):(100*$4/($3+$6)) lt 1 t "Oracle False" fillstyle pattern 2,\
"d2-l3" u ($1+0.2):(100*$6/($3+$6)) lt 2 t "Oracle Missing" fillstyle pattern 2,\
"d3-l3" u ($1+0.8):(100*$4/($3+$6)) lt 1 t "Long Link False" fillstyle pattern 8,\
"d3-l3" u ($1+1.2):(100*$6/($3+$6)) lt 2 t "Long Link Missing" fillstyle pattern 8
