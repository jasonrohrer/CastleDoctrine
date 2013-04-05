<?php

// for header.php to work
$pathToRoot = "";



// Basic settings
// You must set these for seedBlogs to work

$databaseServer = "localhost";
$databaseUsername = "testUser";
$databasePassword = "testPassword";
$databaseName = "test";

// The URL of the "homepage" of your seedBlogs site.
// Used in administrative emails.
$mainSiteURL = "http://thecastledoctrine.net/news.php";

// The full URL to the seedBlogs.php script.
// Used in RSS feeds to uniquely point to specific posts.
$fullSeedBlogsURL = "http://thecastledoctrine.net/seedBlogs.php";


// End Basic settings



// Customization settings

// Adjust these to change the way seedBlogs looks and works.


// Prefix to use in table names (in case more than one application is using
// the same database).  Three tables are created:  "users", posts", and
// "order_map".
// If $tableNamePrefix is "test_" then the tables will be named "test_users",
// "test_posts", and "test_order_map".  Thus, more than one seedBlogs
// installation can use the same database (or seedBlogs can share a database
// with another application that uses similar table names).
$tableNamePrefix = "cdnews_";


// set to 1 to automatically approve all account registrations
// set to 0 to require administrator approval for each account
$autoApproveUsers = 0;

// set to 1 to allow posts from all registered (and approved) users
// set to 0 to require administrator approval for each post
$autoApprovePosts = 0;

// set to 1 to allow the public to submit posts (into the approval queue)
// without logging in
// Even if autoApprovePosts is set, public submissions will still go into
// the approval queue.
// If set to 1, it can be overridden to hide public posting for certain
//   blogs by calling seedBlogFormatted with appropriate parameters.
//   If set to 0, it cannot be overridden.
// This setting *does not* affect comment lists, which always accept posts
//   from the public (see the next setting to control queuing of public
//   comments).
$allowSubmissionsFromPublic = 0;


// Set to 1 to automatically approve and display comments from the public.
// Set to 0 to put all public comments into the queue for approval.
// This only affects posts for which comments have been enabled.
// Comments from registered users are always auto-approved.
$autoApprovePublicComments = 0;


// Set to 1 for to email notices of new pending posts and accounts to the
// users who are marked as admins.  An email about *each* new pending item
// will be sent to *each* admin user.
// Set to 0 to disable these email notices
$emailAdminsAboutPendingItems = 0;


// The name of this site (appears in administrative emails)
$siteName = "Castle Doctrine News";
// Used as the From address for administrative emails
$siteEmailAddress = "Jason Rohrer <jason@thecastledoctrine.net>";


// This is not displayed anywhere on the site, but instead is
// used when securely hashing passwords (so that the same password, if
// used on two different sites with different $siteShortName settings,
// will have different hashes in the database).
$siteShortName = "cdNews";


// For standard displays (lists of posts), you call seedBlogs from
//   within your web page, so the surrounding HTML is under your full control.
// However, some pages (such as the edit forms and the full story displays)
//   are generated by seedBlogs in response to user actions.  To customize
//   the layout of these pages, you must provide header and footer files.

// inserted at the top of every page generated by seedBlogs 
$header = "header.php";

// inserted at the end of every page generated by seedBlogs
$footer = "footer.php";



// These are used to format stories in both list displays and full-story
//    displays.
// The default settings are specified below, but they can be overridden
//   for a given list display if you call the "advanced" seedBlog interface
//   function (seedBlogFormatted).
// However, these default settings will always be used in full-story displays,
//   archive list displays, and comment list displays.

// HTML used to open/close each story block (only used in story lists that
// display intro text for each story)
$storyBlockFormatOpen = "<TABLE WIDTH=100% cellpadding=0><TR><TD>";
$storyBlockFormatClose = "</TD></TR></TABLE>";


// HTML used to format headlines when intro text is shown
$headlineFormatOpen = "<FONT SIZE=6>";
$headlineFormatClose = "</FONT>";

// HTML used to open/close the text below each headline
// this block of text includes:
//   admin widgets, intro text, Read more link, and comment widget
$textBlockFormatOpen = "<TABLE WIDTH=100% cellpadding=5><TR><TD bgcolor=#222222>";
$textBlockFormatClose = "</TD></TR></TABLE>";


// HTML used to open/close each story block (for lists where intro text
// is hidden)
$linkStoryBlockFormatOpen =
"<TABLE CELLSPACING=2 CELLPADDING=0><TR><TD NOWRAP VALIGN=TOP>--</TD><TD VALIGN=TOP>";
$linkStoryBlockFormatClose = "</TD></TR></TABLE>";


// HTML used to format link-only headlines (for lists where intro text
// is hidden).  These format strings are inserted *inside* the HTML link tags.
// Thus, they can be used to change the color of the link text.
$linkHeadlineFormatOpen = "";
$linkHeadlineFormatClose = "";


// HTML to insert between each story in a story list when intro text is shown
$storySeparator = "<BR><BR><BR>";

// HTML to insert between each headline link when intro text is hidden
$linkStorySeparator = "";

// used around the list of comments (shown at the bottom of the story display)
// comment lists are formatted as a seedBlog using the default formatting
// settings specified above
$commentListOpen = "<BR><BR><FONT SIZE=5>Comments</FONT><BR>";
$commentListClose = "";



// End of all settings


?>