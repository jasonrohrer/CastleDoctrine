#!/usr/bin/perl -w 


$result = `ls . -1`;

@lines = split( /^/, $result );

foreach $line (@lines) {
	chomp $line;

	print "Object: '$line'\n";

	`cp -r $line/0 $line/100`;
	`rm -f $line/100/properties*`;
	`rm -f $line/100/*behind*`;
	`rm -f $line/100/*under*`;
	`rm -f $line/100/*shadeMap*`;
}
