set terminal pdf
set style data candlesticks
set boxwidth 0.5
set offsets 0.5, 0.5, 0, 0
set yrange [0:*]

set xlabel " "

set xtics ("Ground" 0, "SU\n1HB" 1, "SU\n2HB" 2, "SU\n3HB" 3, "SACA\nLL" 4, "SACA\nND" 5, "SACA\nDC LL" 6, "SA\nLL" 7, "SA\nDC LL" 8)

set output 'link-durration-speed-'.speed.'-nodes-'.nodes.'-hz-'.hz.'.pdf'
set title "Link Durrations\n".nodes." nodes, density 10, speed ".speed."mps, range 560m, ".hz."hz period"
set ylabel "Link Durration (seconds)"
plot \
   su1."/ld-q" u (1.0):($25/10/hz):($5/10/hz):($95/10/hz):($75/10/hz) notitle whiskerbars,\
   su2."/ld-q" u (2.0):($25/10/hz):($5/10/hz):($95/10/hz):($75/10/hz) notitle whiskerbars,\
   su3."/ld-q" u (3.0):($25/10/hz):($5/10/hz):($95/10/hz):($75/10/hz) notitle whiskerbars,\
  saca."/ld-q" u (4.0):($25/10/hz):($5/10/hz):($95/10/hz):($75/10/hz) notitle whiskerbars,\
sacano."/ld-q" u (5.0):($25/10/hz):($5/10/hz):($95/10/hz):($75/10/hz) notitle whiskerbars,\
sacadc."/ld-q" u (6.0):($25/10/hz):($5/10/hz):($95/10/hz):($75/10/hz) notitle whiskerbars,\
    sa."/ld-q" u (7.0):($25/10/hz):($5/10/hz):($95/10/hz):($75/10/hz) notitle whiskerbars,\
  sadc."/ld-q" u (8.0):($25/10/hz):($5/10/hz):($95/10/hz):($75/10/hz) notitle whiskerbars,\
   su1."/ld-q" u (1.0):($50/10/hz):($50/10/hz):($50/10/hz):($50/10/hz) notitle lt -1,\
   su2."/ld-q" u (2.0):($50/10/hz):($50/10/hz):($50/10/hz):($50/10/hz) notitle lt -1,\
   su3."/ld-q" u (3.0):($50/10/hz):($50/10/hz):($50/10/hz):($50/10/hz) notitle lt -1,\
  saca."/ld-q" u (4.0):($50/10/hz):($50/10/hz):($50/10/hz):($50/10/hz) notitle lt -1,\
sacano."/ld-q" u (5.0):($50/10/hz):($50/10/hz):($50/10/hz):($50/10/hz) notitle lt -1,\
sacadc."/ld-q" u (6.0):($50/10/hz):($50/10/hz):($50/10/hz):($50/10/hz) notitle lt -1,\
    sa."/ld-q" u (7.0):($50/10/hz):($50/10/hz):($50/10/hz):($50/10/hz) notitle lt -1,\
  sadc."/ld-q" u (8.0):($50/10/hz):($50/10/hz):($50/10/hz):($50/10/hz) notitle lt -1

set output 'time-to-discover-new-speed-'.speed.'-nodes-'.nodes.'-hz-'.hz.'.pdf'
set title "Time To Discover New Links\n".nodes." nodes, density 10, speed ".speed."mps, range 560m, ".hz."hz period"
set ylabel "Time To Discover (seconds)"
plot \
   su1."/tdn-q" u (1.0):($25/10/hz):($5/10/hz):($95/10/hz):($75/10/hz) notitle whiskerbars,\
   su2."/tdn-q" u (2.0):($25/10/hz):($5/10/hz):($95/10/hz):($75/10/hz) notitle whiskerbars,\
   su3."/tdn-q" u (3.0):($25/10/hz):($5/10/hz):($95/10/hz):($75/10/hz) notitle whiskerbars,\
  saca."/tdn-q" u (4.0):($25/10/hz):($5/10/hz):($95/10/hz):($75/10/hz) notitle whiskerbars,\
