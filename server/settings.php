<?php

// Basic settings
// You must set these for the server to work
// AND these must be shared by the ticketServer operating in the same
// database (because tickets are used to uniquely identify users).

$databaseServer = "localhost";
$databaseUsername = "testUser";
$databasePassword = "testPassword";
$databaseName = "test";

// The URL of to the server.php script.
$fullServerURL = "http://localhost/jcr13/castleServer/server.php";


// The URL of the main, public-face website
$mainSiteURL = "http://FIXME";


// used by server for signing price lists to verify
// that prices are valid later
$serverSecretKey = "mySecret";



// End Basic settings



// Customization settings

// Adjust these to change the way the server  works.


// Prefix to use in table names (in case more than one application is using
// the same database).
$tableNamePrefix = "castleServer_";

// this must be same as what is set for the ticket server
$ticketServerNamePrefix = "ticketServer_";




$enableLog = 1;

// for web-based admin access
$accessPasswords = array( "secret", "secret2" );




// prices that are loaded when database is first setup
// they can be changed manually later from the admin page
$defaultPrices = array(
    1 => 10,
    2 => 20 );





// header and footers for various pages
$header = "include( \"header.php\" );";
$footer = "include( \"footer.php\" );";


// for admin view
$housesPerPage = 50;




?>