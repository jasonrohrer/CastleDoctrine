<?php

// no caching
header('Cache-Control: no-store, no-cache, must-revalidate');
header('Cache-Control: post-check=0, pre-check=0', FALSE);
header('Pragma: no-cache');

error_reporting( E_ALL );


$pathToRoot = "";

include( "header.php" );


//$assetsURL = "https://api.opensea.io/api/v1/assets?order_direction=desc&collection=thecastledoctrine";

//$assetsURL = "https://api.opensea.io/api/v1/assets?order_direction=desc&collection=jason-rohrer";

// use relay, if local server can't support latest ssl
// note ? at end
$assetsURL = "http://onehouronelife.com/openseaRelay.php?";


$numFetchedLastBatch = 1;

$numFetched = 0;

$assetHTML = array();

$itemURLs = array();

$forSaleURLs = array();

$notForSaleURLs = array();

$showURLs = 0;



while( $numFetchedLastBatch > 0 ) {

    // try 50 each time, and last batch will be shorter
    $limit = 50;

    
    $offsetString = "&offset=$numFetched&limit=$limit";

    $fetchURL = $assetsURL . $offsetString;

    $result = file_get_contents( $fetchURL );

    $a = json_decode( $result, true );
    $numAssets = count( $a['assets'] );


    for( $i=0; $i<$numAssets; $i++ ) {
        $asset = $a['assets'][$i];

        $image_url = $asset['image_thumbnail_url'];
        $name = $asset['name'];

        $link = $asset['permalink'];

        $itemURLs[] = $link;
        
        // split name into lines
        $nameParts = preg_split( "/ --- /", $name );

        $nameParts[0] = "<a href=$link>" . $nameParts[0] . "</a>";

        $name = join( "<br>", $nameParts );
        
        
        $sell_orders = $asset['sell_orders'];

        $numOrders = count( $sell_orders );

        $priceEth = 0;

        if( $numOrders > 0 ) {
            
            // last one?
            $orderToCheck = $numOrders - 1;
            
            $price = $sell_orders[ $orderToCheck ][ 'current_price' ];
            
            $priceEth = $price / pow( 10.0, 18.0 );
            }
        
        $priceLine = "$priceEth ETH";
            
        if( $priceEth == 0 ) {
            $priceLine = "[not for sale]";
            $notForSaleURLs[] = $link;
            }
        else {
            $forSaleURLs[] = $link;
            }
        
        $html = "<a href=$link><img border=0 src=$image_url></a>".
            "<br>$name<br>$priceLine";

        $assetHTML[] = $html;
        }
    
    $numFetched += $numAssets;
    $numFetchedLastBatch = $numAssets;
    }

$assetHTML = array_reverse( $assetHTML );


$itemURLs = array_reverse( $itemURLs );
$forSaleURLs = array_reverse( $forSaleURLs );
$notForSaleURLs = array_reverse( $notForSaleURLs );


?>

<center>

<font size=6>THE CRYPTO DOCTRINE</font><br>
    The Castle Doctrine paintings as one-of-a-kind, non-fungible crypto tokens<br>

</center>
<br>
<br>

    <center>
    <table width=75% border=0 cellpadding=10>
    <tr><td bgcolor=#222222>
<?php echo $numFetched;?> unique pixel paintings from the independent video game The Castle Doctrine.  Inside the game world, only one player can own each painting, but paintings can be stolen by other players through in-game burglaries, which are completely legal.  In the real world, only one person can own each non-fungible painting token, but tokens can be stolen by other people through real-life burglaries, which are completely illegal.  Please acquire your tokens responsibly.
    </td></tr></table></center>

    <br>

    <br>
    <center>
    <table width=75% border=0 cellpadding=10>
    <tr><td bgcolor=#222222>
    This is a Dutch auction, where the price starts high and falls over time.  The prices will decline in a linear fashion, all the way down to 0 ETH, by Sunday, March 14, 2021 at Noon EST.<br>
    <br>
    More information can be <a href="#faq">found here</a>.
    
    </td></tr></table></center>
    <br>
    <br>
    
    View the collection and bid on <a href="https://opensea.io/collection/thecastledoctrine">on OpenSea</a>.  Collect all <?php echo $numFetched;?>!

