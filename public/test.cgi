#!/usr/bin/perl

system("gcc", "-o", "file_counts", "file_counts.c");
system("./file_counts", "/");

open(gplot,"| gnuplot -persistent") or die "Error while piping: $! \n";
print gplot << "GNU_EOF";

set terminal jpeg font "Helvetica" 11
set output 'file_count.jpg'
set autoscale y
set bar 1.0 front
set boxwidth 1.8
set style fill solid 0.8 border -1
set key right top vertical Right
set style histogram gap 1.5 
set datafile missing '-'
set style data histograms
set tics scale 0
set title "File Types in Server File System"
x = 0.0
i = 23

plot 'file_count.dat' using 2:xticlabels(1)

GNU_EOF
close(gplot);

print "Content-type: text/html\n\n";
print <<EndOfHTML;
<html><head><title>File Count Test</title></head>
<body>

<h2 align="middle">Results</h2>
<p>
<img src="file_count.jpg" align="middle">
</p>

</body>
</html>
EndOfHTML

exit 1;
