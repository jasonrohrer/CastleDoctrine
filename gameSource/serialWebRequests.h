

// provides serial versions of the web request calls in
// minorGems/game/game.h

// This ensures that web requests are processed in order, with
// early requests completing up to full server response before later
// requests are sent to the server.

// Prevents unexpected request inerleavings (important in this case because
// most requests have non-repetable sequence numbers in them 
// (to thwart replay attacks), and the server only tracks the last sequence
// number used.  If requests arrive at the server out-of-order, then
// the server rejects some of them as stale.



int startWebRequestSerial( const char *inMethod, const char *inURL,
                           const char *inBody );

int stepWebRequestSerial( int inHandle );


char *getWebResultSerial( int inHandle );


// does not wait for request to finish before ending it
void clearWebRequestSerial( int inHandle );