#!/usr/bin/perl

# Test script for CS410-Assignment 3
# Demonstrates processing of raw data (input) into output formatted as HTML

# Script: Counts 10 numbers from a specified starting numbers and displays the
# 		  the values in a specified font-size

# ARGV[0] = html text size (1-7)
# ARGV[1] = starting number
# ARGV[2] = count this many values

print "Content-type: text/html\n\n";
print "<!DOCTYPE html>";
print "<html><body>";
print "<h1> Count $ARGV[2] digits from input value $ARGV[1]: </h1>";
print "<h2> (Using font size $ARGV[0]) </h2>";
print "<font size=\"$ARGV[0]\">";
for (my $i = $ARGV[1]; $i < $ARGV[1]+$ARGV[2]; $i++) {
	print "$i<br>";
}
print "</font>";
print "</body></html>";