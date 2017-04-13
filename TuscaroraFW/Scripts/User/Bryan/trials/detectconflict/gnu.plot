set title sprintf("Histogram of Link Estimation Beacon Schedules - %s", parameter)
set terminal pdf;
set output 'schedule.pdf'
set ylabel "Frequency"
set xlabel "Beaconing Interval in ms"
set xtics rotate by -45

delta_v(x, n) = ( n != old_n ? old_v = newOld_v : old_v = old_v, old_n = n, vD = x - old_v, old_v = x, old_d = vD, vD)
old_v = 1262304000000000.0
old_d = NaN
newOld_v = 1262304000000000.0
old_n = 0

bin(x,width)=width*floor(x/width)

plot 'hbSent' using (bin(delta_v($1, $2),binwidth)):(1.0):xtic(sprintf("%d", floor(old_d / 5000) * 5)) smooth freq with boxes t "Periodic"
