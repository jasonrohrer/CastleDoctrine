<?php

// no caching
header('Cache-Control: no-store, no-cache, must-revalidate');
header('Cache-Control: post-check=0, pre-check=0', FALSE);
header('Pragma: no-cache');
header('Location: http://thecastledoctrine.net/index.php');

error_reporting( E_ALL );


global $pathToRoot;

$pathToRoot = "";

include( "header.php" );

include( "preAlphaList/settings.php" );

global $databaseServer, $databaseUsername, $databasePassword, $databaseName;


$ml_mysqlLink =
    mysql_connect( $databaseServer, $databaseUsername, $databasePassword )
    or ml_operationError( "Could not connect to database server: " .
                          mysql_error() );


mysql_select_db( $databaseName )
or ml_operationError( "Could not select $databaseName database: " .
                      mysql_error() );


global $tableNamePrefix;


    $query = "SELECT * FROM $tableNamePrefix"."log ".
        "ORDER BY entry_time DESC;";


$result = mysql_query( "SELECT COUNT(*) ".
                       "FROM $tableNamePrefix"."recipients ".
                       "WHERE confirmed=1;" );

$hitCount = mysql_result( $result, 0, 0 );


mysql_close( $ml_mysqlLink );


?>
<center>
<font size=6>The Castle Doctrine</font><br>
Call for Pre-alpha testers.
<br><br>
</center>
<?php



if( $hitCount < 125 ) {
    
?>
The Castle Doctrine is nearing release, but it's not quite ready yet.<br><br>

I need a small group of people to help me test it and iron out any last issues.  <b>One hundred</b> people, to be exact.<br><br>

The first 100 people to sign up and confirm their valid email addresses will receive:<br><br>
<center>
<table border=0>
<tr><td>
<ul>
<li>A chance to play The Castle Doctrine before anyone else.
<li>Free download access forever, even after release.
<li>A lifetime membership on the central server.
<li>Potential exposure to bugs, and a chance to help me fix them.
</ul>
</td></tr></table>
    
    <form action="http://thecastledoctrine.net/preAlphaList/server.php"
              method="post">
        <input type="hidden" name="action" value="subscribe">
        Email Address: <input type="text" name="email" value="" size=30>
        <input type="submit" value="Sign up">
        </form>
<?php
        }
else {
?>
<center>
    Sorry, but more than 100 people have already signed up for the free pre-alpha test.<br>
<br>
<br>
<br>

But you can still sign up for a release announcement.<br>
<form action="http://northcountrynotes.org/releaseList/server.php"
      method="post">
<input type="hidden" name="action" value="subscribe">
Email Address: <input type="text" name="email" value="" size=30>
<input type="submit" value="Sign up">
</form>

</center>
<?php
        }
?>



</center>
<?php include( "footer.php" );
 ?>
