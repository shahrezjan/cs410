#!/usr/bin/perl

# ARGV[0] = filename
# ARGV[1] = pattern1
# ARGV[2] = pattern2
# 	.			.
# 	.			.
# 	.			.

# Run gnuplot and read input from filehandler
open my $PROGRAM, '|-', 'gnuplot'
    or die "Error opening gnuplot: $!";

# Print commands as gnuplot input
say {$PROGRAM} "set terminal jpeg";
say {$PROGRAM} "set output 'plotresult.jpeg'";
say {$PROGRAM} "set title \"Pattern occurences in $ARGV[0]\"";
say {$PROGRAM} "set boxwidth 0.5";
say {$PROGRAM} "set style fill solid";
say {$PROGRAM} "plot '-' using 2:xtic(1) with boxes";

# Run my-histogram and store its output locally
open my $read_from_process, "-|", "./my-histogram", @ARGV;
while (my $line = <$read_from_process>) {
	say {$PROGRAM} $line;
}
say ${PROGRAM} "e\n";
close $PROGRAM;

# PRETTY PRINTED OUTPUT
print "Content-type: text/html\n\n";
print "<!DOCTYPE html>";
print "<html><body>";
print "<h1 style=\"color:red;\" align=\"center\"> CS410 Webserver </h1>";
print "<br>";
print "<center><img src=\"./plotresult.jpeg\" align=\"middle\"></center>";
print "</body></html>";
