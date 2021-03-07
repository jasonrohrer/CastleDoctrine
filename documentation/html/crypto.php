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
            }
        
        $html = "<a href=$link><img border=0 src=$image_url></a>".
            "<br>$name<br>$priceLine";

        $assetHTML[] = $html;
        }
    
    $numFetched += $numAssets;
    $numFetchedLastBatch = $numAssets;
    }

$assetHTML = array_reverse( $assetHTML )


?>

<center>

<font size=6>THE CRYPTO DOCTRINE</font><br>
    The Castle Doctrine paintings as one-of-a-kind, Non-fungible crypto tokens<br>

</center>
<br>
<br>

    View collection <a href="https://opensea.io/collection/thecastledoctrine">on OpenSea</a>.  Collect all <?php echo $numFetched;?>!

<br>
<br>


<center>
<table border=0 cellspacing=30 cellpadding=10>
<tr>
<?php

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
