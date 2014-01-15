

#set datafile separator ','
set xdata time
set timefmt '%Y-%m-%d'

set xrange ['2014-01-15':'2014-02-20']
set yrange [4:20]
#set xlabel "Date"

#set ylabel "Price"

set xtics ("Mar 12, 2013" '2014-01-15', "Jan 29, 2014" '2014-01-29', "Feb 5, 2014" '2014-02-05', "{/Symbol=30 \245}" '2014-02-20' )

set ytics ("$8" 8, "$12" 12, "$16" 16 )

set grid

set key off


set terminal postscript eps enhanced
set output "pricePlan.eps"
set size .8,.8

plot "pricePlanData.txt" using 1:2 with lines

pause -1