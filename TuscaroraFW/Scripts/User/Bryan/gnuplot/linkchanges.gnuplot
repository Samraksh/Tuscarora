set title "Link Changes per Node per Second\n100 Nodes, 100m Range, 10hz Period"
set terminal pdf;
set output '../charts/link-changes.pdf'
set ylabel "Link Subtractions  |  Link Additions    "
set yrange [-0.7:0.7]
set style data histogram
set style histogram cluster gap 1
set style fill solid border -1
set boxwidth 0.9
set multiplot
plot 'data.add' using ($2/30000):xtic(1) ti col
set grid ytics
show grid
plot 'data.sub' using ($2/30000*-1):xtic("") ti col