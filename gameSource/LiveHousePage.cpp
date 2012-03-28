#include "LiveHousePage.h"

#include "ticketHash.h"

#include "minorGems/game/game.h"

#include "minorGems/util/stringUtils.h"


#include <time.h>


extern char *serverURL;

extern int userID;



LiveHousePage::LiveHousePage()
        : mWebRequest( -1 ),
          mLastPingTime( time( NULL ) ),
          mLastActionTime( time( NULL ) ) {
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
    
    mLastPingTime = time( NULL );
    mLastActionTime = mLastPingTime;
    }



void LiveHousePage::step() {
    if( mWebRequest != -1 ) {
            
        int result = stepWebRequest( mWebRequest );
          
        if( result != 0 ) {
            clearWebRequest( mWebRequest );
            mWebRequest = -1;
            }
        }
    else {
        int currentTime = time( NULL );
        
        if( currentTime > mLastPingTime + 60 * 4 ) {
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
                
                mLastPingTime = currentTime;
                }
            }
        
        }
    
    }



