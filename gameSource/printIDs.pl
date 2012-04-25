#!/usr/bin/perl -w 


$result = `find . -name "info.txt"`;

@lines = split( /^/, $result );

@idLines = ();

foreach $line (@lines) {
	chomp $line;
	
	$fileContents = readFileValue( $line );

	@fileParts = split( /\s+/, $fileContents );
	
	$fileOneLine = join( " ", @fileParts );
	
	# print "ID line:  $fileOneLine \n";

	push( @idLines, $fileOneLine );
}



@sortedIDLines = 
	sort idComp @idLines;


foreach $line (@sortedIDLines) {
	$line =~ /(\d+)\s+(.+)/;
	
	printf( "%4d   %s\n", $1, $2 );
}



sub idComp {
	my ($id_a, $name_a) = split( /\s+/, $a );
    my ($id_b, $nam_b) = split( /\s+/, $b );
	return ( $id_a <=> $id_b );
}



##
# Reads file as a string.
#
# @param0 the name of the file.
#
# @return the file contents as a string.
#
# Example:
# my $value = readFileValue( "myFile.txt" );
##
sub readFileValue {
    my $fileName = $_[0];
    open( FILE, "$fileName" ) or die;
    flock( FILE, 1 ) or die;

    # read the entire file, set the <> separator to nothing
    local $/;

    my $value = <FILE>;
    close FILE;

    return $value;
}
