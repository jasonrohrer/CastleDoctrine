<?php



global $cd_version;
$cd_version = "1";

global $cd_minClientVersion;
$cd_minClientVersion = "1";


// edit settings.php to change server' settings
include( "settings.php" );



// no end-user settings below this point



// no caching
//header('Expires: Mon, 26 Jul 1997 05:00:00 GMT');
header('Cache-Control: no-store, no-cache, must-revalidate');
header('Cache-Control: post-check=0, pre-check=0', FALSE);
header('Pragma: no-cache'); 



// enable verbose error reporting to detect uninitialized variables
error_reporting( E_ALL );



// page layout for web-based setup
$setup_header = "
<HTML>
<HEAD><TITLE>Castle Doctrine Server Web-based setup</TITLE></HEAD>
<BODY BGCOLOR=#FFFFFF TEXT=#000000 LINK=#0000FF VLINK=#FF0000>

<CENTER>
<TABLE WIDTH=75% BORDER=0 CELLSPACING=0 CELLPADDING=1>
<TR><TD BGCOLOR=#000000>
<TABLE WIDTH=100% BORDER=0 CELLSPACING=0 CELLPADDING=10>
<TR><TD BGCOLOR=#EEEEEE>";

$setup_footer = "
</TD></TR></TABLE>
</TD></TR></TABLE>
</CENTER>
</BODY></HTML>";






// ensure that magic quotes are on (adding slashes before quotes
// so that user-submitted data can be safely submitted in DB queries)
if( get_magic_quotes_gpc() ) {
    // force magic quotes to be added
    $_GET     = array_map( 'cd_stripslashes_deep', $_GET );
    $_POST    = array_map( 'cd_stripslashes_deep', $_POST );
    $_REQUEST = array_map( 'cd_stripslashes_deep', $_REQUEST );
    $_COOKIE  = array_map( 'cd_stripslashes_deep', $_COOKIE );
    }
    





// all calls need to connect to DB, so do it once here
cd_connectToDatabase();

// close connection down below (before function declarations)


// testing:
//sleep( 5 );


// general processing whenver server.php is accessed directly




// grab POST/GET variables
$action = cd_requestFilter( "action", "/[A-Z_]+/i" );


if( $action != "" && $action != "cd_setup" ) {
    // don't check for flush if we may be executing initial database setup
    // (flush breaks in that case)
    cd_checkForFlush();
    }


/*
// debugging:  log full request vars
$fullRequest = "";
foreach( $_REQUEST as $key => $value ) {
    $fullRequest = $fullRequest . ", " . $key . "=>". $value;
    }
cd_log( "Web request with vars $fullRequest" );
*/

$debug = cd_requestFilter( "debug", "/[01]/" );

$remoteIP = "";
if( isset( $_SERVER[ "REMOTE_ADDR" ] ) ) {
    $remoteIP = $_SERVER[ "REMOTE_ADDR" ];
    }


// for debugging of bad server responses
if( false ) {
    echo "BLAHH";
    cd_closeDatabase();
    die();
    }

if( $action == "version" ) {
    global $cd_version;
    echo "$cd_version";
    }
else if( $action == "show_log" ) {
    cd_showLog();
    }
else if( $action == "clear_log" ) {
    cd_clearLog();
    }
else if( $action == "check_user" ) {
    cd_checkUser();
    }
else if( $action == "check_hash" ) {
    cd_checkHash();
    }
else if( $action == "start_edit_house" ) {
    cd_startEditHouse();
    }
else if( $action == "end_edit_house" ) {
    cd_endEditHouse();
    }
else if( $action == "ping_house" ) {
    cd_pingHouse();
    }
else if( $action == "list_houses" ) {
    cd_listHouses();
    }
else if( $action == "start_rob_house" ) {
    cd_startRobHouse();
    }
else if( $action == "end_rob_house" ) {
    cd_endRobHouse();
    }
else if( $action == "list_logged_robberies" ) {
    cd_listLoggedRobberies();
    }
else if( $action == "get_robbery_log" ) {
    cd_getRobberyLog();
    }
else if( $action == "list_auctions" ) {
    cd_listAuctions();
    }
else if( $action == "buy_auction" ) {
    cd_buyAuction();
    }
else if( $action == "show_data" ) {
    cd_showData();
    }
else if( $action == "show_detail" ) {
    cd_showDetail();
    }
else if( $action == "block_user_id" ) {
    cd_blockUserID();
    }
else if( $action == "update_prices" ) {
    cd_updatePrices();
    }
else if( $action == "default_prices" ) {
    cd_defaultPrices();
    }
else if( $action == "delete_price" ) {
    cd_deletePrice();
    }
else if( $action == "logout" ) {
    cd_logout();
    }
else if( $action == "cd_setup" ) {
    global $setup_header, $setup_footer;
    echo $setup_header; 

    echo "<H2>Castle Doctrine Server Web-based Setup</H2>";

    echo "Creating tables:<BR>";

    echo "<CENTER><TABLE BORDER=0 CELLSPACING=0 CELLPADDING=1>
          <TR><TD BGCOLOR=#000000>
          <TABLE BORDER=0 CELLSPACING=0 CELLPADDING=5>
          <TR><TD BGCOLOR=#FFFFFF>";

    cd_setupDatabase();

    echo "</TD></TR></TABLE></TD></TR></TABLE></CENTER><BR><BR>";
    
    echo $setup_footer;
    }
else if( $action != "" ) {
    cd_log( "Unknown action request:  $action" );
    echo "DENIED";
    }
else if( preg_match( "/server\.php/", $_SERVER[ "SCRIPT_NAME" ] ) ) {
    // server.php has been called without an action parameter

    // the preg_match ensures that server.php was called directly and
    // not just included by another script
    
    // quick (and incomplete) test to see if we should show instructions
    global $tableNamePrefix;
    
    // check if our tables exists
    $allExist =
        cd_doesTableExist( $tableNamePrefix."server_globals" ) &&
        cd_doesTableExist( $tableNamePrefix."log" ) &&
        cd_doesTableExist( $tableNamePrefix."users" ) &&
        cd_doesTableExist( $tableNamePrefix."houses" ) &&
        cd_doesTableExist( $tableNamePrefix."robbery_logs" ) &&
        cd_doesTableExist( $tableNamePrefix."limbo_robberies" ) &&
        cd_doesTableExist( $tableNamePrefix."prices" ) &&
        cd_doesTableExist( $tableNamePrefix."auction" ) &&
        cd_doesTableExist( $tableNamePrefix."last_names" ) &&
        cd_doesTableExist( $tableNamePrefix."first_names" );
    
        
    if( $allExist  ) {
        echo "Castle Doctrine Server database setup and ready";
        }
    else {
        // start the setup procedure

        global $setup_header, $setup_footer;
        echo $setup_header; 

        echo "<H2>Castle Doctrine Server Web-based Setup</H2>";
    
        echo "Server will walk you through a " .
            "brief setup process.<BR><BR>";
        
        echo "Step 1: ".
            "<A HREF=\"server.php?action=cd_setup\">".
            "create the database tables</A>";

        echo $setup_footer;
        }
    }



// done processing
// only function declarations below

cd_closeDatabase();




function cd_populateNameTable( $inFileName, $inTableName ) {
    $totalPopulation = 257746103;
    

    global $tableNamePrefix;

    $tableName = $tableNamePrefix . $inTableName;

    
    if( $file = fopen( $inFileName, "r" ) ) {
        $firstLine = true;

        $query = "INSERT INTO $tableName VALUES ";
        /*
			(1, 'Komal',10),
			(2, 'Ajay',10),
			(3, 'Santosh',10),
			(4, 'Rakesh',10),
			(5, 'Bhau',10);
        */

        
        while( !feof( $file ) ) {
            $line = fgets( $file);

            $tokens = preg_split( "/\s+/", trim( $line ) );
            
            if( count( $tokens ) == 4 ) {
                
                $name = $tokens[0];
                $cumulativePercent = $tokens[2];
                
                $cumulative_count =
                    ( $cumulativePercent / 100 ) * $totalPopulation;
                
                if( ! $firstLine ) {
                    $query = $query . ",";
                    }
                
                $query = $query . " ( $cumulative_count, '$name' )";
                
                $firstLine = false;
                }
            }
        
        fclose( $file );

        $query = $query . ";";

        $result = cd_queryDatabase( $query );
        }
    }



// picks name according to cumulative distribution
function cd_pickName( $inTableName ) {
    global $tableNamePrefix;

    $tableName = $tableNamePrefix . $inTableName;

    $query = "SELECT FLOOR( RAND() * MAX( cumulative_count) ) FROM ".
        $tableName .";";

    $result = cd_queryDatabase( $query );

    $cumulativeTarget = mysql_result( $result, 0, 0 );

    $query = "SELECT MIN( cumulative_count ) FROM ". $tableName .
        " WHERE cumulative_count > $cumulativeTarget;";

    $result = cd_queryDatabase( $query );

    $cumulativePick = mysql_result( $result, 0, 0 );

    $query = "SELECT name FROM ". $tableName .
        " WHERE cumulative_count = $cumulativePick;";
    
    $result = cd_queryDatabase( $query );

    return ucfirst( strtolower(
                        mysql_result( $result, 0, 0 ) ) );
    }



function cd_pickFullName() {
    $firstName = cd_pickName( "first_names" );

    $middleName = cd_pickName( "first_names" );

    while( $middleName == $firstName ) {
        $middleName = cd_pickName( "first_names" );
        }
    
    $character_name =
        $firstName . "_" .
        $middleName . "_" .
        cd_pickName( "last_names" );

    return $character_name;
    }



function cd_restoreDefaultPrices() {
    // default prices

    
    global $tableNamePrefix, $defaultPrices;
    
    $tableName = $tableNamePrefix . "prices";

    // clear old
    $query = "DELETE FROM $tableName";
    cd_queryDatabase( $query );

    
    foreach( $defaultPrices as $tuple ) {
        $object_id = $tuple[0];
        $price = $tuple[1];
        $in_gallery = $tuple[2];
        $note = mysql_real_escape_string( $tuple[3] );
        
        $query = "INSERT INTO $tableName ".
            "VALUES ( '$object_id', '$price', '$in_gallery', '$note' )";

        cd_queryDatabase( $query );
        }
    
    }


function cd_startAuction( $object_id, $start_price ) {
    global $tableNamePrefix, $auctionPriceDropInterval;

    $tableName = $tableNamePrefix . "auction";
    
    $dropInt = $auctionPriceDropInterval;
        
    // DATE_SUB trick to round time down to nearest 3-minute interval
    // found here:
    // http://stackoverflow.com/questions/9680144/
    //        mysql-date-time-round-to-nearest-hour
    $query = "INSERT INTO $tableName ".
        "VALUES ( '$object_id', '$start_price', " .
        "DATE_SUB( ".
        "  DATE_SUB( CURRENT_TIMESTAMP, ".
        "            INTERVAL MOD( MINUTE(CURRENT_TIMESTAMP), ".
        "                          $dropInt ) MINUTE ), ".
        "  INTERVAL SECOND(CURRENT_TIMESTAMP) SECOND ) )";
    
    cd_queryDatabase( $query );
    }



function cd_startInitialAuctions() {
    
    global $tableNamePrefix;    

    // clear any old
    cd_queryDatabase( "DELETE FROM $tableNamePrefix"."auction" );
    
    
    $query = "SELECT object_id, price FROM ".
        "$tableNamePrefix"."prices WHERE in_gallery = 1;";
    
    $result = cd_queryDatabase( $query );

    $numRows = mysql_numrows( $result );

    for( $i=0; $i<$numRows; $i++ ) {
        $object_id = mysql_result( $result, $i, "object_id" );
        $price = mysql_result( $result, $i, "price" );
        
        cd_startAuction( $object_id, $price );
        }    
    }








/**
 * Creates the database tables needed by seedBlogs.
 */
