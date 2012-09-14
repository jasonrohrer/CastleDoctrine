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
//
// array( ID, Price, InGallery, Note )
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
    array( 2004, 10000, 1, "Untitled 5 - Rohrer" ),

    array( 3000, 500, 1, "Dummy 0 --- by No One" ),
    array( 3001, 500, 1, "Dummy 1 --- by No One" ),
    array( 3002, 500, 1, "Dummy 2 --- by No One" ),
    array( 3003, 500, 1, "Dummy 3 --- by No One" ),
    array( 3004, 500, 1, "Dummy 4 --- by No One" ),
    array( 3005, 500, 1, "Dummy 5 --- by No One" ),
    array( 3006, 500, 1, "Dummy 6 --- by No One" ),
    array( 3007, 500, 1, "Dummy 7 --- by No One" ),
    array( 3008, 500, 1, "Dummy 8 --- by No One" ),
    array( 3009, 500, 1, "Dummy 9 --- by No One" ),
    array( 3010, 500, 1, "Dummy 10 --- by No One" ),
    array( 3011, 500, 1, "Dummy 11 --- by No One" ),
    array( 3012, 500, 1, "Dummy 12 --- by No One" ),
    array( 3013, 500, 1, "Dummy 13 --- by No One" ),
    array( 3014, 500, 1, "Dummy 14 --- by No One" ),
    array( 3015, 500, 1, "Dummy 15 --- by No One" ),
    array( 3016, 500, 1, "Dummy 16 --- by No One" ),
    array( 3017, 500, 1, "Dummy 17 --- by No One" ),
    array( 3018, 500, 1, "Dummy 18 --- by No One" ),
    array( 3019, 500, 1, "Dummy 19 --- by No One" ),
    array( 3020, 500, 1, "Dummy 20 --- by No One" ),
    array( 3021, 500, 1, "Dummy 21 --- by No One" ),
    array( 3022, 500, 1, "Dummy 22 --- by No One" ),
    array( 3023, 500, 1, "Dummy 23 --- by No One" ),
    array( 3024, 500, 1, "Dummy 24 --- by No One" ),
    array( 3025, 500, 1, "Dummy 25 --- by No One" ),
    array( 3026, 500, 1, "Dummy 26 --- by No One" )
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