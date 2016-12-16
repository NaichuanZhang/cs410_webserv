#!/usr/bin/perl

system('gcc -o public/file_counts public/file_counts.c');
system('public/file_counts');

open(gplot,"| gnuplot -persistent") or die "Error while piping: $! \n";
print gplot << "GNU_EOF";

set bar 1.000000 front
set boxwidth 0.9 absolute
set style fill   solid 1.00 border lt -1
set style circle radius graph 0.02, first 0.00000, 0.00000 
set style ellipse size graph 0.05, 0.03, first 0.00000 angle 0 units xy
set key inside right top vertical Right noreverse noenhanced autotitle nobox
set style histogram clustered gap 1 title textcolor lt -1
set style textbox transparent margins  1.0,  1.0 border
unset logscale
set datafile missing '-'
set style data histograms
set xtics border in scale 0,0 nomirror rotate by -45  autojustify
set xtics  norangelimit 
set xtics   ()
unset paxis 1 tics
unset paxis 2 tics
unset paxis 3 tics
unset paxis 4 tics
unset paxis 5 tics
unset paxis 6 tics
unset paxis 7 tics
set title "US immigration from Northern Europe\nPlot selected data columns as histogram of clustered boxes" 
set yrange [ 0.00000 : 300000. ] noreverse nowriteback
set paxis 1 range [ * : * ] noreverse nowriteback
set paxis 2 range [ * : * ] noreverse nowriteback
set paxis 3 range [ * : * ] noreverse nowriteback
set paxis 4 range [ * : * ] noreverse nowriteback
set paxis 5 range [ * : * ] noreverse nowriteback
set paxis 6 range [ * : * ] noreverse nowriteback
set paxis 7 range [ * : * ] noreverse nowriteback
set colorbox vertical origin screen 0.9, 0.2, 0 size screen 0.05, 0.6, 0 front  noinvert bdefault
x = 0.0
i = 23
plot 'public/filecount.dat' 

#using 6:xtic(1) ti col, '' u 12 ti col, '' u 13 ti col, '' u 14 ti col

GNU_EOF

close(gplot);
exit(1);
