<?php

// no caching
header('Cache-Control: no-store, no-cache, must-revalidate');
header('Cache-Control: post-check=0, pre-check=0', FALSE);
header('Pragma: no-cache');

error_reporting( E_ALL );


$pathToRoot = "";

include( "header.php" );


function showPayLinks( $inSimple ) {
    $referrer = "";
    
    if( isset( $_SERVER['HTTP_REFERER'] ) ) {
        // pass it through without a regex filter
        // because we can't control its safety in the end anyway
        // (user can just edit URL sent to FastSpring).
        $referrer = urlencode( $_SERVER['HTTP_REFERER'] );
        }
    
    

    ?>
 <center>
      <center><table border=0><tr><td> 
<ul> 
      <li>Early access alpha testing
      <li>Lifetime server account
      <li>All future updates
      <li>Tech support included
      </ul>
</td></tr></table>

      <font size=5>Available now for $8</font><br>
      <font size=4 color=#ff0000>(50% off final price during alpha)</font><br><br>
      
      <a href="https://sites.fastspring.com/jasonrohrer/instant/thecastledoctrine?referrer=<?php echo $referrer;?>">
      <img src="fs_cards.png" width=280 height=45 border=0><?php
      if( !$inSimple ) {

          echo "<br>";
          
          echo "<img src=\"fs_button05.png\" width=210 height=60 border=0>";
          }
?>
      </a>
      </center>
<?php
    }

function showLogo( $inImageFile, $inText ) {

    echo "<table border=0><tr><td align=center>
          <img src=\"$inImageFile\"><br>
          <font size=1>$inText</font>
          </td></tr></table>";
    }

?>



<center>

<font size=6>The Castle Doctrine</font><br>
a massively-multiplayer game of burglary and home defense by
<a href="http://hcsoftware.sf.net/jason-rohrer">Jason Rohrer</a>
<br>
<br>
<br>

<font size=5>
"A powerful, memorable game."</font> --Alec Meer, RPS [<a href="http://www.rockpapershotgun.com/2013/03/07/castle-doctrine-preview-2/">1</a>] [<a href="http://www.rockpapershotgun.com/2013/03/08/the-castle-doctrine/">2</a>] 
<br>
<br>

<br>
<br>
<table border=0 width=60% cellpadding=5><tr><td bgcolor="#222222">

<table border=0 cellpadding=5><tr>
<td valign=top><a href="details.php"><img src="detailImages/family.png" width=168 height=154 border=0></a></td>
<td valign=top>
<font size=5>It's 1991, and things are bad.</font> You're a guy with a wife, two kids, and a house.... <br><br>
[<a href="details.php">Learn more</a>]
</td>
</tr></table>

</td></tr></table>
<br>
<br>

<center>
<table border=0><tr>
<td><?php showLogo( "noDRM.png", "No DRM" ); ?></td>
<td><?php showLogo( "noTie.png", "No middle-person" ); ?></td>
<td><?php showLogo( "crossPlatform.png", "Cross-platform" ); ?></td>
<td><?php showLogo( "openSource.png", "Open Source" ); ?></td>
</tr></table>                                  
</center>

<center>
<table border=0 cellpadding=2><tr><td bgcolor="#222222">
<table border=0 cellpadding=5><tr><td bgcolor="#000000">
<center> 

<?php
   showPayLinks( false );
?>

</td></tr></table>
</td></tr></table>
</center>
<br>
<br>
</center>

<font size=5>What you get</font><br>

<table border=0 width="100%" cellpadding=5><tr><td bgcolor="#222222">
Immediately after your payment is processed, you will receive an email with an access link.  You will then be able to download all of the following DRM-free distributions:
<center>
<table border=0><tr><td>
<ul>
<li>Windows build</li>
<li>MacOS build (10.5 and later Intel)</li>
<li>Full source code bundle (which can also be compiled on GNU/Linux)</li>
</ul>
</td></tr></table>
</center>
The price also includes downloads of all future updates and a lifetime account on the main game server that I am running.<br>
<br>
The source bundle includes the server software, allowing you to <b>set up and run your own server</b> (a web server with PHP/MySQL is required).<br>
<br>
You can take a look at the <a href="requirements.php">system requirements</a>.
</td></tr></table>
<br>
<br>

[<a href="credits.php">Credits</a>]



<?php include( "footer.php" ); ?>