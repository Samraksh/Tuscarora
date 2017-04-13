set terminal pdf
set style data l
set style fill solid
set xlabel "Distance from Sender"
set ylabel "% of nodes receiving message"

set output 'prr-density.pdf'
set title "Packet Receive Rate - 100 nodes, period=5Hz, various densities"
plot "d-4" using 1:4 t "4" with l, \
"d-5" using 1:4 t "5" with l, \
"d-6" using 1:4 t "6" with l, \
"d-8" using 1:4 t "8" with l, \
"d-10" using 1:4 t "10" with l, \
"d-20" using 1:4 t "20" with l, \
"d-40" using 1:4 t "40" with l

set output 'prr-size.pdf'
set title "Packet Receive Rate - density 8, period=5Hz, various network sizes"
plot "s-10" using 1:4 t "10" with l, \
"s-30" using 1:4 t "30" with l, \
"s-50" using 1:4 t "50" with l, \
"s-100" using 1:4 t "100" with l, \
"s-200" using 1:4 t "200" with l, \
"s-300" using 1:4 t "300" with l, \
"s-500" using 1:4 t "500" with l, \
"s-700" using 1:4 t "700" with l, \
"s-1000" using 1:4 t "1000" with l

set xrange [0:1000]

set output 'prr-density-subset.pdf'
set title "Packet Receive Rate - 100 nodes, period=5Hz, various densities"
plot "d-4" using 1:4 t "4" with l, \
"d-5" using 1:4 t "5" with l, \
"d-6" using 1:4 t "6" with l, \
"d-8" using 1:4 t "8" with l, \
"d-10" using 1:4 t "10" with l, \
"d-20" using 1:4 t "20" with l, \
"d-40" using 1:4 t "40" with l

set output 'prr-size-subset.pdf'
set title "Packet Receive Rate - density 8, period=5Hz, various network sizes"
plot "s-10" using 1:4 t "10" with l, \
"s-30" using 1:4 t "30" with l, \
"s-50" using 1:4 t "50" with l, \
"s-100" using 1:4 t "100" with l, \
"s-200" using 1:4 t "200" with l, \
"s-300" using 1:4 t "300" with l, \
"s-500" using 1:4 t "500" with l, \
"s-700" using 1:4 t "700" with l, \
"s-1000" using 1:4 t "1000" with l
