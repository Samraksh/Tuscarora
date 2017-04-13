set title "False Links and Missing Links\n100 Nodes, 100m Range, 10hz Period"
set terminal pdf;
set output '../charts/abcd.pdf'
set ylabel "Avg. Number of Links at each Node"
set style data histogram
set style histogram cluster gap 1
set style fill solid border -1
set boxwidth 0.9
set grid ytics
show grid
plot 'abcd.dat' using ($2/100/300/100):xtic(1) ti col, '' u ($3/100/300/100) ti col
