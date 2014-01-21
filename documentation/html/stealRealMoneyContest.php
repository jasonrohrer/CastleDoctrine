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
Pre-Launch Contest<br>

</center>
<br>
<br>

To celebrate The Castle Doctrine's forthcoming launch on January 29, and to mark the last week of a very successful alpha test, I figured it was <b>time to give back</b> to the game's community.  Or rather, <b><i>have it taken back.</i></b>
<br>
<br>
Here's how it works:
<br>
<br>
The contest will run until 5pm US PST on Monday, January 27.  At that moment in time, whatever money you have in the game will be converted into real US Dollars and paid to you.  Steal more, win more.  Have your in-game money stolen from you, and the robber will win more.<br>
<br>
I'm taking a fraction of The Castle Doctrine's alpha-period earnings and putting up a game-wide bounty of $3000 USD.  This amount will be divided amoung all living players at the 5pm deadline based on the fraction of total game money that each player posses.  The exchange rate will fluctuate throughoutt the contest period, according to how much total money acccumulates in the game.  The current exchange rate is:
<br>
<br>
<center>
<table border=0 cellpadding=2><tr><td bgcolor="#222222">
<table border=0 cellpadding=5><tr><td bgcolor="#000000">
<center> 

<?php
   include( "../../server/exchangeRate.php" );
?>

</td></tr></table>
</td></tr></table>
</center>
<br>

But wait!  There's more!  The very best players can also...

<br>
<br>

<font size=6>STEAL REAL STUFF</font>
<br>
<br>
In addition to the cash prize divided amoung all players, the <b>top eight</b> houses in the game at the contest deadline will also win <b>two</b> fabulous prizes each.
<br>
<br>

<br>
<br>

<font size=5>Real Paintings</font>
<br>
<br>

Each of the top eight players will have their favorite, in-game painting transformed into a gorgeous 20x20 inch giclee-printed canvas.  Vibrant colors, archival pigments, the whole nine yards.  For these tiny 32x32 pixel paintings, each pixel will be an enormous 5/8 inch square on the canvas.<br>
<br>

<center>
<img src="contestImages/prizes/paintingSamples.jpg" width=480 height=417 border=0><br>
<font size=3>(A sample of giclee pixel art on canvas.)</font>
</center>
<br>

<b>But there's a catch:</b>  to win a given painting, that player must have that painting in their possession at the contest deadline.  If you have your favorite painting stolen, you're out of luck.

<br>
<br>

<br>
<br>

<font size=5>Real Securty Stuff</font>

<br>
<br>
In addition to a painting, the top eight players will also receive these security prizes.
<br>

<br>
<br>
<br>
<u><font size=5>First Prize:  <i>Jason Rohrer's Dog Club</i></font></u>
<br>
Own the club that started it all.  A heartwarming souvenier from Las Cruces, New Mexico.  (Note:  was never actually used to club a dog.)  Way safer than a gun!  Much harder to commit suicide with.  More compact than a golf club, which was actually the anti-dog tool of choice in Cruces.
<br><br>
<center><img src="contestImages/prizes/clubFolded.jpg" width=329 height=300 border=0> <font color =black>.</font> <img src="contestImages/prizes/clubExtended.jpg" width=225 height=300 border=0></center>
<br>
A little rusty at the tip from being carried around on a bike.  Think of it as patina, not rust. 26 inches extended.  10 inches folded up.  Weighs over one pound.  Cannot be shipped to CA, NY, DC, MA, or out of the US.  Cannot be shipped to anyone under 18 years of age.  Since it's not legal in CA (goodbye NM!), it is being held for us outside CA for shipping.


