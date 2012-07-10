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
    array( 1, 10, 0, "Wooden Wall" ),
    array( 2, 20, 0, "Metal Wall" ),
    array( 20, 15, 0, "Window" ),
    array( 30, 50, 0, "Automatic Door" ),
    array( 51, 20, 0, "Wired Wooden Wall" ),
    array( 70, 200, 0, "Pit Bull" ),
    array( 100, 100, 0, "Invisible Floor Trap" ),
    array( 101, 50, 0, "Pressure Toggle Switch" ),
    array( 102, 5, 0, "Wiring" ),
    array( 103, 100, 0, "Power Source" ),
    array( 104, 20, 0, "Voltage-triggered Switch" ),
    array( 105, 20, 0, "Voltage-triggered Inverted Switch" ),
    array( 106, 10, 0, "Wire Bridge" ),
    array( 110, 20, 0, "Electric Floor" ),
    
    array( 500, 20, 0, "Saw" ),
    array( 501, 100, 0, "Cutting Torch" ),
    array( 502, 470, 0, "S-W Model 36" ),

    array( 2000, 10000, 1, "Owl - Rohrer" ),
    array( 2001, 10000, 1, "Untitled 2 - Rohrer" ),
    array( 2002, 10000, 1, "Untitled 3 - Rohrer" ),
    array( 2003, 10000, 1, "Untitled 4 - Rohrer" ),
    array( 2004, 10000, 1, "Untitled 5 - Rohrer" )
    );
    


// header and footers for various pages
$header = "include( \"header.php\" );";
$footer = "include( \"footer.php\" );";


// for admin view
$housesPerPage = 50;



// with these defaults, a $10000 price will drop below 1 in a bit more than 6
// hours (price clamped to $1 after that)

// price drop interval for auto-auctions in minutes
$auctionPriceDropInterval = 3;

// prices drop by half after this many minutes
$auctionPriceHalfLife = 30;




?>