function cd_setupDatabase() {
    global $tableNamePrefix;


    $tableName = $tableNamePrefix . "server_globals";
    if( ! cd_doesTableExist( $tableName ) ) {

        // this table contains general info about the server
        // use INNODB engine so table can be locked
        $query =
            "CREATE TABLE $tableName(" .
            "last_flush_time DATETIME NOT NULL ) ENGINE = INNODB;";

        $result = cd_queryDatabase( $query );

        echo "<B>$tableName</B> table created<BR>";

        // create one row
        $query = "INSERT INTO $tableName VALUES ( CURRENT_TIMESTAMP );";
        $result = cd_queryDatabase( $query );
        }
    else {
        echo "<B>$tableName</B> table already exists<BR>";
        }


    
    $tableName = $tableNamePrefix . "log";
    if( ! cd_doesTableExist( $tableName ) ) {

        $query =
            "CREATE TABLE $tableName(" .
            "entry TEXT NOT NULL, ".
            "entry_time DATETIME NOT NULL );";

        $result = cd_queryDatabase( $query );

        echo "<B>$tableName</B> table created<BR>";
        }
    else {
        echo "<B>$tableName</B> table already exists<BR>";
        }


    
    $tableName = $tableNamePrefix . "users";
    if( ! cd_doesTableExist( $tableName ) ) {

        // this table contains general info about each user
        // unique ID is ticket ID from ticket server
        //
        // sequence number used and incremented with each client request
        // to prevent replay attacks
        $query =
            "CREATE TABLE $tableName(" .
            "user_id INT NOT NULL PRIMARY KEY AUTO_INCREMENT," .
            "ticket_id CHAR(10) NOT NULL," .
            "sequence_number INT NOT NULL," .
            "last_price_list_number INT NOT NULL," .
            "blocked TINYINT NOT NULL ) ENGINE = INNODB;";

        $result = cd_queryDatabase( $query );

        echo "<B>$tableName</B> table created<BR>";
        }
    else {
        echo "<B>$tableName</B> table already exists<BR>";
        }

    
    
    $tableName = $tableNamePrefix . "houses";
    if( ! cd_doesTableExist( $tableName ) ) {

        // this table contains general info about each user's house
        // EVERY user has EXACTLY ONE house
        $query =
            "CREATE TABLE $tableName(" .
            "user_id INT NOT NULL PRIMARY KEY," .
            "character_name VARCHAR(62) NOT NULL," .
            "UNIQUE KEY( character_name )," .
            "house_map LONGTEXT NOT NULL," .
            "vault_contents LONGTEXT NOT NULL," .
            "backpack_contents LONGTEXT NOT NULL," .
            "gallery_contents LONGTEXT NOT NULL," .
            // times edited since last successful robbery
            "edit_count INT NOT NULL," .
            "loot_value INT NOT NULL," .
            "edit_checkout TINYINT NOT NULL,".
            "rob_checkout TINYINT NOT NULL,".
            // ignored if not checked out for robbery
            "robbing_user_id INT NOT NULL," .
            "rob_attempts INT NOT NULL,".
            "last_ping_time DATETIME NOT NULL,".
            "blocked TINYINT NOT NULL ) ENGINE = INNODB;";

        $result = cd_queryDatabase( $query );

        echo "<B>$tableName</B> table created<BR>";
        }
    else {
        echo "<B>$tableName</B> table already exists<BR>";
        }


    
    $tableName = $tableNamePrefix . "limbo_robberies";
    if( ! cd_doesTableExist( $tableName ) ) {

        // contains information about lapsed robberies (user quits, power
        // out, etc)
        // User gets one success/failure(death) chance to rob each house
        // with current life.  If user "bails out" mid-robbery,
        // it's unfair to punish them (might not be their fault, game crash?),
        // but we don't want them to exploit it as another chance to rob
        // with new info.
        // So, lapsed attempts go into the limbo list, and user is not
        // allowed to try robbing that same house again.

        // This maps a user ID to another user ID, where the second
        // ID specifies a house that the first user cannot rob again.
        $query =
            "CREATE TABLE $tableName(" .
            "user_id INT NOT NULL," .
            "house_user_id INT NOT NULL," .
            "PRIMARY KEY( user_id, house_user_id ) ) ENGINE = INNODB;";

        $result = cd_queryDatabase( $query );

        echo "<B>$tableName</B> table created<BR>";
        }
    else {
        echo "<B>$tableName</B> table already exists<BR>";
        }


    
    $tableName = $tableNamePrefix . "robbery_logs";
    if( ! cd_doesTableExist( $tableName ) ) {

        // contains move log for each robbery

        $query =
            "CREATE TABLE $tableName(" .
            "log_id INT NOT NULL PRIMARY KEY AUTO_INCREMENT," .
            "user_id INT NOT NULL," .
            "house_user_id INT NOT NULL," .
            "loot_value INT NOT NULL," .
            "rob_attempts INT NOT NULL,".
            "robber_name VARCHAR(62) NOT NULL," .
            "victim_name VARCHAR(62) NOT NULL," .
            "rob_time DATETIME NOT NULL,".
            "house_start_map LONGTEXT NOT NULL," .
            "loadout LONGTEXT NOT NULL," .
            "move_list LONGTEXT NOT NULL," .
            "house_end_map LONGTEXT NOT NULL ) ENGINE = INNODB;";

        $result = cd_queryDatabase( $query );

        echo "<B>$tableName</B> table created<BR>";
        }
    else {
        echo "<B>$tableName</B> table already exists<BR>";
        }    



    $tableName = $tableNamePrefix . "prices";
    if( ! cd_doesTableExist( $tableName ) ) {

        $query =
            "CREATE TABLE $tableName(" .
            "object_id INT NOT NULL PRIMARY KEY," .
            "price INT NOT NULL, ".
            "in_gallery TINYINT NOT NULL, ".
            "note LONGTEXT NOT NULL ) ENGINE = INNODB;";

        $result = cd_queryDatabase( $query );

        echo "<B>$tableName</B> table created<BR>";
        cd_restoreDefaultPrices();
        }
    else {
        echo "<B>$tableName</B> table already exists<BR>";
        }


    
    $tableName = $tableNamePrefix . "auction";
    if( ! cd_doesTableExist( $tableName ) ) {

        $query =
            "CREATE TABLE $tableName(" .
            "object_id INT NOT NULL PRIMARY KEY," .
            "start_price INT NOT NULL, ".
            "start_time DATETIME NOT NULL ) ENGINE = INNODB;";

        $result = cd_queryDatabase( $query );

        echo "<B>$tableName</B> table created<BR>";
        cd_startInitialAuctions();
        }
    else {
        echo "<B>$tableName</B> table already exists<BR>";
        }
    
    
    

    $tableName = $tableNamePrefix . "last_names";
    if( ! cd_doesTableExist( $tableName ) ) {

        // a source list of character last names
        // cumulative count is number of people in 1993 population
        // who have this name or a more common name
        // less common names have higher cumulative counts
        $query =
            "CREATE TABLE $tableName(" .
            "cumulative_count INT NOT NULL PRIMARY KEY," .
            "name VARCHAR(20) NOT NULL );";

        $result = cd_queryDatabase( $query );

        echo "<B>$tableName</B> table created<BR>";

        cd_populateNameTable( "namesLast.txt", "last_names" );
        }
    else {
        echo "<B>$tableName</B> table already exists<BR>";
        }


    $tableName = $tableNamePrefix . "first_names";
    if( ! cd_doesTableExist( $tableName ) ) {


        // a source list of character first names
        // cumulative count is number of people in 1993 population
        // who have this name or a more common name
        // less common names have higher cumulative counts
        $query =
            "CREATE TABLE $tableName(" .
            "cumulative_count INT NOT NULL PRIMARY KEY," .
            "name VARCHAR(20) NOT NULL );";

        $result = cd_queryDatabase( $query );

        echo "<B>$tableName</B> table created<BR>";

        cd_populateNameTable( "namesFirst.txt", "first_names" );
        }
    else {
        echo "<B>$tableName</B> table already exists<BR>";
        }


    }



function cd_showLog() {
    cd_checkPassword( "show_log" );

     echo "[<a href=\"server.php?action=show_data" .
         "\">Main</a>]<br><br><br>";
    
    global $tableNamePrefix;

    $query = "SELECT entry, entry_time FROM $tableNamePrefix"."log ".
        "ORDER BY entry_time DESC;";
    $result = cd_queryDatabase( $query );

    $numRows = mysql_numrows( $result );



    echo "<a href=\"server.php?action=clear_log\">".
        "Clear log</a>";
        
    echo "<hr>";
        
    echo "$numRows log entries:<br><br><br>\n";
        

    for( $i=0; $i<$numRows; $i++ ) {
        $time = mysql_result( $result, $i, "entry_time" );
        $entry = mysql_result( $result, $i, "entry" );

        echo "<b>$time</b>:<br>$entry<hr>\n";
        }
    }



function cd_clearLog() {
    cd_checkPassword( "clear_log" );

     echo "[<a href=\"server.php?action=show_data" .
         "\">Main</a>]<br><br><br>";
    
    global $tableNamePrefix;

    $query = "DELETE FROM $tableNamePrefix"."log;";
    $result = cd_queryDatabase( $query );
    
    if( $result ) {
        echo "Log cleared.";
        }
    else {
        echo "DELETE operation failed?";
        }
    }






// check if we should flush stale checkouts from the database
// do this once every 2 minutes
function cd_checkForFlush() {
    global $tableNamePrefix;

    $tableName = "$tableNamePrefix"."server_globals";
    
    if( !cd_doesTableExist( $tableName ) ) {
        return;
        }
    
    
    cd_queryDatabase( "SET AUTOCOMMIT = 0;" );

    
    $query = "SELECT last_flush_time FROM $tableName ".
        "WHERE last_flush_time < ".
        "SUBTIME( CURRENT_TIMESTAMP, '0 0:02:0.000' ) ".
        "FOR UPDATE;";

    $result = cd_queryDatabase( $query );

    if( mysql_numrows( $result ) > 0 ) {

        // last flush time is old

        global $tableNamePrefix;


        // for each robber who quit game mid-robbery, clear robbery checkout
        // and add entry to limbo table
        $query = "SELECT robbing_user_id FROM $tableNamePrefix"."houses ".
            "WHERE rob_checkout = 1 ".
            "AND last_ping_time < ".
            "SUBTIME( CURRENT_TIMESTAMP, '0 0:05:0.000' ) FOR UPDATE;";

        $result = cd_queryDatabase( $query );

        $numRows = mysql_numrows( $result );
    
        for( $i=0; $i<$numRows; $i++ ) {

            $robbing_user_id = mysql_result( $result, $i, "robbing_user_id" );

            cd_processStaleRobberies( $robbing_user_id );
            }
        
        
        // now clear checkout status on stale edit checkouts
        $query = "UPDATE $tableNamePrefix"."houses ".
            "SET rob_checkout = 0, edit_checkout = 0 ".
            "WHERE edit_checkout = 1 ".
            "AND last_ping_time < ".
            "SUBTIME( CURRENT_TIMESTAMP, '0 0:05:0.000' );";

        $result = cd_queryDatabase( $query );

        $numRowsRemoved = mysql_affected_rows();

        cd_log( "Flush operation checked back in $numRowsRemoved ".
                "stale houses." );

        global $enableLog;
        
        if( $enableLog ) {
            // count remaining games for log
            $query = "SELECT COUNT(*) FROM $tableNamePrefix"."houses ".
                "WHERE rob_checkout = 1 OR edit_checkout = 1;";

            $result = cd_queryDatabase( $query );

            $count = mysql_result( $result, 0, 0 );

            cd_log( "After flush, $count houses still checked out." );
            }

        
        // set new flush time

        $query = "UPDATE $tableName SET " .
            "last_flush_time = CURRENT_TIMESTAMP;";
    
        $result = cd_queryDatabase( $query );

    
        }

    cd_queryDatabase( "COMMIT;" );

    cd_queryDatabase( "SET AUTOCOMMIT = 1;" );
    }








