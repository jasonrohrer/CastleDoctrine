<?php
include_once( "seedBlogs.php" );
include_once( "header.php" );
?>

</CENTER>



<table width="100%" border=0><tr>

<td align=left valign=top>

<!-- hard-coded clean RSS link
This needs to be redirected with .htaccess
-->
<a HREF="http://thecastledoctrine.net/rss.xml"><img border=0 width=20 height=20 src="rss-icon.png"></a>

</td>

<td align=right>

<?php
seedBlogs_showSearchBox(
    // 15 characters wide
    15,
    // show the Search button
    true );
?>

</td>
</tr>
</table>


<?php
seedBlog(
    // name of this seed blog in the database
    "news",
    // 1 = show intro text below headlines
    // 0 = show only headlines
    1,
    // 1 = show author for each post
    // 0 = hide author
    1,
    // 1 = show creation date for each post
    // 0 = hide dates
    1,
    // 2 = allow custom order tweaking with up/down widgets
    // 1 = order by creation date (newest first)
    // 0 = order by expiration date (oldest first)
    1,
    // show at most 5 posts
    5,
    // skip none of them (start with first post)
    0,
    // show the archive link
    1,
    // hide the submission link from public
    0 );
/*
    // opening tags for each story block
    "<TABLE WIDTH=100% cellspacing=0 cellpadding=0>",
    // closing tags for each story block
    "</TABLE>",
    // opening tags for headlines
    "<TR><TD colspan=2 BGCOLOR=\"#ffffff\">" .
    "<FONT SIZE=5>",
    // closing tags for headlines
    "</FONT></td></tr>",
    // opening tags for each text block
    "<tr><td colspan=2 bgcolor=\"#cccccc\" height=1></td></tr>" .
    "<TR><td bgcolor=\"#cccccc\" width=1></td>" .
    "<TD BGCOLOR=\"#ffffff\">" .
    "<table border=0 width=\"100%\" cellspacing=0 " .
    "cellpadding=5><tr><td>",
    // closing tags for each text block
    "</td></tr></table></TD></TR>",
    // story separator
    "<BR><BR><BR>" );
*/
?>

<BR>

<hr>

<font size=6>Featured articles:</font>
<br>
<br>

<?php
seedBlog(
    // name of this seed blog in the database
    "news",
    // 1 = show intro text below headlines
    // 0 = show only headlines
    1,
    // 1 = show author for each post
    // 0 = hide author
    1,
    // 1 = show creation date for each post
    // 0 = hide dates
    1,
    // 2 = allow custom order tweaking with up/down widgets
    // 1 = order by creation date (newest first)
    // 0 = order by expiration date (oldest first)
    2,
    // show at most 5 posts
    5,
    // skip none of them (start with first post)
    0,
    // hide the archive link
    0,
    // hide the submission link from public
    0 );
?>

<BR>




<!-- see hard-coded clean rss link above
<?php
// show an RSS feed button here
seedBlogRSSButton(
    // the name of the blog in the database
    "stories",
    // title of the RSS channel
    "Arthouse Games",
    // description of the channel
    "insisting that our medium can reach beyond entertainment.",
    // include at most 10 features in the feed
    10,
    // show authors
    1,
    // show post dates
    1 );
?>
-->

<table width="100%" border=0><tr>

<td align=right>
[<a href="newsLogin.php">login</a>]
</td>
</tr>
</table>

<CENTER>

<?php include_once( "footer.php" ); ?>

