set title sprintf("Time to discover missing links, 100 nodes, %s", type)
set xtics rotate by -45
set terminal pdf;
set output 'link-subtractions.pdf'
set ylabel "Link Subtractions"
set xlabel "Milliseconds"

binwidth=1
bin(x,width)=width*floor(x/width)

plot 'linksRemoved' using (bin($1,binwidth)):(1.0):xtic(sprintf("%d", floor($1 * beaconrate / 10000))) smooth freq with boxes

set title sprintf("Time to discover new links - 100 nodes, %s", type)
set terminal pdf;
set xtics rotate by -45
set output 'link-additions.pdf'
set ylabel "Link Additions"
set xlabel "Milliseconds"

binwidth=1
bin(x,width)=width*floor(x/width)

plot 'linksAdded' using (bin($1,binwidth)):(1.0):xtic(sprintf("%d", floor($1 * beaconrate / 10000))) smooth freq with boxes