<br>
<br>


<center>
<table border=0 cellspacing=30 cellpadding=10>
<tr>
<?php

if( $showURLs ) {
    $c = count( $itemURLs );
    echo "<br><br>All URLS ($c):<br>";
    foreach( $itemURLs as $url ) {
        echo "$url<br>";
        }

    $c = count( $forSaleURLs );
    echo "<br><br>For SaleURLS ($c):<br>";
    foreach( $forSaleURLs as $url ) {
        echo "$url<br>";
        }

    $c = count( $notForSaleURLs );
    echo "<br><br>NOT for sale URLS ($c):<br>";
    foreach( $notForSaleURLs as $url ) {
        echo "$url<br>";
        }
    }


    
$perRow = 3;

$numPrinted = 0;

foreach( $assetHTML as $html ) {

    
    echo "<td align=center bgcolor=#222222>";
    echo "<div style='width: 170px; word-wrap: break-word'>";
    echo "$html</dev></td>";
    $numPrinted++;
    
    if( $numPrinted % $perRow == 0 ) {
        // start new row
        echo "</tr><tr>";
        }
    }
?>
</tr>
</table>
</center>



<br>
<br>
<br>

<a name="faq"></a>
<font size=6>Questions and Answers</font>
<br>
<br>
<br>

<b>What am I buying?</b>
<br>
<br>
A non-fungible token is like a title deed on the Ethereum blockchain that declares you as the one true owner of the painting token in question.  Because I effectively sign the deed over to you, using the secret key in my wallet, the deed cannot be forged by someone else.  And because the blockchain is tracking the deed as it is transferred from person to person, there's a visible provenance that can be traced all the way back to me, the creator of the token.  The provenance can be viewed by anyone by looking the token up in the blockchain, using a tool like OpenSea.  You can see an example token with a storied provenance <a href=https://opensea.io/assets/0xbb5ed1edeb5149af3ab43ea9c7a6963b3c1374f7/242>here</a>.
<br>
<br>
<br>
<br>

<b>Can't you just make more copies of these tokens later?</b>
<br>
<br>
Yes, I can, but they won't be the real, original tokens sold in this auction.  In fact, someone else can make "fake" painting tokens too and put them out there.  However, anyone can look up a token's history on the blockchain, using a tool like OpenSea, and check the provenance.  The real painting tokens were created by me this week.  Any fake tokens, created by me or anyone else, will have later birth dates, making them easily recognizable as fakes.
<br>
<br>
<br>
<br>

<b>Why would I want one of these?  What can I do with it?</b>
<br>
<br>
Once you get one of these tokens into your Ethereum wallet, you become the one true owner of the token, and you can do whatever you want with it.  You can hold it, transfer it, gift it, wait for offers on it, or even auction it yourself.  This web page, and the <a href="https://opensea.io/collection/thecastledoctrine">OpenSea collection page</a>, track the current status of each painting.  If someone offers up a painting for sale in the future, their asking price will show up here.  Even though the painting token will be out of my hands, it will still be in the public blockchain, where everyone can check its status.
<br>
<br>
<br>
<br>

<b>So I can potentially resell a painting for a profit in the future?</b>
<br>
<br>
Yes, that's possible.  It's also possible for it to drop in value in the future, of course.
<br>
<br>
<br>
<br>


<b>What happens if someone snags an auction painting for cheap, and sells it later for millions?</b>
<br>
<br>
The blockchain token contract specifies a 10% royalty fee, back to me, on all future sales, so while you enjoy your millions, I'll be taking a small cut of that windfall.  Thus, I'm not scared that these paintings will sell for cheap now and gain popularity later.
<br>
<br>
<br>
<br>



<b>What about the original artists?</b>
<br>
<br>
If, in a miraculous turn of events, these paintings become insanely popular and bring in loads of auction money and back-end royalties, I will share the money with any of the original artists who want some of it.  These paintings were all made for me at my specific request, back in 2013, but these people are all my personal friends, and I'm not looking to screw anyone over.
<br>
<br>
<br>
<br>




