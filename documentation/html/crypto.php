<?php

// no caching
header('Cache-Control: no-store, no-cache, must-revalidate');
header('Cache-Control: post-check=0, pre-check=0', FALSE);
header('Pragma: no-cache');

error_reporting( E_ALL );


$pathToRoot = "";

include( "header.php" );


//$assetsURL = "https://api.opensea.io/api/v1/assets?order_direction=desc&collection=the-castle-doctrine";

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
    The Castle Doctrine paintings as one-of-a-kind, Non-fungible crypto tokens<br>

</center>
<br>
<br>

    <center>
    <table width=75% border=0 cellpadding=10>
    <tr><td bgcolor=#222222>
<?php echo $numFetched;?> unique pixel paintings from the independent video game The Castle Doctrine.  Inside the game world, only one player can own each painting, but paintings can be stolen by other players through in-game burglaries, which are completely legal.  In the real world, only one person can own each non-fungible painting token, but tokens can be stolen by other people through real-life burglaries, which are completely illegal.  Please acquire your tokens responsibly.
    </td></tr></table></center>

    <br>

    
    View collection and bid on <a href="https://opensea.io/collection/the-castle-doctrine">on OpenSea</a>.  Collect all <?php echo $numFetched;?>!

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

    
More info coming soon...

<?php include( "footer.php" ); ?>