function cd_checkUser() {
    global $tableNamePrefix, $ticketServerNamePrefix;

    $email = cd_requestFilter( "email", "/[A-Z0-9._%+-]+@[A-Z0-9.-]+/i" );
    
    $query = "SELECT ticket_id, blocked ".
        "FROM $ticketServerNamePrefix"."tickets ".
        "WHERE email = '$email';";
    $result = cd_queryDatabase( $query );
    
    $numRows = mysql_numrows( $result );

    // what if same email signs up multiple times?
    // no way to prevent this through FastSpring payments
    // and it will be a rare case.  Just assume it's the FIRST result
    // back.  Users who have trouble with this can email me to complain, and
    // I can fix it manually by changing their subsequent email addresses
    // in the ticket server.
    if( $numRows < 1 ) {
        echo "DENIED";

        cd_log( "checkUser for $email DENIED, email not found" );
        return;
        }
    
    
    $row = mysql_fetch_array( $result, MYSQL_ASSOC );
    
    $ticket_id = $row[ "ticket_id" ];
    $blocked = $row[ "blocked" ];

    if( $blocked ) {
        echo "DENIED";

        cd_log( "checkUser for $email DENIED, blocked on ticket server" );
        return;
        }

    cd_queryDatabase( "SET AUTOCOMMIT=0" );
    
    
    $query = "SELECT user_id, blocked, sequence_number ".
        "FROM $tableNamePrefix"."users ".
        "WHERE ticket_id = '$ticket_id' FOR UPDATE;";
    $result = cd_queryDatabase( $query );
    
    $numRows = mysql_numrows( $result );

    $user_id;
    $sequence_number;
    
    if( $numRows < 1 ) {
        // new user, in ticket server but not here yet

        // create

        // user_id auto-assigned
        $query = "INSERT INTO $tableNamePrefix"."users ".
            "(ticket_id, sequence_number, last_price_list_number, blocked) ".
            "VALUES(" .
            " '$ticket_id', 0, 0, 0 );";
        $result = cd_queryDatabase( $query );

        $user_id = mysql_insert_id();
        $sequence_number = 0;
        
        
        cd_queryDatabase( "COMMIT;" );
        cd_queryDatabase( "SET AUTOCOMMIT=1" );
        

        cd_newHouseForUser( $user_id );
        }
    else {
        $row = mysql_fetch_array( $result, MYSQL_ASSOC );
    
        $blocked = $row[ "blocked" ];

        cd_queryDatabase( "COMMIT;" );
        cd_queryDatabase( "SET AUTOCOMMIT=1" );
        
        
        if( $blocked ) {
            echo "DENIED";

            

            cd_log( "checkUser for $email DENIED, blocked on castle server" );

            
            return;
            }

        $user_id = $row[ "user_id" ];
        $sequence_number = $row[ "sequence_number" ];
        }

    global $cd_minClientVersion;
    
    echo "$cd_minClientVersion $user_id $sequence_number OK";
    }



function cd_checkHash() {
    if( ! cd_verifyTransaction() ) {
        return;
        }

    echo "OK";
    }


function cd_processStaleRobberies( $user_id ) {
    global $tableNamePrefix;


    // first find all stale robberies
    $query = "SELECT user_id FROM $tableNamePrefix"."houses ".
        "WHERE rob_checkout = 1 AND robbing_user_id = '$user_id';";
    $result = cd_queryDatabase( $query );


    
    $numRows = mysql_numrows( $result );
    
    for( $i=0; $i<$numRows; $i++ ) {
        $house_user_id = mysql_result( $result, $i, "user_id" );

        // add each one to the limbo table
        $query = "INSERT INTO $tableNamePrefix"."limbo_robberies VALUES(" .
            " $user_id, $house_user_id );";
        cd_queryDatabase( $query );
        }
    

    // finally clear all the robberies themselves
    $query = "UPDATE $tableNamePrefix"."houses SET ".
        "rob_checkout = 0 WHERE robbing_user_id = '$user_id';";
    cd_queryDatabase( $query );
    }



function cd_startEditHouse() {
    global $tableNamePrefix;

    if( ! cd_verifyTransaction() ) {
        return;
        }

    $user_id = cd_getUserID();

    
    cd_queryDatabase( "SET AUTOCOMMIT=0" );

    cd_processStaleRobberies( $user_id );
    
    
    // automatically ignore blocked users and houses already checked
    // out for robbery
    
    $query = "SELECT house_map, vault_contents, backpack_contents, ".
        "gallery_contents, ".
        "loot_value, edit_count FROM $tableNamePrefix"."houses ".
        "WHERE user_id = '$user_id' AND blocked='0' ".
        "AND rob_checkout = 0 FOR UPDATE;";

    $result = cd_queryDatabase( $query );

    $numRows = mysql_numrows( $result );
    
    if( $numRows < 1 ) {
        cd_transactionDeny();
        return;
        }
    $row = mysql_fetch_array( $result, MYSQL_ASSOC );

    $house_map = $row[ "house_map" ];
    $vault_contents = $row[ "vault_contents" ];
    $backpack_contents = $row[ "backpack_contents" ];
    $gallery_contents = $row[ "gallery_contents" ];
    $loot_value = $row[ "loot_value" ];
    $edit_count = $row[ "edit_count" ];


    
    $query = "UPDATE $tableNamePrefix"."houses SET ".
        "edit_checkout = 1, last_ping_time = CURRENT_TIMESTAMP ".
        "WHERE user_id = $user_id;";
    cd_queryDatabase( $query );


    
    $query = "SELECT last_price_list_number FROM $tableNamePrefix"."users ".
        " WHERE user_id = '$user_id' FOR UPDATE;";

    $result = cd_queryDatabase( $query );

    $numRows = mysql_numrows( $result );
    
    if( $numRows < 1 ) {
        cd_transactionDeny();
        return;
        }
    $row = mysql_fetch_array( $result, MYSQL_ASSOC );

    $last_price_list_number = $row[ "last_price_list_number" ];


    $last_price_list_number ++;


    $query = "UPDATE $tableNamePrefix"."users SET ".
        "last_price_list_number = '$last_price_list_number' ".
        "WHERE user_id = $user_id;";
    cd_queryDatabase( $query );

    
    
    cd_queryDatabase( "COMMIT;" );
    cd_queryDatabase( "SET AUTOCOMMIT=1" );


    $query = "SELECT object_id, price FROM $tableNamePrefix"."prices;";

    $result = cd_queryDatabase( $query );

    $numRows = mysql_numrows( $result );

    $priceListBody = "";

    $firstRow = true;
    
    for( $i=0; $i<$numRows; $i++ ) {
        if( !$firstRow ) {
            $priceListBody = $priceListBody . "#";
            }
        $firstRow = false;
            
        $object_id = mysql_result( $result, $i, "object_id" );
        $price = mysql_result( $result, $i, "price" );

        $priceListBody = $priceListBody . "$object_id"."@"."$price";
        }

    global $serverSecretKey;
    
    $signature =
        sha1( $last_price_list_number . $priceListBody . $serverSecretKey );


    $must_self_test = 0;

    if( $edit_count == 0 ) {
        $must_self_test = 1;
        }
        
        
    
    echo $house_map;
    echo "\n";
    echo $vault_contents;
    echo "\n";
    echo $backpack_contents;
    echo "\n";
    echo $gallery_contents;
    echo "\n";
    echo $last_price_list_number . ":" . $priceListBody . ":" . $signature;
    echo "\n";
    echo $loot_value;
    echo "\n";
    echo $must_self_test;
    echo "\nOK";
    }




function cd_idQuanityStringToArray( $inIDQuantityString ) {

    $pairArray = preg_split( "/#/", $inIDQuantityString );

    $result = array();
    
    if( $inIDQuantityString == "#" ) {
        return $result;
        }

    foreach( $pairArray as $pair ) {
        $pairParts = preg_split( "/:/", $pair );

        if( count( $pairParts ) == 2 ) {
            $id = $pairParts[0];
            $quantity = $pairParts[1];

            $result[ $id ] = $quantity;
            }
        }
    return $result;
    }



function cd_idQuanityArrayToString( $inArray ) {
    ksort( $inArray );
    
    $pairArray = array();
    
    foreach( $inArray as $id => $quantity ) {
        // append
        $pairArray[] = "$id:$quantity";
        }

    $resultString = implode( "#", $pairArray );

    if( $resultString == "" ) {
        $resultString = "#";
        }

    return $resultString;
    }



// takes strings that are ID:quantity pairs, like:
// 101:3#3:10#5:1#102:1
//
// And computes a union operation, returning a new string
// (where the quanity for each ID in the result is the sum of the
//  quantities of that ID in A and B)
function cd_idQuantityUnion( $inIDQuantityStringA, $inIDQuantityStringB ) {

    $arrayA = cd_idQuanityStringToArray( $inIDQuantityStringA );
    $arrayB = cd_idQuanityStringToArray( $inIDQuantityStringB );
    
    // start with B as base, sum in (or append) ID/quantities from A
    $result = $arrayB;

    foreach( $arrayA as $id => $quantity ) {    
        if( array_key_exists( $id, $result ) ) {
            // exists in B, sum with A's quantity
            $result[ $id ] += $quantity;
            }
        else {
            // doesn't exist in B, insert A's quantity
            $result[ $id ] = $quantity;
            }
        }

    return cd_idQuanityArrayToString( $result );
    }





