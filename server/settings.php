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

// The URL of the ticket server's server.php script.
// This is used to fetch assigned ticket IDs for users.
$ticketServerURL = "http://localhost/jcr13/ticketServer/server.php";


// The ticket server encrypts the ticket IDs that it returns to us
// using this shared secret
$sharedEncryptionSecret = "secret2";



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





$enableLog = 1;

// for web-based admin access
$accessPasswords = array( "secret", "secret2" );




// prices that are loaded when database is first setup
// they can be changed manually later from the admin page
//
// array( ID, Price, InGallery, Note )
$defaultPrices = array(
    array( 1, 10, 0, "Wooden Wall" ),
    array( 2, 20, 0, "Steel Wall" ),
    array( 3, 50, 0, "Concrete Wall" ),
    array( 0, 0, 0, "Empty Floor" ),
    array( 20, 15, 0, "Window" ),
    array( 111, 100, 0, "Pit" ),
    array( 103, 100, 0, "Power Source" ),
    array( 102, 5, 0, "Wiring" ),
    array( 51, 20, 0, "Wired Wooden Wall" ),
    array( 101, 50, 0, "Pressure Toggle Switch (Starts Off)" ),
    array( 108, 50, 0, "Pressure Toggle Switch (Starts On)" ),
    array( 100, 50, 0, "Sticking Pressure Switch" ),
    array( 107, 50, 0, "Rotary Toggle Switch" ),
    array( 106, 10, 0, "Wire Bridge" ),
    array( 104, 20, 0, "Voltage-triggered Switch" ),
    array( 105, 20, 0, "Voltage-triggered Inverted Switch" ),
    array( 30, 50, 0, "Automatic Door" ),
    array( 110, 20, 0, "Electric Floor" ),
    array( 112, 150, 0, "Powered Trapdoor" ),
    array( 70, 200, 0, "Pit Bull" ),
    array( 71, 100, 0, "Chihuahua" ),
    array( 72, 20, 0, "Cat" ),
    
    array( 500, 20, 0, "Saw" ),
    array( 509, 20, 0, "Crowbar" ),
    array( 501, 100, 0, "Cutting Torch" ),
    array( 502, 470, 0, "S-W Model 36" ),
    array( 503, 2, 0, "Brick" ),
    array( 504, 26, 0, "Voltage Detector" ),
    array( 505, 6, 0, "Doorstop" ),
    array( 506, 20, 0, "Wire Cutters" ),
    array( 507, 50, 0, "Ammonium Nitrate Diesel Bomb" ),
    array( 508, 8, 0, "Drugged Meat" ),
    array( 510, 2, 0, "Water" ),

    array( 2000, 10000, 1, "Owl - Rohrer" ),
    array( 2001, 10000, 1, "Sky - Rohrer" ),
    array( 2002, 10000, 1, "Everything I Love --- by Stephen Lavelle" ),
    array( 2003, 10000, 1, "Breeze --- by Jenova Chen" ),
    array( 2004, 10000, 1, "Kiwi! --- by Danny Ledonne" ),
    array( 2005, 10000, 1, "Are you embarrassed to dance? --- by Ashly Burch" ),
    array( 2006, 10000, 1, "Lips for Eyelids --- by Ashly Burch" ),
    array( 2007, 10000, 1, "Escape Found --- by Scott Brodie" ),
    array( 2008, 10000, 1, "Old Guard --- by Scott Brodie" ),
    array( 2009, 10000, 1, "Bound Sun --- by Scott Brodie" ),
    array( 2010, 10000, 1, "Eyes Wide Spread --- by Daniel Cook" ),
    array( 2011, 10000, 1, "Down Goes Frazier --- by Jason Stevenson" ),
    array( 2012, 10000, 1, "Yellow Bars --- by Jason Stevenson" ),
    array( 2013, 10000, 1, "Obviously, a major malfunction --- by Jason Stevenson" ),
    array( 2014, 10000, 1, "Flame Out --- by Jason Stevenson" ),
    array( 2015, 10000, 1, "Band of Brothers --- by Jason Stevenson" ),
    array( 2016, 10000, 1, "Mike Shot --- by Jason Stevenson" ),
    array( 2017, 10000, 1, "Tentacles! --- by Jason Stevenson" ),
    array( 2018, 10000, 1, "Zapruder260 --- by Jason Stevenson" ),
    array( 2019, 10000, 1, "Delicate Arch --- by Jason Stevenson" ),
    array( 2020, 10000, 1, "Manhattan --- by William Beebe" ),
    array( 2021, 10000, 1, "Core --- by William Beebe" ),
    array( 2022, 10000, 1, "Night, Harlem --- by William Beebe" ),
    array( 2023, 10000, 1, "Night City --- by William Beebe" ),
    array( 2024, 10000, 1, "Lulu --- by American McGee" ),
    array( 2025, 10000, 1, "Crossed --- by American McGee" ),
    array( 2026, 10000, 1, "Flicker --- by American McGee" ),
    array( 2027, 10000, 1, "Xian --- by American McGee" ),
    array( 2028, 10000, 1, "Whoops --- by Anthony Burch" ),
    array( 2029, 10000, 1, "Today --- by Nick Montfort" )
    );

    
// price change factor when reselling items
$resaleRate = 0.5;


// list of all mobile objects (those that don't block family exit)
$mobileList = array( 70, 71, 72 );

// list of all possible objects to use as wife
$wifeList = array( 1010,
                   1011,
                   1012,
                   1013 );
// same for sons
$sonList = array( 1020,
                  1021,
                  1022,
                  1023 );

// same for daughters
$daughterList = array( 1040,
                       1041,
                       1042,
                       1043 );




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