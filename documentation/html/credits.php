<?php

// no caching
header('Cache-Control: no-store, no-cache, must-revalidate');
header('Cache-Control: post-check=0, pre-check=0', FALSE);
header('Pragma: no-cache');

error_reporting( E_ALL );


$pathToRoot = "";

include( "header.php" );

?>


<font size=6>Credits</font><br>

<table border=0 width="100%" cellpadding=5><tr><td bgcolor="#222222">
This game was made possible by the support of Jeff Roberts.
<br>
<br>
All design, programming, graphics, fonts, and sound by Jason Rohrer.<br>
<br>

The graphics were made with <a href="http://mtpaint.sourceforge.net/">mtPaint</a>. The <a href="http://www.libsdl.org/">SDL</a> library provides cross-platform screen, sound, and user input.  <a href="http://www.mingw.org/">MinGW</a> was used to build the game for Windows.  The server runs on <a href="http://www.apache.org">Apache</a>, <a href="http://www.mysql.com">MySQL</a>, and <a href="http://php.net">PHP</a>.<br>
<br>
The following testers played early versions of the game and took the time to send me feedback:<br>
<br>

Thibaut Girka,
Jeremiah Reid,
David Pittman,
Malcom McCrimmon,
Allen Trivette,
Eric McQuiggan,
Garrett,
Nicholas Rodine,
Manuel Magalhães,
Madeleine Burleson,
Olivier Lejade,
Rod Humble,
Chris Benson,
Will,
Ross Calderaro,
Kristian Bagger,
Luca Hibbard-Curto,
Juan Valdelomar,
    Cristian Gamboa,
	Joe Mirabello,
Ben Keenan,
		Wes Cook,
Mike Walbridge,
	David Tran Quang Ty,
Justin Leites,
	Duncan Keller,
    Austin Auclair,
Paolo Pedercini,
Daniel Benmergui

 
</td></tr></table>
<br>



<?php include( "footer.php" ); ?>