function cd_endEditHouse() {
    global $tableNamePrefix;

    if( ! cd_verifyTransaction() ) {
        return;
        }

    $user_id = cd_getUserID();

    
    cd_queryDatabase( "SET AUTOCOMMIT=0" );

    // automatically ignore blocked users and houses already checked
    // out for robbery
    
    $query = "SELECT user_id, edit_count, loot_value, house_map, ".
        "vault_contents, backpack_contents, gallery_contents ".
        "FROM $tableNamePrefix"."houses ".
        "WHERE user_id = '$user_id' AND blocked='0' ".
        "AND rob_checkout = 0 and edit_checkout = 1 FOR UPDATE;";

    $result = cd_queryDatabase( $query );

    $numRows = mysql_numrows( $result );
    
    if( $numRows < 1 ) {
        cd_transactionDeny();
        return;
        }
    $row = mysql_fetch_array( $result, MYSQL_ASSOC );

    $edit_count = $row[ "edit_count" ];
    $loot_value = $row[ "loot_value" ];
    $old_house_map = $row[ "house_map" ];
    $old_vault_contents = $row[ "vault_contents" ];
    $old_backpack_contents = $row[ "backpack_contents" ];
    $old_gallery_contents = $row[ "gallery_contents" ];
    
    $edit_count ++;
    
    $house_map = cd_requestFilter( "house_map", "/[#0-9,:!]+/" );

    $vault_contents = cd_requestFilter( "vault_contents", "/[#0-9:]+/" );

    $backpack_contents = cd_requestFilter( "backpack_contents", "/[#0-9:]+/" );

    $gallery_contents = cd_requestFilter( "gallery_contents", "/[#0-9]+/" );

    $price_list = cd_requestFilter( "price_list",
                                    "/\d+:[0-9@#]+:[A-F0-9]+/i" );

    $edit_list = cd_requestFilter( "edit_list", "/[0-9@#]+/" );

    $purchase_list = cd_requestFilter( "purchase_list", "/[#0-9:]+/" );

    
    $died = cd_requestFilter( "died", "/[01]/" );


    if( $died == 1 ) {
        // don't need to check edit, because player died and house
        // will be destroyed anyway

        cd_queryDatabase( "COMMIT;" );
        cd_queryDatabase( "SET AUTOCOMMIT=1" );

        
        cd_newHouseForUser( $user_id );

        echo "OK";

        // skip rest
        return;
        }
    

    

    $query = "SELECT last_price_list_number ".
        "FROM $tableNamePrefix"."users ".
        "WHERE user_id = '$user_id';";

    $result = cd_queryDatabase( $query );

    $numRows = mysql_numrows( $result );
    
    if( $numRows < 1 ) {
        cd_transactionDeny();
        return;
        }
    $row = mysql_fetch_array( $result, MYSQL_ASSOC );

    $last_price_list_number = $row[ "last_price_list_number" ];


    $priceListParts = preg_split( "/:/", $price_list );


    if( count( $priceListParts ) != 3 ) {
        cd_log( "House check-in with badly-formatted price list denied" );
        cd_transactionDeny();
        return;
        }

    if( $last_price_list_number != $priceListParts[0] ) {
        cd_log( "House check-in with stale price list denied" );
        cd_transactionDeny();
        return;
        }

    $priceListBody = $priceListParts[1];
    $sig = $priceListParts[2];
    
    global $serverSecretKey;
    
    $true_sig =
        sha1( $last_price_list_number . $priceListBody . $serverSecretKey );

    if( $true_sig != $sig ) {
        cd_log( "House check-in with badly-signed price list denied" );
        cd_transactionDeny();
        return;
        }


    // valid, fresh price list, signed by this server!

    
    $priceList = preg_split( "/#/", $priceListBody );

    // build an array mapping object_id => price
    $priceArray = array();

    $numPrices = count( $priceList );


    for( $i=0; $i<$numPrices; $i++ ) {
        $priceParts = preg_split( "/@/", $priceList[$i] );
        
        $priceArray[ $priceParts[0] ] = $priceParts[1];
        }

    // finally, stick 0 prices for vault placement and floor placement
    // (floor placement is erasing---free)
    $priceArray[ 999 ] = 0;
    $priceArray[ 0 ] = 0;
    
    

    // now we need to check new house map to make sure it is a valid edit

    $houseArray = preg_split( "/#/", $house_map );

    $numHouseCells = count( $houseArray );

    // check 0:
    // house map is 32x32
    if( $numHouseCells != 32 * 32 ) {
        cd_log( "House check-in with $numHouseCells map cells denied" );
        cd_transactionDeny();
        return;
        }
    
    
    // check 1:
    // make sure it has exactly 1 vault and correct number of ext walls
    
    $vaultCount = 0;
    $extWallCount = 0;
    for( $i=0; $i<$numHouseCells; $i++ ) {
        if( $houseArray[$i] == 999 ) {
            $vaultCount++;
            }
        else if( $houseArray[$i] == 998 ) {
            $extWallCount++;
            }
        }

    if( $vaultCount != 1 ||
        $extWallCount !=
        // top row, bottom row
        32 + 32 +
        // edges between top and bottom
        2 * 30
        // empty spot on left edge for start position
        - 1 ) {

        cd_log(
            "House check-in with bad vault count ($vaultCount) ".
            "or ext wall count ($extWallCount) denied" );

        cd_transactionDeny();
        return;
        }


    // check 2:
    // make sure the start location is empty

    // row 16, column 0
    $startIndex = 16 * 32 + 0;

    if( $houseArray[ $startIndex ] != 0 ) {
        cd_log( "House check-in with bad start location on map denied" );
        cd_transactionDeny();
        return;
        }

    
    // check 3:
    // all edges must be exterior walls (or blank for start spot)

    // top row
    for( $i=0; $i<32; $i++ ) {
        if( $houseArray[$i] != 998 ) {
            cd_log( "House check-in with bad ext wall placement denied" );
            cd_transactionDeny();
            return;
            }
        }

    // bottom row
    for( $i=31*32; $i<32*32; $i++ ) {
        if( $houseArray[$i] != 998 ) {
            cd_log( "House check-in with bad ext wall placement denied" );
            cd_transactionDeny();
            return;
            }
        }

    // sides in between top and bottom rows
    for( $y=1; $y<31; $y++ ) {

        $left = $y * 32;

        if( $houseArray[$left] != 998 ) {
            // start location is one exception
            if( $y != 16 ) {
                cd_log( "House check-in with bad ext wall placement denied" );
                cd_transactionDeny();
                return;
                }
            }
        
        
        $right = $y * 32 + 31;
        if( $houseArray[$right] != 998 ) {
            cd_log( "House check-in with bad ext wall placement denied" );
            cd_transactionDeny();
            return;
            }
        }
    
    
    
    
    
    // now walk through edit list, totalling cost, and making sure
    // that changes result in new, submitted house map

    $oldHouseArray = preg_split( "/#/", $old_house_map );

    $editArray = preg_split( "/#/", $edit_list );
    
    $editedHouseArray = $oldHouseArray;

    $numEdits = count( $editArray );


    if( $edit_list == "" ) {
        // split on empty string returns array with 1 element, which screws
        // up loop below
        $numEdits = 0;
        }
    

    for( $i=0; $i<$numEdits; $i++ ) {

        // object_id@index
        $editParts = preg_split( "/@/", $editArray[$i] );

        if( count( $editParts ) != 2 ) {
            cd_log( "House check-in with badly-formatted edit list denied" );
            cd_transactionDeny();
            return;
            }

        $id = $editParts[0];
        $mapIndex = $editParts[1];
        
        if( $mapIndex >= 32 * 32 || $mapIndex < 0 ) {
            // out of bounds
            cd_log( "House check-in with out-of-bounds edit denied" );
            cd_transactionDeny();
            return;
            }


        if( ! array_key_exists( "$id", $priceArray ) ) {
            // id's not on the price list can't be placed!
            cd_log( "House check-in with unplaceable object in edit denied" );
            cd_transactionDeny();
            return;
            }
        
        
        $editedHouseArray[ $editParts[1] ] = $id;

        $loot_value -= $priceArray[ "$id" ];

        if( $loot_value < 0 ) {
            // more edits than they could afford
            cd_log( "House check-in with exceeded player budget denied" );
            cd_transactionDeny();
            return;
            }
        }

    // all edits applied


    // auto-reset any object states to 0 that aren't stuck
    // these may be left over from previous robbery (switches that are toggled,
    //  etc)
    // (client does this every time an edit completes)

    for( $i=0; $i<$numHouseCells; $i++ ) {

        $cellObjects = preg_split( "/,/", $editedHouseArray[$i] );

        $numObjects = count( $cellObjects );

        for( $j=0; $j<$numObjects; $j++ ) {

            $objectParts = preg_split( "/:/", $cellObjects[$j] );

            if( count( $objectParts ) > 1 ) {

                // second part is state

                if( strstr( $objectParts[1], "!" ) ) {
                    // stuck, don't change state
                    }
                else {
                    // set state back to 0, which means state is simply left
                    // off (implied ":0")
                    // so drop second part of object, including ":" separator
                    
                    $cellObjects[$j] = $objectParts[0];
                    }
                }
            }

        $editedHouseArray[$i] = implode( ",", $cellObjects );        
        }
    

    
    
    $edited_house_map = implode( "#", $editedHouseArray );


    if( $edited_house_map != $house_map ) {
        // edits + old map don't add up to the map that was submitted
        cd_log( "House check-in with map and edits mismatch denied" );
        
        cd_transactionDeny();
        return;
        }
    

    // Well...
    // if we get here, then we have a valid, edited house map.



    // Compose UNION of current vault and backpack

    
    // NEXT:
    // Check that purchases don't exceed loot value,
    // PLUS subtract them from vault/backpack union

    $purchaseArray = preg_split( "/#/", $purchase_list );

    $numPurchases = count( $purchaseArray );
        
    
    if( $purchase_list == "#" ) {
        $numPurchases = 0;
        }
    
    for( $i=0; $i<$numPurchases; $i++ ) {
        $purchaseParts = preg_split( "/:/", $purchaseArray[$i] );

        if( count( $purchaseParts ) != 2 ) {
            cd_log(
                "House check-in with badly-formatted purchase list denied" );
            cd_transactionDeny();
            return;
            }

        $id = $purchaseParts[0];
        $quantity = $purchaseParts[1];
        

        if( ! array_key_exists( "$id", $priceArray ) ) {
            // id's not on the price list can't be bought!
            cd_log(
                "House check-in with unbuyable object in purchase denied" );
            cd_transactionDeny();
            return;
            }
        
        $loot_value -= $quantity * $priceArray[ "$id" ];

        if( $loot_value < 0 ) {
            // more edits than they could afford
            cd_log( "House check-in with ".
                    "purchases exceeding player budget denied" );
            cd_transactionDeny();
            return;
            }

        }
    

    // Finally, make sure
    // (Vault U Backpack) =  ( (old_Vault U old_Backpack)  + Purchaes )
    
    $ownedUnion = cd_idQuantityUnion( $vault_contents, $backpack_contents );

    $oldOwnedUnion = cd_idQuantityUnion( $old_vault_contents,
                                         $old_backpack_contents );
    $newOwnedUnion = cd_idQuantityUnion( $oldOwnedUnion, $purchase_list );


    if( $ownedUnion != $newOwnedUnion ) {
        cd_log( "House check-in with ".
                " purchases that don't match contents of vault and ".
                " backpack denied" );
        cd_transactionDeny();
        return;
        }

    
    // also, make sure gallery contains no unexpected items

    if( $old_gallery_contents != "#" ) {

        if( $gallery_contents == "#" ) {
            cd_log( "House check-in with ".
                        " unexpected empty gallery denied" );
            cd_transactionDeny();
            return;
            }
        
        $oldGalleryArray = preg_split( "/#/", $old_gallery_contents );
        $newGalleryArray = preg_split( "/#/", $gallery_contents );

        foreach( $newGalleryArray as $item ) {    
            if( ! in_array( $item, $oldGalleryArray ) ) {
                cd_log( "House check-in with ".
                        " extra gallery items denied" );
                cd_transactionDeny();
                return;
                }
            }
        }
    else if( $gallery_contents != "#" ) {
        cd_log( "House check-in with ".
                " extra gallery items denied" );
        cd_transactionDeny();
        return;
        }
    
        
    

    // map and edits okay
    // purchases okay
    // accept it and check house back in with these changes
    
    
    $query = "UPDATE $tableNamePrefix"."houses SET ".
        "edit_checkout = 0, house_map='$house_map', ".
        "vault_contents='$vault_contents', ".
        "backpack_contents='$backpack_contents', ".
        "gallery_contents='$gallery_contents', ".
        "edit_count='$edit_count', loot_value='$loot_value' ".
        "WHERE user_id = $user_id;";
    cd_queryDatabase( $query );

    cd_queryDatabase( "COMMIT;" );
    cd_queryDatabase( "SET AUTOCOMMIT=1" );

    echo "OK";    
    }




function cd_pingHouse() {
    global $tableNamePrefix;

    if( ! cd_verifyTransaction() ) {
        return;
        }

    $user_id = cd_getUserID();

    
    // automatically ignore blocked users and houses not checked out

    $query = "UPDATE $tableNamePrefix"."houses SET ".
        "last_ping_time = CURRENT_TIMESTAMP ".
        "WHERE ( user_id = $user_id OR robbing_user_id = $user_id ) ".
        "AND blocked='0' ".
        "AND ( rob_checkout = 1 OR edit_checkout = 1 );";
    
    $result = cd_queryDatabase( $query );

    
    if( mysql_affected_rows() == 1 ) {
        echo "OK";
        }
    else {
        echo "FAILED";
        }
    }



function cd_listHouses() {
    global $tableNamePrefix;

    if( ! cd_verifyTransaction() ) {
        return;
        }

    $user_id = cd_getUserID();


    $skip = cd_requestFilter( "skip", "/\d+/", 0 );
    
    $limit = cd_requestFilter( "limit", "/\d+/", 20 );
    
    
    
    // automatically ignore blocked users and houses already checked
    // out for robbery and houses that haven't been edited yet and
    // limbo houses for this user

    // join to include last robber name for each result
    // (maps each robbing_user_id to the corresponding character_name
    //  by joining the houses table to itself)
    $tableName = $tableNamePrefix ."houses";
    
    $query = "SELECT houses.user_id, houses.character_name, ".
        "houses.loot_value, houses.rob_attempts, ".
        "robbers.character_name as robber_name, ".
        "robbers.user_id as robber_id ".
        "FROM $tableName as houses ".
        "LEFT JOIN $tableName as robbers ".
        "ON houses.robbing_user_id = robbers.user_id ".
        "WHERE houses.user_id != '$user_id' AND houses.blocked='0' ".
        "AND houses.rob_checkout = 0 AND houses.edit_checkout = 0 ".
        "AND houses.edit_count > 0 ".        
        "AND houses.user_id NOT IN ".
        "( SELECT house_user_id FROM $tableNamePrefix"."limbo_robberies ".
        "  WHERE user_id = $user_id ) ".
        "ORDER BY houses.loot_value DESC ".
        "LIMIT $skip, $limit;";

    $result = cd_queryDatabase( $query );

    $numRows = mysql_numrows( $result );


    for( $i=0; $i<$numRows; $i++ ) {
        $house_user_id = mysql_result( $result, $i, "user_id" );
        $character_name = mysql_result( $result, $i, "character_name" );
        $robber_name = mysql_result( $result, $i, "robber_name" );
        $robber_id = mysql_result( $result, $i, "robber_id" );
        $loot_value = mysql_result( $result, $i, "loot_value" );
        $rob_attempts = mysql_result( $result, $i, "rob_attempts" );

        if( $robber_name == NULL ) {
            $robber_name = "Null_Null_Null";
            }

        if( $robber_id == $user_id ) {
            // hide name from self
            $robber_name = "You";
            }
        
        echo "$house_user_id#$character_name#$robber_name".
            "#$loot_value#$rob_attempts\n";
        }
    echo "OK";
    }



function cd_startRobHouse() {
    global $tableNamePrefix;

    if( ! cd_verifyTransaction() ) {
        return;
        }

    $user_id = cd_getUserID();

    $to_rob_user_id = cd_requestFilter( "to_rob_user_id", "/\d+/" );

    
    cd_queryDatabase( "SET AUTOCOMMIT=0" );

    cd_processStaleRobberies( $user_id );
    
    // get user's backpack contents
    $query = "SELECT backpack_contents ".
        "FROM $tableNamePrefix"."houses ".
        "WHERE user_id = '$user_id';";

    $result = cd_queryDatabase( $query );

    $backpack_contents = mysql_result( $result, 0, "backpack_contents" );

    
    
    // automatically ignore blocked users and houses already checked
    // out for robbery and limbo houses for this user
    
    $query = "SELECT house_map, gallery_contents, ".
        "character_name, rob_attempts ".
        "FROM $tableNamePrefix"."houses ".
        "WHERE user_id = '$to_rob_user_id' AND blocked='0' ".
        "AND edit_checkout = 0 AND rob_checkout = 0 ".
        "AND user_id NOT IN ".
        "( SELECT house_user_id FROM $tableNamePrefix"."limbo_robberies ".
        "  WHERE user_id = $user_id ) ".
        "FOR UPDATE;";

    $result = cd_queryDatabase( $query );

    $numRows = mysql_numrows( $result );
    
    if( $numRows < 1 ) {
        cd_transactionDeny();
        return;
        }
    $row = mysql_fetch_array( $result, MYSQL_ASSOC );

    $house_map = $row[ "house_map" ];
    $gallery_contents = $row[ "gallery_contents" ];
    $character_name = $row[ "character_name" ];
    $rob_attempts = $row[ "rob_attempts" ];
    $rob_attempts ++;
    
    $query = "UPDATE $tableNamePrefix"."houses SET ".
        "rob_checkout = 1, robbing_user_id = '$user_id', ".
        "rob_attempts = '$rob_attempts', last_ping_time = CURRENT_TIMESTAMP ".
        "WHERE user_id = $to_rob_user_id;";
    cd_queryDatabase( $query );

    cd_queryDatabase( "COMMIT;" );
    cd_queryDatabase( "SET AUTOCOMMIT=1" );

    echo "$character_name\n";
    echo "$house_map\n";
    echo "$backpack_contents\n";
    echo "$gallery_contents\n";
    echo "OK";
    }



