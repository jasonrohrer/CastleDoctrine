<?php

// no caching
header('Cache-Control: no-store, no-cache, must-revalidate');
header('Cache-Control: post-check=0, pre-check=0', FALSE);
header('Pragma: no-cache');

error_reporting( E_ALL );


$pathToRoot = "";

include( "header.php" );

?>

<center>

<font size=6>STEAL REAL MONEY</font><br>
Contest Results<br>

</center>
<br>
<br>

This page contains the official results of The Castle Doctrine's <a href="stealRealMoneyContest.php">STEAL REAL MONEY Contest</a>.
<br>
<br>
First of all, over 1000 people participated during peak of contest on Sunday, with 859 people playing during last day on Monday.  At the end, 297 of those people still had a house in the game.<br>
<br>
<b>$3,163,271</b> of in-game dollar value had accumulated in these 297 vaults.  The resulting exchange rate was $1 USD for every $1054 in-game dollars.  121 lucky people ended the game with at least $1054, which translates into at least a $1 USD prize for each of them.  55 people won $10 USD or more.  6 people won $100 USD or more.  As a result, I will pay out $2902 in total prize money.<br>
<br>
Here's the prize distribution:
<center>
<img src="contestImages/results/prizeGraph.png" width=540 height=377>
</center>
With those stats out of the way, on to...
<br>
<br>
<center><font size=6>The Top 9 Houses</font></center>

<br>
It turns out that house #7 on the list <i>had no painting</i> at the end of the contest.  Neither did #9.  Thus, player #10 receives a painting instead.
<br>
<br>
Now that a week has passed since the end of the contest, all top players have either died or been robbed down to $0.  Thus, I can share their full house maps without giving away any crucial secrets.  Yes, insane genius was on full display.
<br>
<br>
<br>

<?php

function winnerRow( $inNumber, $inNumberDisplay, $inGameAmount,
                    $inCharacterName, $inPlayerName, $inPrize,
                    $inPaintingName ) {
                        
    echo "<tr><td><table border=0>";

    $inGameAmountString = number_format( $inGameAmount );
    
    echo "<tr><td valign=top>#$inNumberDisplay: <b>$inPlayerName</b><br>".
    "\$$inGameAmountString<br><br></td>";
    echo "<td valign=top>Also known as<br><b>$inCharacterName</b></td></tr>";

    $prizeMoney = floor( $inGameAmount / 1054 );

    $extraPrize = "";
    if( $inPrize != "" ) {
        $extraPrize = "<br>AND <i>$inPrize</i>";
        }

    $paintingLine = "";

    if( $inPaintingName != "" ) {
        $paintingLine = "<br>AND A giclee print of <i>$inPaintingName</i>";
        }
    
    
    echo "<tr><td colspan=2>Prize: <b>\$$prizeMoney USD</b>".
    "$extraPrize".
    "$paintingLine</td></tr>";

    $paintingNumber = $inNumber + 10 - 1;
    
    
    echo "<tr><td align=center><img src='contestImages/results/entryways/screen0000$inNumber.png' ".
    "width=365 height=200><br>".
    "<img src='contestImages/results/paintings/screen000$paintingNumber.png' ".
    "width=152 height=152 alt='$inPaintingName'></td>";
    echo "<td align=center><a href='contestImages/results/maps/map0000$inNumber.png'>".
    "<img border=0 src='contestImages/results/maps/thumbs/map0000$inNumber.png' ".
    "width=224 height=224><br>Click for full map</a></td>";

    
    echo "</tr>";

    echo "</table border=0></td></tr>";
    }


?>


<center>
<table border=1 cellspacing=10>

<?php winnerRow( 1, 1, 333952, "Jeffrey William McCoy",
                 "Aaron Bleackley (ZenRose)", "Jason Rohrer's Dog Club",
                 "Surge After Hokusai --- by Steven Diamond" ); ?>

<?php winnerRow( 2, 2, 254150, "Andrew Curtis Thomas",
                 "Tom Loughlin (ukuko)", "A Door Devil",
                 "Blythe --- by Andy Lin" ); ?>

<?php winnerRow( 3, 3, 200813, "David Travis Lopez",
                 "Iceman", "A Door Devil",
                 "Savanna --- by Adam Saltsman" ); ?>

<?php winnerRow( 4, 4, 135160, "Hector Vance Bradley",
                 "CryptoMogul", "A Door Devil",
                 "Down Goes Frazier --- by Jason Stevenson" ); ?>

<?php winnerRow( 5, 5, 119720, "Charles Carl Perez",
                 "SecretBeachIsStillAlive (stars)", "A Door Devil",
                 "Speech Balloon --- by Frank Lantz" ); ?>

<?php winnerRow( 6, 6, 119698, "John Robert Carpenter",
                 "Jeremiah Reid (jere)",
                 "A Custom Cartridge Shopping Spree",
                 "Redface --- by Phil Fish" ); ?>

<?php winnerRow( 7, 7, 87853, "James Mark Worley",
                 "KimJaeSung",
                 "A Custom Cartridge Shopping Spree",
                 "" ); ?>

<?php winnerRow( 8, 8, 87225, "Steven Shaun Black",
                 "Meier Ernst",
                 "A Custom Cartridge Shopping Spree",
                 "Warhol --- by Victor Stone" ); ?>

<?php winnerRow( 9, 10, 82269, "Andrew Richard Whitfield",
                 "Christopher Hart (Synthesis)",
                 "",
                 "One of Two --- by Chris Bell" ); ?>

</table>
</center>


<br>
<br>
Thank you, everyone, for participating.<br>
<br>
So... did this contest work to promote the game before launch?  Was it worth running?  Absolutely.  The buzz around the contest created the second biggest pre-Steam sales day in Castle Doctrine history:  795 people bought the game in one day.  This was topped only by the alpha launch day back in March 2013, when 826 people bought the game.<br>
<br>
All told the contest cost me around $4500 to run, which is cheaper than advertising for a week on any noteworthy website.  The sales generated from the coverage on a single day more than covered this cost.  It didn't annoy innocent bystanders (like advertising does), and it added to the world and community of the game in an interesting and meaningful way.<br>
<br>
And, perhaps most importantly, it created an incredibly tense moment in history that I, and many members of the game's community, will never forget.<br>
<br>
4:59pm PST on Monday, January 27, 2014.<br>
<br>

<?php include( "footer.php" ); ?>