<br>
<br>
<br>
<br>
<u><font size=5>Second - Fifth Prizes:  <i>A Door Devil</i></font></u>
<br>
Sick of having your front door kicked in at three in the afternoon?  Our 85-year-old neighbor in Las Cruces was.  Maybe she should have installed one of these.  Worried that the feds aren't going to give you enough warning when they barge in?  If you're actually worried about that, you're probably the kind of person the feds are actually watching!  In any case, it's amazing what a few strip of metal and some long screws can do:
<center>
<iframe title="YouTube video player" width="640" height="390" src="http://www.youtube.com/embed/2UxEX2HbhO8?rel=0" frameborder="0" allowfullscreen></iframe>
<br>
<img src="contestImages/prizes/doorDevil.png" width=540 height=132 border=0>
</center><br>
You can also watch <a href="http://www.youtube.com/watch?v=vVw-5gHIJZ8">this video</a> to see some big guys really hammer on one of these.  The folks at Door Devil were nice enough to give us <b>four</b> kits to hand out as prizes for the second-, third-, fourth-, and fifth-place players.  Did I mention that it comes with a free yard sign?


<br>
<br>
<br>
<br>
<u><font size=5>Sixth - Eigth Prizes:  <i>Custom Cartridge Shopping Spree</i></font></u><br>
Ready to take life into your own hands?  Why not do it in style, the New Mexico way, at Las Cruces's very own Custom Cartridge Company?<br>
<br>
<center><img src="newsImages/selfDefense/gunShop.jpg" width=395 height=348 border=0></center>
<br>
Go on a shopping spree at the only place where I ever touched a handgun.  Don't try anything funny, though, because all the guys who work at the store are packing.  Finish the contest in sixth, seventh, or eighth place to get a $50 gift certificate.  (Note:  $50 is not enough to actually buy a gun.)
<br>

<br>
<br>
<br>

<font size=6>Questions and Answers</font>
<br>
<br>
<br>

<b>How will you fairly determine who won at 5pm?  Won't there be chaos?</b>
<br>
<br>
At 5pm, I will take a snapshot of the server database to "freeze" the state of the game world safely for study offline.  I will then study that game world to determine who is living, and how much money they have, and also who the top 8 players are.  Note that only players who have edited their current house at least once, and put their house on the public list, will be considered for prize money.  Your private starter home, with its untouched $2000, won't get you a prize.
<br>
<br>
<br>

<b>How will you determine the top eight players?</b>
<br>
<br>
The top eight players will be determined by estimated house value as it appears on the public house list.  This is determined both by cash held and the resale value of all tools held.  Essentially, if your house is on the first page of the house list, you are in the top eight.
<br>
<br>
<br>

<b>How will you pay us?</b>
<br>
<br>
All payments will be made using PayPal.  Their MassPay feature will let me export the payment list and automatically pay everyone with one click.  I'm assuming that each person registered for the game with an email address that can receive PayPal payments.  If this is not the case for you, you may want to add your game email to your PayPal account (one PayPal account can have multiple email addresses associated with it).  If this is impossible for some reason, please get in touch with me by email to have your game email address changed on my server.
<br>
<br>
<br>

<b>When will you pay us?</b>
<br>
<br>
I plan to have all payments sent out within a week after the contest ends.  Remember, this is my launch week, and I have a ton of other stuff to do, but I will do my best.
<br>
<br>
<br>


    

<center>

<br><br><br>
<img src="contestImages/approachCropped.png" width=256 height=256>
<br><br><br><br>
<br><br><br><br>
<img src="contestImages/attackCropped.png" width=256 height=256>
<br><br><br><br>
<br><br><br><br>
<img src="contestImages/zoom1.png" width=256 height=256>
<br><br><br><br>
<br><br><br><br>
<img src="contestImages/zoom2.png" width=256 height=256>
<br><br><br><br>
<br><br><br><br>
<img src="contestImages/zoom3.png" width=256 height=256>
<br><br><br><br>
<br><br><br><br>
<img src="contestImages/zoom4.png" width=256 height=256>
<br><br><br><br>
<br><br><br><br>
<img src="contestImages/zoom5.png" width=256 height=256>
<br><br><br>

<br><br><br>
</center>
<?php include( "footer.php" ); ?>