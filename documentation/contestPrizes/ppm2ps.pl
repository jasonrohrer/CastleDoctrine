#!/usr/bin/perl



my $fileName = $ARGV[0];

open( FILE, $fileName );

$line = <FILE>;

chomp $line;

if( $line ne "P3" ) {
	print "Only plain ppm's supported\n";
	exit;
    }

$line = <FILE>;
chomp $line;

$width = -1;
$height = -1;

( $width, $height ) = split( / /, $line );

$pageWidth = 20 * 72;
$pageHeight = 20 * 72;

$squareWidth = $pageWidth / $width;
$squareHeight = $pageWidth / $height;


#print "image is $width x $height\n";

$maxColorVal = <FILE>;
chomp $maxColorVal;


#print square-drawing function
print 
"%!PS-Adobe-3.0 EPSF-3.0
%%Creator: ppm2ps.pl (by Jason Rohrer)
%%Title: PS version of $fileName
%%CreationDate: June 22, 2008
%%DocumentData: Clean7Bit
%%Origin: 0 0
%%BoundingBox: 0 0 $pageWidth $pageHeight
%%LanguageLevel: 2
%%Pages: 1
%%Page: 1 1
%%EOF

% draws a colored square using r g b from the stack
/csquare {
        newpath
        0    0   moveto
        0    $squareHeight.1 lineto
        $squareWidth.1  $squareHeight.1 lineto
        $squareWidth.1  0   lineto
        closepath
        setrgbcolor
        fill
} def


% draws a colored square using r g b x y from the stack
/dp {
        gsave
        translate
        csquare
        grestore
} def


%2 2 scale

";


# switch to reading in space-delim mode
$/ = " ";




for( $y=0; $y<$height; $y++ ) {
	for( $x=0; $x<$width; $x++ ) {
		
		my $r = <FILE>;
		$r = trim( $r );
		my $g = <FILE>;
		$g = trim( $g );
		my $b = <FILE>;
		$b = trim( $b );
		
		my $pageY = $height - 1 - $y;

		my $squareX = $x * $squareWidth;
		my $squareY = $pageY * $squareHeight;

		$r = $r / $maxColorVal;
		$g = $g / $maxColorVal;
		$b = $b / $maxColorVal;
		
		printf( "%.3f %.3f %.3f %d %d dp\n", 
				$r, $g, $b, $squareX, $squareY );
	}
}

print "\nshowpage\n";


# Perl trim function to remove whitespace from the start and end of the string
sub trim($)
{
	my $string = shift;
	$string =~ s/^\s+//;
	$string =~ s/\s+$//;
	return $string;
}