function cd_endRobHouse() {
    global $tableNamePrefix;

    if( ! cd_verifyTransaction() ) {
        return;
        }

    $user_id = cd_getUserID();

    $success = cd_requestFilter( "success", "/[012]/" );

    $backpack_contents = cd_requestFilter( "backpack_contents", "/[#0-9:]+/" );

    
    cd_queryDatabase( "SET AUTOCOMMIT=0" );



    
    $query = "SELECT backpack_contents, vault_contents ".
        "FROM $tableNamePrefix"."houses ".
        "WHERE user_id = '$user_id' AND blocked='0' FOR UPDATE;";

    $result = cd_queryDatabase( $query );

    $numRows = mysql_numrows( $result );

    if( $numRows < 1 ) {
        cd_transactionDeny();
        return;
        }
    
    $old_backpack_contents = mysql_result( $result, 0, "backpack_contents" );
    $old_robber_vault_contents = mysql_result( $result, 0, "vault_contents" );



    $move_list = cd_requestFilter( "move_list", "/[mt0-9@#]+/" );
    

    // make sure tools used in move_list agrees with change to backpack
    // contents
    $toolsUsedString = "#";

    if( $move_list != "" ) {
        

    
        $toolsUsedArray = array();

        $moves = $pairArray = preg_split( "/#/", $move_list );

        foreach( $moves as $move ) {
            if( $move[0] == 't' ) {
                // tool use

                $parts = preg_split( "/@/", substr( $move, 1 ) );

                if( count( $parts ) != 2 ) {
                    cd_log( "Robbery end with bad move list ".
                            "($move_list) denied" );
                    cd_transactionDeny();
                    return;
                    }
                $tool_id = $parts[0];

                if( array_key_exists( $tool_id, $toolsUsedArray ) ) {
                    $toolsUsedArray[ $tool_id ] ++;
                    }
                else {
                    $toolsUsedArray[ $tool_id ] = 1;
                    }
                }
        
            }

        $toolsUsedString = cd_idQuanityArrayToString( $toolsUsedArray );
        }

    
    $totalBackpack =
        cd_idQuantityUnion( $backpack_contents, $toolsUsedString );

    // make sure $old_backpack_contents is sorted the same way before
    // doing the compare
    $totalBackpackShouldBe =
        cd_idQuantityUnion( $old_backpack_contents, "#" );
    
    
    if( $totalBackpack != $totalBackpackShouldBe ) {
        cd_log( "Robbery end with tools used not adding up with remaining ".
                "backpack contents denied" );
        cd_transactionDeny();
        return;
        }
    
        
    
    
    // automatically ignore blocked users and houses already checked
    // out for robbery and limbo houses for this user
    
    $query = "SELECT loot_value, house_map, user_id, character_name, ".
        "loot_value, vault_contents, gallery_contents, ".
        "rob_attempts, edit_count ".
        "FROM $tableNamePrefix"."houses ".
        "WHERE robbing_user_id = '$user_id' AND blocked='0' ".
        "AND rob_checkout = 1 AND edit_checkout = 0 ".
        "AND user_id NOT IN ".
        "( SELECT house_user_id FROM $tableNamePrefix"."limbo_robberies ".
        "  WHERE user_id = $user_id ) ".
        "FOR UPDATE;";

    $result = cd_queryDatabase( $query );

    $numRows = mysql_numrows( $result );
    
    if( $numRows < 1 ) {
        cd_transactionDeny();
        return;
        }
    $row = mysql_fetch_array( $result, MYSQL_ASSOC );




    // update contents of backpac (checked to be okay above)
    $query = "UPDATE $tableNamePrefix"."houses SET ".
        "backpack_contents = '$backpack_contents'".
        "WHERE user_id = $user_id;";
    cd_queryDatabase( $query );

    
    
    $house_map = cd_requestFilter( "house_map", "/[#0-9,:!]+/" );

    $house_money = $row[ "loot_value" ];
    $house_vault_contents = $row[ "vault_contents" ];
    $gallery_contents = $row[ "gallery_contents" ];
    
    $amountTaken = $house_money;
    $stuffTaken = $house_vault_contents;
    $galleryStuffTaken = $gallery_contents;

    
    $old_house_map = $row[ "house_map" ];
    $victim_id = $row[ "user_id" ];
    $victim_name = $row[ "character_name" ];
    $loot_value = $row[ "loot_value" ];
    $rob_attempts = $row[ "rob_attempts" ];
    $edit_count = $row[ "edit_count" ];
    
    
    if( $success == 0 || $success == 2 ) {
        // keep original house map, untouched
        $house_map = $old_house_map;

        // don't touch loot value
        // or vault
        $amountTaken = 0;
        $stuffTaken = "#";
        $galleryStuffTaken = "#";
        
        if( $success == 0 ) {
            // robber dies

            // drops backpack in this house's vault
            $house_vault_contents = cd_idQuantityUnion( $house_vault_contents,
                                                        $backpack_contents );
            
            // starts over as new character, house destroyed
            cd_newHouseForUser( $user_id );
            }
        }
    else {
        // reached vault, successful robbery
        
        // use new house map

        // successful robbery, has not been edited since
        $edit_count = 0;
        
        // also transfer all money and vault stuff from victim to robber

        $new_robber_vault_contents =
            cd_idQuantityUnion( $old_robber_vault_contents, $stuffTaken );
        
        
        $query = "UPDATE $tableNamePrefix"."houses SET ".
            "loot_value = loot_value + $house_money, ".
            "vault_contents = '$new_robber_vault_contents' ".
            "WHERE user_id = $user_id;";
        cd_queryDatabase( $query );

        // all loot taken
        $house_money = 0;
        $house_vault_contents = "#";

        // log robbery
        $robber_name = cd_getCharacterName( $user_id );

        $loadout = $old_backpack_contents;

        

        /*
        $query =
            "CREATE TABLE $tableName(" .
            "log_id INT NOT NULL PRIMARY KEY AUTO_INCREMENT," .
            "user_id INT NOT NULL," .
            "house_user_id INT NOT NULL," .
            "loot_value INT NOT NULL," .
            "rob_attempts INT NOT NULL,".
            "robber_name VARCHAR(62) NOT NULL," .
            "victim_name VARCHAR(62) NOT NULL," .
            "rob_time DATETIME NOT NULL,".
            "house_start_map LONGTEXT NOT NULL," .
            "loadout LONGTEXT NOT NULL," .
            "move_list LONGTEXT NOT NULL," .
            "house_end_map LONGTEXT NOT NULL ) ENGINE = INNODB;";
        */

        // log_id auto-assigned
        $query =
            "INSERT INTO $tableNamePrefix"."robbery_logs ".
            "(user_id, house_user_id, loot_value, rob_attempts, ".
            " robber_name, victim_name, rob_time, ".
            " house_start_map, loadout, move_list, house_end_map ) ".
            "VALUES(" .
            " $user_id, $victim_id, '$loot_value', '$rob_attempts', ".
            " '$robber_name', '$victim_name', ".
            " CURRENT_TIMESTAMP, '$old_house_map', '$loadout', '$move_list', ".
            " '$house_map' );";
        cd_queryDatabase( $query );        
        }
    
        
    
    $query = "UPDATE $tableNamePrefix"."houses SET ".
        "rob_checkout = 0, edit_count = '$edit_count', ".
        "house_map='$house_map', ".
        "loot_value = $house_money,  ".
        "vault_contents = '$house_vault_contents' ".
        "WHERE robbing_user_id = $user_id AND rob_checkout = 1;";
    cd_queryDatabase( $query );

    cd_queryDatabase( "COMMIT;" );
    cd_queryDatabase( "SET AUTOCOMMIT=1" );

    echo "$amountTaken\n";
    echo "$stuffTaken\n";
    echo "$galleryStuffTaken\n";
    echo "OK";
    }




function cd_listLoggedRobberies() {
    global $tableNamePrefix;

    if( ! cd_verifyTransaction() ) {
        return;
        }

    $user_id = cd_getUserID();


    $skip = cd_requestFilter( "skip", "/\d+/", 0 );
    
    $limit = cd_requestFilter( "limit", "/\d+/", 20 );
    

    /*
        $query =
            "CREATE TABLE $tableName(" .
            "log_id INT NOT NULL PRIMARY KEY AUTO_INCREMENT," .
            "user_id INT NOT NULL," .
            "house_user_id INT NOT NULL," .
            "loot_value INT NOT NULL," .
            "rob_attempts INT NOT NULL,".
            "robber_name VARCHAR(62) NOT NULL," .
            "victim_name VARCHAR(62) NOT NULL," .
            "rob_time DATETIME NOT NULL,".
            "house_start_map LONGTEXT NOT NULL," .
            "loadout LONGTEXT NOT NULL," .
            "move_list LONGTEXT NOT NULL," .
            "house_end_map LONGTEXT NOT NULL ) ENGINE = INNODB;";
        */
    
    $tableName = $tableNamePrefix ."robbery_logs";
    
    $query = "SELECT user_id, house_user_id, ".
        "log_id, victim_name, robber_name, ".
        "loot_value, rob_attempts ".
        "FROM $tableName ".
        "ORDER BY rob_time DESC ".
        "LIMIT $skip, $limit;";

    $result = cd_queryDatabase( $query );

    $numRows = mysql_numrows( $result );


    for( $i=0; $i<$numRows; $i++ ) {
        $robber_id = mysql_result( $result, $i, "user_id" );
        $victim_id = mysql_result( $result, $i, "house_user_id" );

        $log_id = mysql_result( $result, $i, "log_id" );
        $victim_name = mysql_result( $result, $i, "victim_name" );
        $robber_name = mysql_result( $result, $i, "robber_name" );
        $loot_value = mysql_result( $result, $i, "loot_value" );
        $rob_attempts = mysql_result( $result, $i, "rob_attempts" );

        if( $robber_id == $user_id ) {
            $robber_name = "You";
            }
        if( $victim_id == $user_id ) {
            $victim_name = "You";
            }
        
        
        echo "$log_id#$victim_name#$robber_name".
            "#$loot_value#$rob_attempts\n";
        }
    echo "OK";
    }



function cd_getRobberyLog() {
    global $tableNamePrefix;

    if( ! cd_verifyTransaction() ) {
        return;
        }


    $user_id = cd_getUserID();


    $log_id = cd_requestFilter( "log_id", "/\d+/" );
    
    
    
    $query = "SELECT user_id, house_user_id, ".
        "robber_name, victim_name, house_start_map, loadout, ".
        "move_list, loot_value ".
        "FROM $tableNamePrefix"."robbery_logs ".
        "WHERE log_id = '$log_id';";

    $result = cd_queryDatabase( $query );

    $numRows = mysql_numrows( $result );
    
    if( $numRows < 1 ) {
        cd_transactionDeny();
        return;
        }
    $row = mysql_fetch_array( $result, MYSQL_ASSOC );


    $robber_name = $row[ "robber_name" ];
    $victim_name = $row[ "victim_name" ];

    if( $user_id == $row[ "user_id" ] ) {
        $robber_name = "You";
        }

    if( $user_id == $row[ "house_user_id" ] ) {
        $victim_name = "You";
        }
    
    
    
    echo $robber_name . "\n";    
    echo $victim_name . "\n";    
    echo $row[ "house_start_map" ] . "\n";    
    echo $row[ "loadout" ] . "\n";    
    echo $row[ "move_list" ] . "\n";
    echo $row[ "loot_value" ] . "\n";
    echo "OK";
    }




