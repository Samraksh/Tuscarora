set title "True, False and Missing Links\n100 nodes, Periodic, Inactivity=1"
set terminal pdf;
set output 'abcd-periodic.pdf'
set ylabel "Percentage of links"
set xlabel "Mean Speed - Speed=+/-0.5 Mean Speed"
set style data linespoints
plot 'output' i 0 u 1:($3/($2+$5)*100) w points title "False Links", \
     'output' i 0 u 1:($5/($2+$5)*100) w points title "Missing Links"

set title "True, False and Missing Links\n100 nodes, Scheduled"
set terminal pdf;
set output 'abcd-scheduled.pdf'
set ylabel "Percentage of links"
set xlabel "Mean Speed - Speed=+/-0.5 Mean Speed"
set style data linespoints
plot 'output' i 1 u 1:($3/($2+$5)*100) w points title "False Links", \
     'output' i 1 u 1:($5/($2+$5)*100) w points title "Missing Links"

set title "True, False and Missing Links\n100 nodes, Avoidance"
set terminal pdf;
set output 'abcd-avoidance.pdf'
set ylabel "Percentage of links"
set xlabel "Mean Speed - Speed=+/-0.5 Mean Speed"
set style data linespoints
plot 'output' i 2 u 1:($3/($2+$5)*100) w points title "False Links", \
     'output' i 2 u 1:($5/($2+$5)*100) w points title "Missing Links"

set title "Time to Discover New Links\n100 nodes"
set terminal pdf;
set output 'discover-new.pdf'
set ylabel "Milliseconds to Discover"
set xlabel "Mean Speed - Speed=+/-0.5 Mean Speed"
set style data linespoints
plot 'output' i 0 u 1:6 w points title "Periodic - Inactivity=1", \
     'output' i 1 u 1:6 w points title "Scheduled", \
     'output' i 1 u 1:6 w points title "Avoidance"

set title "Time to Discover Missing Links\n100 nodes"
set terminal pdf;
set output 'discover-missing.pdf'
set ylabel "Milliseconds to Discover"
set xlabel "Mean Speed - Speed=+/-0.5 Mean Speed"
set style data linespoints
plot 'output' i 0 u 1:8 w points title "Periodic - Inactivity=1", \
     'output' i 1 u 1:8 w points title "Scheduled", \
     'output' i 2 u 1:8 w points title "Avoidance"