sacano."/tdn-q" u (5.0):($25/10/hz):($5/10/hz):($95/10/hz):($75/10/hz) notitle whiskerbars,\
sacadc."/tdn-q" u (6.0):($25/10/hz):($5/10/hz):($95/10/hz):($75/10/hz) notitle whiskerbars,\
    sa."/tdn-q" u (7.0):($25/10/hz):($5/10/hz):($95/10/hz):($75/10/hz) notitle whiskerbars,\
  sadc."/tdn-q" u (8.0):($25/10/hz):($5/10/hz):($95/10/hz):($75/10/hz) notitle whiskerbars,\
   su1."/tdn-q" u (1.0):($50/10/hz):($50/10/hz):($50/10/hz):($50/10/hz) notitle lt -1,\
   su2."/tdn-q" u (2.0):($50/10/hz):($50/10/hz):($50/10/hz):($50/10/hz) notitle lt -1,\
   su3."/tdn-q" u (3.0):($50/10/hz):($50/10/hz):($50/10/hz):($50/10/hz) notitle lt -1,\
  saca."/tdn-q" u (4.0):($50/10/hz):($50/10/hz):($50/10/hz):($50/10/hz) notitle lt -1,\
sacano."/tdn-q" u (5.0):($50/10/hz):($50/10/hz):($50/10/hz):($50/10/hz) notitle lt -1,\
sacadc."/tdn-q" u (6.0):($50/10/hz):($50/10/hz):($50/10/hz):($50/10/hz) notitle lt -1,\
    sa."/tdn-q" u (7.0):($50/10/hz):($50/10/hz):($50/10/hz):($50/10/hz) notitle lt -1,\
  sadc."/tdn-q" u (8.0):($50/10/hz):($50/10/hz):($50/10/hz):($50/10/hz) notitle lt -1

set output 'time-to-discover-failed-speed-'.speed.'-nodes-'.nodes.'-hz-'.hz.'.pdf'
set title "Time To Discover Failed Links\n".nodes." nodes, density 10, speed ".speed."mps, range 560m, ".hz."hz period"
plot \
   su1."/tdf-q" u (1.0):($25/10/hz):($5/10/hz):($95/10/hz):($75/10/hz) notitle whiskerbars,\
   su2."/tdf-q" u (2.0):($25/10/hz):($5/10/hz):($95/10/hz):($75/10/hz) notitle whiskerbars,\
   su3."/tdf-q" u (3.0):($25/10/hz):($5/10/hz):($95/10/hz):($75/10/hz) notitle whiskerbars,\
  saca."/tdf-q" u (4.0):($25/10/hz):($5/10/hz):($95/10/hz):($75/10/hz) notitle whiskerbars,\
sacano."/tdf-q" u (5.0):($25/10/hz):($5/10/hz):($95/10/hz):($75/10/hz) notitle whiskerbars,\
sacadc."/tdf-q" u (6.0):($25/10/hz):($5/10/hz):($95/10/hz):($75/10/hz) notitle whiskerbars,\
    sa."/tdf-q" u (7.0):($25/10/hz):($5/10/hz):($95/10/hz):($75/10/hz) notitle whiskerbars,\
  sadc."/tdf-q" u (8.0):($25/10/hz):($5/10/hz):($95/10/hz):($75/10/hz) notitle whiskerbars,\
   su1."/tdf-q" u (1.0):($50/10/hz):($50/10/hz):($50/10/hz):($50/10/hz) notitle lt -1,\
   su2."/tdf-q" u (2.0):($50/10/hz):($50/10/hz):($50/10/hz):($50/10/hz) notitle lt -1,\
   su3."/tdf-q" u (3.0):($50/10/hz):($50/10/hz):($50/10/hz):($50/10/hz) notitle lt -1,\
  saca."/tdf-q" u (4.0):($50/10/hz):($50/10/hz):($50/10/hz):($50/10/hz) notitle lt -1,\