<b>How does a Dutch auction work?</b>
<br>
<br>
An English auction is more familiar, where the price starts with a low bid, people outbid each other, and the auction continues as long as the bidding, with the highest bidder winning.  A Dutch auction works in the opposite way:  the price starts very high, beyond what anyone would be willing to pay, and falls gradually over time.  As the price falls, anyone can buy the item at its current price, and the auction ends immediately at that point, with no further bidding.  People still compete with each other, but the psychology is different, since they're trying to wait as long as possible to get the best price, but they don't want to wait too long, because there's a risk of someone else snagging it.    
<br>
<br>
<br>
<br>

<b>Why not an English auction?</b>
<br>
<br>
Bidding in an English auction requires an irrevocable promise to pay, so that the highest bidder can receive the item in an atomic swap when the auction ends.  That's a more complicated (and thus expensive) operation on the blockchain, because the bid funds need to be locked in a contract.  Thus, in an English auction, potential buyers need to essentially pay gas fees even to bid, and they don't get those fees back if someone else wins the item.  A Dutch auction doesn't have this problem, because it's essential a falling fixed price, and a buyer can buy the item instantly at any time, with no promise to pay later.  Also, the original game used a Dutch auction for the paintings, with very high starting prices, so this setup in real life makes a nice parallel.
<br>
<br>
<br>
<br>


<b>The starting price is 6 ETH per painting?  That's insanely high!</b>
<br>
<br>
A Dutch auction works best with an insanely high starting price, to put everyone on equal footing in the waiting game.  If the starting price was reasonable, and several people were willing to pay that reasonable price right at the start, the quickest person would win the auction.  But the price will fall to 3 ETH in 48 hours, and 1 ETH by Sunday.  There will be several hours of much more reasonable prices on Sunday.  A Dutch auction kinda comes to a head right at the end.
<br>
<br>
<br>
<br>

<b>How do I actually bid?</b>
<br>
<br>
Bidding takes place with Ethereum cryptocurrency, so you're going to need to get your hands on some of that, and put it into an Ethereum wallet.  You bid through the OpenSea system, so you have to use wallet software that is compatible with their website, as a browser extension.  That's how they authorize your purchase, by asking your browser extension wallet for permission.  MetaMask is the most widely used browser wallet, with support for Firefox, Chrome, Brave, and Edge.  You can download it for your browser here:  <a href="https://metamask.io/download.html">https://metamask.io/download.html</a>.  Once you have it installed, you can either create a new Ethereum wallet in MetaMask, and send some Ethereum to it, or sign in to an existing Ethereum wallet inside MetaMask.  When you go to buy an item on OpenSea, MetaMask will pop up and ask you to approve the transaction.
<br>
<br>
<br>
<br>


<b>What's going on with the gas fees?</b>
<br>
<br>
Doing anything on the Ethereum blockchain requires a gas fee, payed to the nodes that are operating the network.  The size of the fee corresponds to the complexity of the operation and the current congestion in the network.  Something simple, like sending someone Ethereum currency, might incur a fee of something like $4 to $10 worth of Ethereum, depending on the time of day and network congestion.  Transferring non-fungible tokens require a more complex contract operation on the blockchain, so the gas fees are quite a bit higher, with $60 fees or more being common at peak times.  You will pay a substantial gas fee when buying a painting token, but you'll be able to see a preview of that fee before you approve the transaction.  Keep gas fees in mind, however, if you're thinking about snagging a painting on the cheap.<br>
<br>
<br>
<br>

<b>NFTs are a scam.  And they're destroying the planet.  They're a planet-destroying scam!  How dare you?</b>
<br>
<br>
I'm definitely taking a walk on the moral wild side with this one.  Follow the saccharine sound of my silver pipe, as I lead the way to ruin.
<br>
<br>
<br>
<br>
    
<b>You're trying to pass this off as some kind of edgy artistic statement?  You're really just trying to cash in on the hype!</b>

<br>
<br>
Oh, I'm definitely cashing in on the hype.
<br>
<br>
<br>
<br>


<?php include( "footer.php" ); ?>
