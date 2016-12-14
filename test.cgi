#!/usr/bin/perl

system('public/file_counts');

open(gplot,"| gnuplot -persistent") or die "Error while piping: $! \n";
print gplot << "GNU_EOF";


# set output
set bar 1.0 front
set boxwidth 2.0
set style fill solid 0.8 border -1
set key right top vertical Right
set style histogram clustered gap 2 
set datafile missing '-'
set style data histograms
#set xtics
set title "File Types in Server File System"
set yrange [0 : 205]
x = 0.0
i = 23

plot 'public/filecount.dat' using 6:xtic(1) ti col, '' u 12 ti col, '' u 13 ti col, '' u 14 ti col

GNU_EOF
close(gplot);

#print "Content-type: text/html\n\n";
#print <<EndOfHTML;
#<html><head><title>Test Page</title></head>
#<body>
#<h2>Hello, world!</h2>
#</body></html>
#EndOfHTML
exit 1;
