
set grid

set key off

set terminal postscript eps enhanced
set output "prizeGraph.eps"
set size .8,.8

#set logscale x
#set logscale y

set format x '$%.f'

set xtics (1,50, 100, 150, 200, 250, 300)

set xrange[-10:320]
set yrange[-2:50]

plot "prizeGraphData2.txt" using 1:2 with lines


pause -1