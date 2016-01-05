#!/usr/bin/perl

# A cgi script to get current temperature
# to be called from my webserver (Assignment a3).
# Ex: localhost:8080/arduinotemp.cgi

$output = `./arduinoGetTemp`;

print "Content-type: text/plain\n\n";
print "$output";
