<?php

// no caching
header('Cache-Control: no-store, no-cache, must-revalidate');
header('Cache-Control: post-check=0, pre-check=0', FALSE);
header('Pragma: no-cache');
header('Location: http://thecastledoctrine.net/index.php');

error_reporting( E_ALL );


$pathToRoot = "";

include( "header.php" );

?>

<center>

<font size=6>The Castle Doctrine</font><br>
a massively-multiplayer game of burglary and home defense by
<a href="http://hcsoftware.sf.net/jason-rohrer">Jason Rohrer</a>

<br>
<br>

Coming soon for PC/Mac/Linux.

<br>
<br>

Sign up for a release announcement.<br>
<form action="http://northcountrynotes.org/releaseList/server.php" 
      method="post">
<input type="hidden" name="action" value="subscribe">
Email Address: <input type="text" name="email" value="">
<input type="submit" value="Sign up">
</form>

<br>
<br>

<image border=0 width=364 height=364 src="teaserImages/screen1.png">

<br>
<br>
<br>
<br>
<br>
<br>

<image border=0 width=364 height=364 src="teaserImages/screen2.png">

<br>
<br>
<br>
<br>
<br>
<br>


<image border=0 width=364 height=364 src="teaserImages/screen3.png">


</center>
<?php include( "footer.php" ); ?>