set title sprintf("Percentage of colisions for link estimation")
set terminal pdf;
set output 'colisions.pdf'
set ylabel "Percentage"
set xlabel "Beaconing Period - Hz"

plot 'total_colisions' using 0:((1-($2/$3))*100):xtic(sprintf("%d", 1000000/$1)) with boxes t "Periodic"