function cd_computeAuctionPrice( $start_price, $elapsed_seconds ) {
    global $auctionPriceDropInterval, $auctionPriceHalfLife;

    $priceDropIntervalSeconds = $auctionPriceDropInterval * 60;
    
    
    $intervalsPerHalfLife = $auctionPriceHalfLife / $auctionPriceDropInterval;

    // want D ^ ($intervalsPerHalfLife) = 0.5;
    // where D is the price shrink factor per interval
    // $D = pow( 0.5, 1 / $intervalsPerHalfLife );

    // but we're just going to raise D ^ (numIntervalsPassed) anyway
    // so might as well just rais 0.5 to the multiplied power instead
    
    $numIntervalsPassed =
        floor( $elapsed_seconds / $priceDropIntervalSeconds );
    
    $price = floor( $start_price *
                    pow( 0.5, $numIntervalsPassed / $intervalsPerHalfLife ) );

    if( $price < 1 ) {
        $price = 1;
        }
    
    return $price;
    }



function cd_listAuctions() {
    global $tableNamePrefix;

    if( ! cd_verifyTransaction() ) {
        return;
        }

    $tableName = $tableNamePrefix ."auction";
    
    $query = "SELECT object_id, start_price, ".
        "TIMESTAMPDIFF( SECOND, start_time, CURRENT_TIMESTAMP ) ".
        "   as elapsed_seconds ".
        "FROM $tableName ".
        "ORDER BY elapsed_seconds DESC;";

    $result = cd_queryDatabase( $query );

    $numRows = mysql_numrows( $result );

    $seconds_until_price_drop = 0;

    global $auctionPriceDropInterval, $auctionPriceHalfLife;

    $priceDropIntervalSeconds = $auctionPriceDropInterval * 60;
    
    if( $numRows > 0 ) {
        $elapsed_seconds = mysql_result( $result, 0, "elapsed_seconds" );

        // subtract whole multiple of 3 minutes
        $seconds_until_price_drop =
            $priceDropIntervalSeconds -
            $elapsed_seconds % $priceDropIntervalSeconds;
        
        }

    echo "$seconds_until_price_drop\n";
    
    for( $i=0; $i<$numRows; $i++ ) {
        $object_id = mysql_result( $result, $i, "object_id" );
        $start_price = mysql_result( $result, $i, "start_price" );
        $elapsed_seconds = mysql_result( $result, $i, "elapsed_seconds" );


        $price = cd_computeAuctionPrice( $start_price, $elapsed_seconds );
                
        echo "$object_id#$price\n";
        }
    echo "OK";
    }



function cd_buyAuction() {
    global $tableNamePrefix;

    if( ! cd_verifyTransaction() ) {
        return;
        }


    $user_id = cd_getUserID();


    $object_id = cd_requestFilter( "object_id", "/\d+/" );
    
    
    
    $auctionTable = $tableNamePrefix ."auction";
    $houseTable = $tableNamePrefix ."houses";

    // make sure it's there, and check it's current price
    $query = "SELECT start_price, ".
        "TIMESTAMPDIFF( SECOND, start_time, CURRENT_TIMESTAMP ) ".
        "   as elapsed_seconds ".
        "FROM $auctionTable WHERE object_id = '$object_id' FOR UPDATE;";

    $result = cd_queryDatabase( $query );

    $numRows = mysql_numrows( $result );

    if( $numRows < 1 ) {
        cd_transactionDeny();
        return;
        }

    $start_price = mysql_result( $result, 0, "start_price" );
    $elapsed_seconds = mysql_result( $result, 0, "elapsed_seconds" );
    
    $price = cd_computeAuctionPrice( $start_price, $elapsed_seconds );


    // make sure user has enough balance in house, and house checked out

    $query = "SELECT gallery_contents, loot_value ".
        "FROM $houseTable ".
        "WHERE user_id = '$user_id' AND blocked='0' ".
        "AND edit_checkout = 1 FOR UPDATE;";

    $result = cd_queryDatabase( $query );

    $numRows = mysql_numrows( $result );
    
    if( $numRows < 1 ) {
        cd_transactionDeny();
        return;
        }

    $old_balance = mysql_result( $result, 0, "loot_value" );

    if( $old_balance < $price ) {
        cd_transactionDeny();
        return;
        }

    // user has enough money!
    $new_balance = $old_balance - $price;

    $old_gallery_contents = mysql_result( $result, 0, "gallery_contents" );
    $new_gallery_contents = "";
    
    if( $old_gallery_contents == "#" ) {
        // empty
        $new_gallery_contents = "$object_id";
        }
    else {
        // prepend
        $new_gallery_contents = "$object_id#". $old_gallery_contents;
        }


    
    // now make changes

    $query = "UPDATE $houseTable SET ".
        "loot_value = '$new_balance', ".
        "gallery_contents = '$new_gallery_contents' ".
        "WHERE user_id = '$user_id';";

    $result = cd_queryDatabase( $query );


    $query = "DELETE FROM $auctionTable WHERE object_id = '$object_id';";
    
    $result = cd_queryDatabase( $query );


    cd_queryDatabase( "COMMIT;" );
    cd_queryDatabase( "SET AUTOCOMMIT=1" );

    echo "$price\n";
    echo "OK";
    }
















// utility function for stuff common to all denied user transactions
function cd_transactionDeny() {
    echo "DENIED";
    
    cd_queryDatabase( "COMMIT;" );
    cd_queryDatabase( "SET AUTOCOMMIT=1" );
    }



function cd_getCharacterName( $user_id ) {
    global $tableNamePrefix;
    
    $result = cd_queryDatabase( "SELECT character_name ".
                                "FROM $tableNamePrefix"."houses " .
                                "WHERE user_id = $user_id;" );

    $numRows = mysql_numrows( $result );

    
    if( $numRows < 1 ) {
        cd_fatalError( "Failed to fetch character name for user $user_id" );
        }

    return mysql_result( $result, 0, 0 );
    }



function cd_getUserID() {
    return cd_requestFilter( "user_id", "/\d+/" );
    }



// checks the ticket hash for the user ID and sequence number
// attached to a transaction (also makes sure user isn't blocked!)
function cd_verifyTransaction() {
    global $tableNamePrefix;
    
    $user_id = cd_getUserID();

    $sequence_number = cd_requestFilter( "sequence_number", "/\d+/" );

    $ticket_hash = cd_requestFilter( "ticket_hash", "/[A-F0-9]+/i" );
    

    cd_queryDatabase( "SET AUTOCOMMIT=0" );

    // automatically ignore blocked users
    
    $query = "SELECT sequence_number, ticket_id ".
        "FROM $tableNamePrefix"."users ".
        "WHERE user_id = '$user_id' AND blocked='0' FOR UPDATE;";

    $result = cd_queryDatabase( $query );


    $numRows = mysql_numrows( $result );

    
    if( $numRows < 1 ) {
        cd_transactionDeny();
        cd_log( "Transaction denied for user_id $user_id, ".
                "not found or blocked" );
        return 0;
        }
    
    $row = mysql_fetch_array( $result, MYSQL_ASSOC );

    $last_sequence_number = $row[ "sequence_number" ];

    if( $sequence_number < $last_sequence_number ) {
        cd_transactionDeny();
        cd_log( "Transaction denied for user_id $user_id, ".
                "stale sequence number" );
        return 0;
        }
    
    
    
    $ticket_id = $row[ "ticket_id" ];


    $correct_ticket_hash = sha1( $ticket_id . "$sequence_number" );


    if( strtoupper( $correct_ticket_hash ) !=
        strtoupper( $ticket_hash ) ) {
        cd_transactionDeny();
        cd_log( "Transaction denied for user_id $user_id, ".
                "hash check failed" );

        return 0;
        }

    // sig passed, sequence number not a replay!

    // update the sequence number, which we have locked

    $new_number = $sequence_number + 1;
    
    $query = "UPDATE $tableNamePrefix"."users SET ".
        "sequence_number = $new_number ".
        "WHERE user_id = $user_id;";
    cd_queryDatabase( $query );

    cd_queryDatabase( "COMMIT;" );
    cd_queryDatabase( "SET AUTOCOMMIT=1" );

    return 1;
    }





function cd_newHouseForUser( $user_id ) {
    global $tableNamePrefix;
    
    
    // create default house for user


    cd_queryDatabase( "SET AUTOCOMMIT = 0;" );

    // select first, for update, so we can safely delete old house
    // if there is one
    // NOTE that if house doesn't exist, this select will NOT block the
    // row gap.  In the case of concurrent inserts for the same user_id,
    // the second insert will fail (user_id is the primary key)
    
    $query = "select user_id from $tableNamePrefix"."houses ".
        "WHERE user_id = $user_id;";

    $result = cd_queryDatabase( $query );

    $numRows = mysql_numrows( $result );

    if( $numRows > 0 ) {
        $query = "delete from $tableNamePrefix"."houses ".
            "WHERE user_id = $user_id;";
        cd_queryDatabase( $query );
        }
    
    
    // Generate a unique name here
    // Error number generated when a forced-unique key already exists upon
    // insertion
    // Best way to ensure that character names are unique, and keep
    // searching for a unique one after a collision.
    $errorNumber = 1062;    
    $foundName = false;
    
    
    
    // default house map, 32x32 map
    // impenetrable walls around exterior
    // goal in place
    // default state for each cell (no ":state" part)
    $house_map =
        "998#998#998#998#998#998#998#998#998#998#998#998#998#998#998#998#998#998#998#998#998#998#998#998#998#998#998#998#998#998#998#998#".
        "998#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#998#".
        "998#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#998#".
        "998#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#998#".

        "998#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#998#".
        "998#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#998#".
        "998#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#998#".
        "998#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#998#".

        "998#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#998#".
        "998#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#998#".
        "998#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#998#".
        "998#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#998#".

        "998#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#998#".
        "998#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#998#".
        "998#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#998#".
      "998#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#998#".

        
        "0#0#0#0#0#0#0#0#0#999#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#998#".
        "998#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#998#".
        "998#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#998#".
        "998#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#998#".

        "998#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#998#".
        "998#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#998#".
        "998#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#998#".
        "998#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#998#".

        "998#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#998#".
        "998#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#998#".
        "998#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#998#".
        "998#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#998#".

        "998#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#998#".
        "998#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#998#".
        "998#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#0#998#".
        "998#998#998#998#998#998#998#998#998#998#998#998#998#998#998#998#998#998#998#998#998#998#998#998#998#998#998#998#998#998#998#998";
    
    $vault_contents = "#";
    $backpack_contents = "#";
    $gallery_contnets = '#';
    
    while( !$foundName && $errorNumber == 1062 ) {
        $character_name = cd_pickFullName();
        
        
        $query = "INSERT INTO $tableNamePrefix"."houses VALUES(" .
            " $user_id, '$character_name', '$house_map', ".
            "'$vault_contents', '$backpack_contents', '$gallery_contnets', ".
            "0, 1000, 0, 0, 0, 0, ".
            "CURRENT_TIMESTAMP, 0 );";

        // bypass our default error handling here so that
        // we can react to duplicate errors
        $result = mysql_query( $query );

        if( $result ) {
            $foundName = true;
            }
        else {
            $errorNumber = mysql_errno();

            cd_log( "Name collision for $character_name?  ".
                    "Error: $errorNumber" );

            if( $errorNumber != 1062 ) {
                cd_fatalError(
                    "Database query failed:<BR>$inQueryString<BR><BR>" .
                    mysql_error() );
                }
            }
        }
    
    cd_queryDatabase( "COMMIT;" );
    
    
    cd_queryDatabase( "SET AUTOCOMMIT = 1;" );
    }




function cd_logout() {

    cd_clearPasswordCookie();

    echo "Logged out";
    }







