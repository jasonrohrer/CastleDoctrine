<?php

$offsetString = $_SERVER['QUERY_STRING'];

$assetsURL = "https://api.opensea.io/api/v1/assets?order_direction=desc&collection=the-castle-doctrine";

$fetchURL = $assetsURL . $offsetString;

$result = file_get_contents( $fetchURL );

echo $result;
?>