sacano."/tdf-q" u (5.0):($50/10/hz):($50/10/hz):($50/10/hz):($50/10/hz) notitle lt -1,\
sacadc."/tdf-q" u (6.0):($50/10/hz):($50/10/hz):($50/10/hz):($50/10/hz) notitle lt -1,\
    sa."/tdf-q" u (7.0):($50/10/hz):($50/10/hz):($50/10/hz):($50/10/hz) notitle lt -1,\
  sadc."/tdf-q" u (8.0):($50/10/hz):($50/10/hz):($50/10/hz):($50/10/hz) notitle lt -1

set style data boxes
set boxwidth 0.85
set style fill solid

set output 'false-positive-speed-'.speed.'-nodes-'.nodes.'-hz-'.hz.'.pdf'
set title "Percentage of False Postive Links\n".nodes." nodes, density 10, speed ".speed."mps, range 560m, ".hz."hz period"
set ylabel "Percentage of Links"
plot \
   su1."/abcd" u (1.0):(100*$3/($2+$5)) notitle,\
   su2."/abcd" u (2.0):(100*$3/($2+$5)) notitle,\
   su3."/abcd" u (3.0):(100*$3/($2+$5)) notitle,\
  saca."/abcd" u (4.0):(100*$3/($2+$5)) notitle,\
sacano."/abcd" u (5.0):(100*$3/($2+$5)) notitle,\
sacadc."/abcd" u (6.0):(100*$3/($2+$5)) notitle,\
    sa."/abcd" u (7.0):(100*$3/($2+$5)) notitle,\
  sadc."/abcd" u (8.0):(100*$3/($2+$5)) notitle

set output 'false-negative-speed-'.speed.'-nodes-'.nodes.'-hz-'.hz.'.pdf'
set title "Percentage of False Negative Links\n".nodes." nodes, density 10, speed ".speed."mps, range 560m, ".hz."hz period"
set ylabel "Percentage of Links"
plot \
   su1."/abcd" u (1.0):(100*$5/($2+$5)) notitle,\
   su2."/abcd" u (2.0):(100*$5/($2+$5)) notitle,\
   su3."/abcd" u (3.0):(100*$5/($2+$5)) notitle,\
  saca."/abcd" u (4.0):(100*$5/($2+$5)) notitle,\
sacano."/abcd" u (5.0):(100*$5/($2+$5)) notitle,\
sacadc."/abcd" u (6.0):(100*$5/($2+$5)) notitle,\
    sa."/abcd" u (7.0):(100*$5/($2+$5)) notitle,\
  sadc."/abcd" u (8.0):(100*$5/($2+$5)) notitle

set output 'link-churn-speed-'.speed.'-nodes-'.nodes.'-hz-'.hz.'.pdf'
set title "Link Changes per Node per Second\n".nodes." nodes, density 10, speed ".speed."mps, range 560m, ".hz."hz period"
set ylabel "Percentage of Links"
plot \
   su1."/lc-ground" u (0.0):($1/nodes/seconds) notitle,\
   su1."/lc"       u (1.0):($1/nodes/seconds) notitle,\
   su2."/lc"       u (2.0):($1/nodes/seconds) notitle,\
   su3."/lc"       u (3.0):($1/nodes/seconds) notitle,\
  saca."/lc"       u (4.0):($1/nodes/seconds) notitle,\
sacano."/lc"       u (5.0):($1/nodes/seconds) notitle,\
sacadc."/lc"       u (6.0):($1/nodes/seconds) notitle,\
    sa."/lc"       u (7.0):($1/nodes/seconds) notitle,\
  sadc."/lc"       u (8.0):($1/nodes/seconds) notitle