function cd_showData() {

    global $tableNamePrefix, $remoteIP;

    // these are global so they work in embeded function call below
    global $skip, $search, $order_by;

    cd_checkPassword( "show_data" );
    

    echo "<table width='100%' border=0><tr>".
        "<td>[<a href=\"server.php?action=show_data" .
            "\">Main</a>]</td>".
        "<td align=right>[<a href=\"server.php?action=logout" .
            "\">Logout</a>]</td>".
        "</tr></table><br><br><br>";




    $skip = cd_requestFilter( "skip", "/\d+/", 0 );

    global $housesPerPage;
    
    $search = cd_requestFilter( "search", "/[A-Z0-9_@. -]+/i" );

    $order_by = cd_requestFilter( "order_by", "/[A-Z_]+/i", "last_ping_time" );
    

    $keywordClause = "";
    $searchDisplay = "";
    
    if( $search != "" ) {
        

        $keywordClause = "WHERE ( user_id LIKE '%$search%' " .
            "OR character_name LIKE '%$search%' ".
            "OR loot_value LIKE '%$search%' ) ";

        $searchDisplay = " matching <b>$search</b>";
        }
    

    

    // first, count results
    $query = "SELECT COUNT(*) FROM $tableNamePrefix"."houses $keywordClause;";

    $result = cd_queryDatabase( $query );
    $totalHouses = mysql_result( $result, 0, 0 );

    
             
    $query = "SELECT user_id, character_name, loot_value, edit_checkout, ".
        "rob_checkout, robbing_user_id, rob_attempts, last_ping_time, ".
        "blocked ".
        "FROM $tableNamePrefix"."houses $keywordClause".
        "ORDER BY $order_by DESC ".
        "LIMIT $skip, $housesPerPage;";
    $result = cd_queryDatabase( $query );
    
    $numRows = mysql_numrows( $result );

    $startSkip = $skip + 1;
    
    $endSkip = $startSkip + $housesPerPage - 1;

    if( $endSkip > $totalHouses ) {
        $endSkip = $totalHouses;
        }



        // form for searching houses
?>
        <hr>
            <FORM ACTION="server.php" METHOD="post">
    <INPUT TYPE="hidden" NAME="action" VALUE="show_data">
    <INPUT TYPE="hidden" NAME="order_by" VALUE="<?php echo $order_by;?>">
    <INPUT TYPE="text" MAXLENGTH=40 SIZE=20 NAME="search"
             VALUE="<?php echo $search;?>">
    <INPUT TYPE="Submit" VALUE="Search">
    </FORM>
        <hr>
<?php

    

    
    echo "$totalHouses active houses". $searchDisplay .
        " (showing $startSkip - $endSkip):<br>\n";

    
    $nextSkip = $skip + $housesPerPage;

    $prevSkip = $skip - $housesPerPage;
    
    if( $prevSkip >= 0 ) {
        echo "[<a href=\"server.php?action=show_data" .
            "&skip=$prevSkip&search=$search&order_by=$order_by\">".
            "Previous Page</a>] ";
        }
    if( $nextSkip < $totalHouses ) {
        echo "[<a href=\"server.php?action=show_data" .
            "&skip=$nextSkip&search=$search&order_by=$order_by\">".
            "Next Page</a>]";
        }

    echo "<br><br>";
    
    echo "<table border=1 cellpadding=5>\n";


    function orderLink( $inOrderBy, $inLinkText ) {
        global $skip, $search, $order_by;
        if( $inOrderBy == $order_by ) {
            // already displaying this order, don't show link
            return "<b>$inLinkText</b>";
            }

        // else show a link to switch to this order
        return "<a href=\"server.php?action=show_data" .
            "&search=$search&skip=$skip&order_by=$inOrderBy\">$inLinkText</a>";
        }
    
    echo "<tr>\n";
    echo "<td>".orderLink( "user_id", "User ID" )."</td>\n";
    echo "<td>Blocked?</td>\n";
    echo "<td>".orderLink( "character_name", "Character Name" )."</td>\n";
    echo "<td>".orderLink( "loot_value", "Loot Value" )."</td>\n";
    echo "<td>".orderLink( "rob_attempts", "Rob Attempts" )."</td>\n";
    echo "<td>Checkout?</td>\n";
    echo "<td>Robbing User</td>\n";
    echo "<td>".orderLink( "last_ping_time", "PingTime" )."</td>\n";

    echo "</tr>\n";
    

    for( $i=0; $i<$numRows; $i++ ) {
        $user_id = mysql_result( $result, $i, "user_id" );
        $character_name = mysql_result( $result, $i, "character_name" );
        $loot_value = mysql_result( $result, $i, "loot_value" );
        $edit_checkout = mysql_result( $result, $i, "edit_checkout" );
        $rob_checkout = mysql_result( $result, $i, "rob_checkout" );
        $robbing_user_id = mysql_result( $result, $i, "robbing_user_id" );
        $rob_attempts = mysql_result( $result, $i, "rob_attempts" );
        $last_ping_time = mysql_result( $result, $i, "last_ping_time" );
        $blocked = mysql_result( $result, $i, "blocked" );
        
        $block_toggle = "";
        
        if( $blocked ) {
            $blocked = "BLOCKED";
            $block_toggle = "<a href=\"server.php?action=block_user_id&".
                "blocked=0&user_id=$user_id".
                "&search=$search&skip=$skip&order_by=$order_by\">unblock</a>";
            
            }
        else {
            $blocked = "";
            $block_toggle = "<a href=\"server.php?action=block_user_id&".
                "blocked=1&user_id=$user_id".
                "&search=$search&skip=$skip&order_by=$order_by\">block</a>";
            
            }

        $checkout = " ";

        if( $edit_checkout ) {
            $checkout = "edit";
            }
        if( $rob_checkout ) {
            $checkout = "rob";
            }
        if( $edit_checkout && $rob_checkout ) {
            $checkout = "BOTH!";
            }
        

        
        echo "<tr>\n";
        
        echo "<td><b>$user_id</b> ";
        echo "[<a href=\"server.php?action=show_detail" .
            "&user_id=$user_id\">detail</a>]</td>\n";
        echo "<td align=right>$blocked [$block_toggle]</td>\n";
        echo "<td>$character_name</td>\n";
        echo "<td>$loot_value</td>\n";
        echo "<td>$rob_attempts</td>\n";
        echo "<td>$checkout</td>\n";
        echo "<td>$robbing_user_id</td>\n";
        echo "<td>$last_ping_time</td>\n";
        echo "</tr>\n";
        }
    echo "</table>";


    echo "<hr>";


    $query = "SELECT object_id, price, in_gallery, note ".
        "FROM $tableNamePrefix"."prices;";
    $result = cd_queryDatabase( $query );
    
    $numRows = mysql_numrows( $result );

?>
    <br>
    <a name="priceList">      
    <FORM ACTION="server.php#priceList" METHOD="post">
    <INPUT TYPE="Submit" VALUE="Update Prices">
    <INPUT TYPE="hidden" NAME="action" VALUE="update_prices">
    <INPUT TYPE="hidden" NAME="num_prices" VALUE="<?php echo $numRows;?>">
<?php

    echo "<table border=1>\n";

    $bgColor = "#EEEEEE";
    $altBGColor = "#CCCCCC";
                 
    for( $i=0; $i<$numRows; $i++ ) {
        $object_id = mysql_result( $result, $i, "object_id" );
        $price = mysql_result( $result, $i, "price" );
        $in_gallery = mysql_result( $result, $i, "in_gallery" );
        $note = mysql_result( $result, $i, "note" );

        $note = htmlspecialchars( $note, ENT_QUOTES );

        $checked = "";

        if( $in_gallery ) {
            $checked = "CHECKED";
            }
        
        echo "<tr>\n";
        echo "<td bgcolor=$bgColor>".
            "Object ID: <b>$object_id</b>".
            "<INPUT TYPE='hidden' NAME='id_$i' VALUE='$object_id'></td>\n";
        echo "<td bgcolor=$bgColor>Price: $<INPUT TYPE='text' ".
                          "MAXLENGTH=40 SIZE=20 NAME='price_$i' ".
                          "VALUE='$price'></td>\n";
        echo "<td bgcolor=$bgColor>Gallery: <INPUT TYPE='checkbox' ".
                          "NAME='in_gallery_$i' VALUE='1' $checked></td>\n";
        echo "<td bgcolor=$bgColor>Note: <INPUT TYPE='text' ".
                          "MAXLENGTH=255 SIZE=30 NAME='note_$i' ".
                          "VALUE='$note'></td>\n";
        echo "<td bgcolor=$bgColor>[<a href='server.php?action=delete_price".
                           "&object_id=$object_id".
                           "#priceList'>delete]</td>\n";
        echo "</tr>\n\n";

        $temp = $bgColor;
        $bgColor = $altBGColor;
        $altBGColor = $temp;
        }
    
    echo "<tr>\n";
    echo "<td colspan=5>New Price:</td><tr>\n";
    echo "<tr>\n";
    echo "<td>Object ID: <INPUT TYPE='text' MAXLENGTH=40 SIZE=20 NAME='id_NEW'
             VALUE=''></td>\n";
    echo "<td>Price: $<INPUT TYPE='text' ".
        "MAXLENGTH=40 SIZE=20 NAME='price_NEW' ".
        "VALUE=''></td>\n";
    echo "<td>Gallery: <INPUT TYPE='checkbox' ".
        "NAME='in_gallery_NEW' VALUE='1'></td>\n";
    echo "<td>Note: <INPUT TYPE='text' ".
        "MAXLENGTH=255 SIZE=30 NAME='note_NEW' ".
        "VALUE=''></td>\n";
    echo "<td></td>\n";
    echo "</tr>\n\n";
    
    echo "</table>\n";
?>    
    <INPUT TYPE="Submit" VALUE="Update Prices">
    </FORM>
    <br>
    <br>

         
    <FORM ACTION="server.php#priceList" METHOD="post">
    <INPUT TYPE="hidden" NAME="action" VALUE="default_prices">
    <table border=1>
    <tr>
    <td><INPUT TYPE="Submit" VALUE="Restore Default Prices"></td>
    <td><INPUT TYPE="checkbox" NAME="confirm1" VALUE=1> Sure?<br>
    <INPUT TYPE="checkbox" NAME="confirm2" VALUE=1> Really sure?<td>
    </tr>
    </table>     
    </FORM>
         
<?php

         
    echo "<hr>";

    // Show auction list
    echo "Auction:<br>";
    
    $query = "SELECT object_id, start_price, start_time, ".
        "TIMESTAMPDIFF( SECOND, start_time, CURRENT_TIMESTAMP ) ".
        "   as elapsed_seconds ".
        "FROM $tableNamePrefix"."auction;";
    $result = cd_queryDatabase( $query );
    
    $numRows = mysql_numrows( $result );

    echo "<table border=1 cellpadding=5>\n";

    $bgColor = "#EEEEEE";
    $altBGColor = "#CCCCCC";
                 
    for( $i=0; $i<$numRows; $i++ ) {
        $object_id = mysql_result( $result, $i, "object_id" );
        $start_price = mysql_result( $result, $i, "start_price" );
        $start_time = mysql_result( $result, $i, "start_time" );
        $elapsed_seconds = mysql_result( $result, $i, "elapsed_seconds" );

        $price = cd_computeAuctionPrice( $start_price, $elapsed_seconds );
        
        echo "<tr>\n";
        echo "<td bgcolor=$bgColor>".
            "Object ID: <b>$object_id</b></td>\n";
        echo "<td bgcolor=$bgColor>".
            "Start Price: <b>\$$start_price</b></td>\n";
        echo "<td bgcolor=$bgColor>".
            "Start Time: <b>$start_time</b></td>\n";
        echo "<td bgcolor=$bgColor>".
            "Elapsed Seconds: <b>$elapsed_seconds</b></td>\n";
        echo "<td bgcolor=$bgColor>".
            "Current Price: <b>\$$price</b></td>\n";
        echo "</tr>\n\n";

        $temp = $bgColor;
        $bgColor = $altBGColor;
        $altBGColor = $temp;
        }
    echo "</table>\n";
    
    
    echo "<hr>";

    echo "<a href=\"server.php?action=show_log\">".
        "Show log</a>";
    echo "<hr>";
    echo "Generated for $remoteIP\n";

    /*
    echo "Name Test:<br>";

    for( $i=0; $i<100; $i++ ) {
        $character_name = cd_pickFullName();
        echo "$character_name<br>\n";
        }
    */
    }



function cd_showDetail() {
    cd_checkPassword( "show_detail" );

    $user_id = cd_getUserID();
    
    echo "[<a href=\"server.php?action=show_data" .
        "\">Main</a>]<br><br><br>";
     
    global $tableNamePrefix, $ticketServerNamePrefix;

    $query = "SELECT ticket_id, blocked FROM $tableNamePrefix"."users ".
        "WHERE user_id = '$user_id';";

    $result = cd_queryDatabase( $query );
    
    $numRows = mysql_numrows( $result );

    if( $numRows < 1 ) {
        cd_operationError( "User ID $user_id not found" );
        }
    $row = mysql_fetch_array( $result, MYSQL_ASSOC );

    $ticket_id = $row[ "ticket_id" ];
    $blocked = $row[ "blocked" ];


    $query = "SELECT email FROM $ticketServerNamePrefix"."tickets ".
        "WHERE ticket_id = '$ticket_id';";

    $result = cd_queryDatabase( $query );
    
    $numRows = mysql_numrows( $result );

    if( $numRows < 1 ) {
        cd_operationError( "Ticket ID $ticket_id not found" );
        }
    $row = mysql_fetch_array( $result, MYSQL_ASSOC );

    $email = $row[ "email" ];


    echo "User ID: $user_id<br>\n";
    echo "Ticket: $ticket_id<br>\n";
    echo "Email: $email<br>\n";
    }




