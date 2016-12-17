#!/usr/bin/perl

print "Content-type: text/html\n\n";
print <<EndOfHTML;
<html><head><title>File Count Test</title></head>
<body>

<h2 align="middle">Results</h2>
<center><img src="file_count.jpg" align="middle"></center>
<a href="test.cgi?image=file_count.jpg">

</body>
</html>
EndOfHTML

exit 1;
