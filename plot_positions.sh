#!/bin/bash
gnuplot <<\__EOF
set xrange [-10:10]
set yrange [-10:10]
set term png
set output "/tmp/positions0.png"
set ticslevel 0
splot "/tmp/positions.txt" u 1:2:3 with lines
pause -1
__EOF