function cd_blockUserID() {
    cd_checkPassword( "block_user_id" );


    global $tableNamePrefix;
        
    $user_id = cd_getUserID();

    $blocked = cd_requestFilter( "blocked", "/[01]/" );

    
    global $remoteIP;

    

    
    $query = "SELECT user_id FROM $tableNamePrefix"."users ".
        "WHERE user_id = '$user_id';";
    $result = cd_queryDatabase( $query );

    $numRows = mysql_numrows( $result );

    if( $numRows == 1 ) {

        
        $query = "UPDATE $tableNamePrefix"."users SET " .
            "blocked = '$blocked' " .
            "WHERE user_id = '$user_id';";
        
        $result = cd_queryDatabase( $query );

                
        $query = "UPDATE $tableNamePrefix"."houses SET " .
            "blocked = '$blocked' " .
            "WHERE user_id = '$user_id';";
        
        $result = cd_queryDatabase( $query );

        
        cd_log( "$user_id block changed to $blocked by $remoteIP" );

        cd_showData();
        }
    else {
        cd_log( "$user_id not found for $remoteIP" );

        echo "$user_id not found";
        }    
    }




function cd_defaultPrices() {
    cd_checkPassword( "default_prices" );


    global $tableNamePrefix;
    global $remoteIP;

    $confirm1 = cd_requestFilter( "confirm1", "/[1]/" );
    $confirm2 = cd_requestFilter( "confirm2", "/[1]/" );
    
    if( $confirm1 == 1 && $confirm2 == 1 ) {

        cd_restoreDefaultPrices();

        cd_log( "Default prices restored by $remoteIP" );
    
        cd_showData();
        }
    else {
        cd_nonFatalError( "Double confirmation boxes not checked to restore ".
                          "default prices" );
        }
    }






function cd_updatePrices() {
    cd_checkPassword( "update_prices" );


    global $tableNamePrefix;
    global $remoteIP;

    
    
    $num_prices = cd_requestFilter( "num_prices", "/\d+/" );

    
    if( $num_prices > 0 ) {
        

        for( $i=0; $i<$num_prices; $i++ ) {
            $id = cd_requestFilter( "id_$i", "/\d+/" );
            $price = cd_requestFilter( "price_$i", "/\d+/" );
            $in_gallery = cd_requestFilter( "in_gallery_$i", "/1/", "0" );
            
            $note = cd_requestFilter( "note_$i", "/[A-Z0-9.' _-]+/i" );
            
            if( $id != "" && $price != "" ) {

                // note may have ' in it
                $note = mysql_real_escape_string( $note );
                
                $query = "UPDATE $tableNamePrefix"."prices SET " .
                    "price = '$price', in_gallery = '$in_gallery', ".
                    "note = '$note' " .
                    "WHERE object_id = '$id';";
                
                $result = cd_queryDatabase( $query );
                }
            }
        
        cd_log( "Prices updated by $remoteIP" );
        }

    // new one to insert?

    $id = cd_requestFilter( "id_NEW", "/\d+/" );

    $price = cd_requestFilter( "price_NEW", "/\d+/" );

    $in_gallery = cd_requestFilter( "in_gallery_NEW", "/1/", "0" );

    $note = cd_requestFilter( "note_NEW", "/[A-Z0-9.' _-]+/i" );

    
    if( $id != "" && $price != "" ) {
        // first, make sure it doesn't already exist
        $query = "SELECT COUNT(object_id) FROM $tableNamePrefix"."prices ".
            "WHERE object_id = '$id';";
        $result = cd_queryDatabase( $query );

        $count = mysql_result( $result, 0, 0 );
        if( $count != 0 ) {

            cd_nonFatalError( "Price already exists for '$id'" );            
            }

        // note may have ' in it
        $note = mysql_real_escape_string( $note );
        
        
        $query = "INSERT INTO $tableNamePrefix"."prices VALUES ( " .
            "'$id', '$price', '$in_gallery', '$note' );";
        $result = cd_queryDatabase( $query );

        if( $result ) {
            $galleryLabel = "";
            if( $in_gallery ) {
                $galleryLabel = "gallery ";
                }
            
            cd_log( "New $galleryLabel"."price ($id, \$$price, '$note' ) ".
                    "created by $remoteIP" );
            }

        if( $in_gallery == 1 ) {
            cd_startAuction( $id, $price );
            }
        
        }
    


    
    cd_showData();
    }




function cd_deletePrice() {
    cd_checkPassword( "delete_price" );


    global $tableNamePrefix;
    global $remoteIP;


    $success = false;
    
    $object_id = cd_requestFilter( "object_id", "/\d+/" );
    if( $object_id != "" ) {

        $query = "DELETE FROM $tableNamePrefix"."prices " .
            "WHERE object_id = '$object_id';";
        
        $result = cd_queryDatabase( $query );

        if( $result && mysql_affected_rows() == 1 ) {
            cd_log( "Price for $object_id deleted by $remoteIP" );
            $success = true;
            }
        }

    if( ! $success ) {
        cd_nonFatalError( "Failed to delete price for '$object_id'" );
        }
    
    cd_showData();
    }









// general-purpose functions down here, many copied from seedBlogs

/**
 * Connects to the database according to the database variables.
 */  
function cd_connectToDatabase() {
    global $databaseServer,
        $databaseUsername, $databasePassword, $databaseName;
    
    
    mysql_connect( $databaseServer, $databaseUsername, $databasePassword )
        or cd_fatalError( "Could not connect to database server: " .
                       mysql_error() );
    
	mysql_select_db( $databaseName )
        or cd_fatalError( "Could not select $databaseName database: " .
                       mysql_error() );
    }


 
/**
 * Closes the database connection.
 */
function cd_closeDatabase() {
    mysql_close();
    }



/**
 * Queries the database, and dies with an error message on failure.
 *
 * @param $inQueryString the SQL query string.
 *
 * @return a result handle that can be passed to other mysql functions.
 */
function cd_queryDatabase( $inQueryString ) {

    $result = mysql_query( $inQueryString )
        or cd_fatalError( "Database query failed:<BR>$inQueryString<BR><BR>" .
                       mysql_error() );

    return $result;
    }



/**
 * Checks whether a table exists in the currently-connected database.
 *
 * @param $inTableName the name of the table to look for.
 *
 * @return 1 if the table exists, or 0 if not.
 */
function cd_doesTableExist( $inTableName ) {
    // check if our table exists
    $tableExists = 0;
    
    $query = "SHOW TABLES";
    $result = cd_queryDatabase( $query );

    $numRows = mysql_numrows( $result );


    for( $i=0; $i<$numRows && ! $tableExists; $i++ ) {

        $tableName = mysql_result( $result, $i, 0 );
        
        if( $tableName == $inTableName ) {
            $tableExists = 1;
            }
        }
    return $tableExists;
    }



function cd_log( $message ) {
    global $enableLog, $tableNamePrefix;

    if( $enableLog ) {
        $user_id = cd_getUserID();
        
        if( $user_id != "" ) {
            $message = "[user_id = $user_id] " . $message;
            }

        $slashedMessage = mysql_real_escape_string( $message );
        
        $query = "INSERT INTO $tableNamePrefix"."log VALUES ( " .
            "'$slashedMessage', CURRENT_TIMESTAMP );";
        $result = cd_queryDatabase( $query );
        }
    }



/**
 * Displays the error page and dies.
 *
 * @param $message the error message to display on the error page.
 */
function cd_fatalError( $message ) {
    //global $errorMessage;

    // set the variable that is displayed inside error.php
    //$errorMessage = $message;
    
    //include_once( "error.php" );

    // for now, just print error message
    $logMessage = "Fatal error:  $message";
    
    echo( $logMessage );

    cd_log( $logMessage );
    
    die();
    }



/**
 * Displays the operation error message and dies.
 *
 * @param $message the error message to display.
 */
function cd_operationError( $message ) {
    
    // for now, just print error message
    echo( "ERROR:  $message" );
    die();
    }



/**
 * Displays the non-fatal error page and dies.
 *
 * @param $message the error message to display on the error page.
 */
function cd_nonFatalError( $message ) {

    cd_checkPassword( "nonFatalError" );
    
     echo "[<a href=\"server.php?action=show_data" .
         "\">Main</a>]<br><br><br>";
    
    // for now, just print error message
    $logMessage = "Error:  $message";
    
    echo( $logMessage );

    cd_log( $logMessage );
    
    die();
    }





/**
 * Recursively applies the addslashes function to arrays of arrays.
 * This effectively forces magic_quote escaping behavior, eliminating
 * a slew of possible database security issues. 
 *
 * @inValue the value or array to addslashes to.
 *
 * @return the value or array with slashes added.
 */
function cd_addslashes_deep( $inValue ) {
    return
        ( is_array( $inValue )
          ? array_map( 'cd_addslashes_deep', $inValue )
          : addslashes( $inValue ) );
    }



/**
 * Recursively applies the stripslashes function to arrays of arrays.
 * This effectively disables magic_quote escaping behavior. 
 *
 * @inValue the value or array to stripslashes from.
 *
 * @return the value or array with slashes removed.
 */
function cd_stripslashes_deep( $inValue ) {
    return
        ( is_array( $inValue )
          ? array_map( 'sb_stripslashes_deep', $inValue )
          : stripslashes( $inValue ) );
    }




/**
 * Filters a $_REQUEST variable using a regex match.
 *
 * Returns "" (or specified default value) if there is no match.
 */
function cd_requestFilter( $inRequestVariable, $inRegex, $inDefault = "" ) {
    if( ! isset( $_REQUEST[ $inRequestVariable ] ) ) {
        return $inDefault;
        }
    
    $numMatches = preg_match( $inRegex,
                              $_REQUEST[ $inRequestVariable ], $matches );

    if( $numMatches != 1 ) {
        return $inDefault;
        }
        
    return $matches[0];
    }





// this function checks the password directly from a request variable
// or via hash from a cookie.
//
// It then sets a new cookie for the next request.
//
// This avoids storing the password itself in the cookie, so a stale cookie
// (cached by a browser) can't be used to figure out the cookie and log in
// later. 
function cd_checkPassword( $inFunctionName ) {
    $password = "";
    $password_hash = "";

    $badCookie = false;
    
    
    global $accessPasswords, $tableNamePrefix, $remoteIP;

    $cookieName = $tableNamePrefix . "cookie_password_hash";

    
    if( isset( $_REQUEST[ "password" ] ) ) {
        $password = $_REQUEST[ "password" ];

        // generate a new hash cookie from this password
        $newSalt = time();
        $newHash = md5( $newSalt . $password );
        
        $password_hash = $newSalt . "_" . $newHash;
        }
    else if( isset( $_COOKIE[ $cookieName ] ) ) {
        $password_hash = $_COOKIE[ $cookieName ];
        
        // check that it's a good hash
        
        $hashParts = preg_split( "/_/", $password_hash );

        // default, to show in log message on failure
        // gets replaced if cookie contains a good hash
        $password = "(bad cookie:  $password_hash)";

        $badCookie = true;
        
        if( count( $hashParts ) == 2 ) {
            
            $salt = $hashParts[0];
            $hash = $hashParts[1];

            foreach( $accessPasswords as $truePassword ) {    
                $trueHash = md5( $salt . $truePassword );
            
                if( $trueHash == $hash ) {
                    $password = $truePassword;
                    $badCookie = false;
                    }
                }
            
            }
        }
    else {
        // no request variable, no cookie
        // cookie probably expired
        $badCookie = true;
        $password_hash = "(no cookie.  expired?)";
        }
    
        
    
    if( ! in_array( $password, $accessPasswords ) ) {

        if( ! $badCookie ) {
            
            echo "Incorrect password.";

            cd_log( "Failed $inFunctionName access with password:  ".
                    "$password" );
            }
        else {
            echo "Session expired.";
                
            cd_log( "Failed $inFunctionName access with bad cookie:  ".
                    "$password_hash" );
            }
        
        die();
        }
    else {
        // set cookie again, renewing it, expires in 24 hours
        $expireTime = time() + 60 * 60 * 24;
    
        setcookie( $cookieName, $password_hash, $expireTime, "/" );
        }
    }
 



function cd_clearPasswordCookie() {
    global $tableNamePrefix;

    $cookieName = $tableNamePrefix . "cookie_password_hash";

    // expire 24 hours ago (to avoid timezone issues)
    $expireTime = time() - 60 * 60 * 24;

    setcookie( $cookieName, "", $expireTime, "/" );
    }






?>
