#include "LiveHousePage.h"

#include "ticketHash.h"

#include "minorGems/game/game.h"

#include "minorGems/util/stringUtils.h"


#include <time.h>


extern char *serverURL;

extern int userID;


// static init
// assume ping sent at startup, because no house checked out at startup,
// and checking out a house automatically pings it
int LiveHousePage::sLastPingTime = time( NULL );



LiveHousePage::LiveHousePage()
        : mWebRequest( -1 ),
          mLastActionTime( time( NULL ) ),
          mCheckoutStale( false ),
          mStartSelfTestPending( false ), mEndSelfTestPending( false ) {
    }



LiveHousePage::~LiveHousePage() {
    if( mWebRequest != -1 ) {
        clearWebRequest( mWebRequest );
        }
    }



void LiveHousePage::makeActive( char inFresh ) {
    if( !inFresh ) {
        return;
        }

    // this page becoming active is an action
    mLastActionTime = time( NULL );
    mCheckoutStale = false;
    }



void LiveHousePage::actionHappened() {
    mLastActionTime = time( NULL );
    }


void LiveHousePage::startSelfTest() {
    mStartSelfTestPending = true;
    }


void LiveHousePage::endSelfTest() {
    mEndSelfTestPending = true;
    }



void LiveHousePage::step() {
    if( mWebRequest != -1 ) {
            
        int result = stepWebRequest( mWebRequest );
          
        if( result != 0 ) {
            
            switch( result ) {
                case -1:
                    mCheckoutStale = true;
                    break;
                case 1: {
                    char *result = getWebResult( mWebRequest );

                    // same OK result expected whether we
                    // have sent a ping or a self-test start/end
                    if( strstr( result, "OK" ) == NULL ) {
                        mCheckoutStale = true;
                        }
                    else {
                        // handled the send of the start or end message
                        if( mStartSelfTestPending ) {
                            mStartSelfTestPending = false;
                            }
                        else if( mEndSelfTestPending ) {
                            mEndSelfTestPending = false;
                            }
                        }

                    delete [] result;
                    }
                }

            clearWebRequest( mWebRequest );
            mWebRequest = -1;
            }
        }
    else if( mStartSelfTestPending || mEndSelfTestPending ) {

        const char *command = "start_self_test";
        
        if( ! mStartSelfTestPending ) {
            command = "end_self_test";
            }
        

        char *ticketHash = getTicketHash();
            
        char *fullRequestURL = autoSprintf( 
            "%s?action=%s&user_id=%d"
            "&%s",
            serverURL, command, userID, ticketHash );
        delete [] ticketHash;
        
        mWebRequest = startWebRequest( "GET", 
                                       fullRequestURL, 
                                       NULL );
        
        printf( "Sending web request:  %s\n", fullRequestURL );
        
        delete [] fullRequestURL;
        
        // counts as a ping
        sLastPingTime = time( NULL );
        }
    else if( ! mCheckoutStale ) {
        int currentTime = time( NULL );
        
        if( currentTime > sLastPingTime + 60 * 4 ) {
            // getting close to five minute timeout mark
            
            if( currentTime - mLastActionTime < 60 * 5 ) {
                // there's been activity in the last five minutes
                
                // send ping
                
                char *ticketHash = getTicketHash();
            
                char *fullRequestURL = autoSprintf( 
                    "%s?action=ping_house&user_id=%d"
                    "&%s",
                    serverURL, userID, ticketHash );
                delete [] ticketHash;
                
                mWebRequest = startWebRequest( "GET", 
                                               fullRequestURL, 
                                               NULL );
                
                printf( "Sending web request:  %s\n", fullRequestURL );
                
                delete [] fullRequestURL;
                
                sLastPingTime = currentTime;
                }
            }
        
        }
    
    }



