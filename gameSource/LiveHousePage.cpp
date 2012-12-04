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


int LiveHousePage::sWebRequest = -1;

char LiveHousePage::sStartSelfTestPending = false;
char LiveHousePage::sStartSelfTestSent = false;

char LiveHousePage::sEndSelfTestPending = false;
char LiveHousePage::sEndSelfTestSent = false;



LiveHousePage::LiveHousePage()
        : mLastActionTime( time( NULL ) ),
          mCheckoutStale( false ) {
    }



LiveHousePage::~LiveHousePage() {
    if( sWebRequest != -1 ) {
        clearWebRequest( sWebRequest );
        sWebRequest = -1;
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
    sStartSelfTestPending = true;
    }


void LiveHousePage::endSelfTest() {
    sEndSelfTestPending = true;
    }



void LiveHousePage::step() {
    if( sWebRequest != -1 ) {
            
        int result = stepWebRequest( sWebRequest );
          
        if( result != 0 ) {
            // send is over, not matter what response we get back
            sStartSelfTestSent = false;
            sEndSelfTestSent = false;
            
            switch( result ) {
                case -1:
                    mCheckoutStale = true;
                    printf( "Web request FAILED!\n" );
                    break;
                case 1: {
                    char *response = getWebResult( sWebRequest );

                    printf( "Server response:  %s\n", response );
                    
                    // same OK result expected whether we
                    // have sent a ping or a self-test start/end
                    if( strstr( response, "OK" ) == NULL ) {
                        mCheckoutStale = true;
                        }

                    delete [] response;
                    }
                }

            clearWebRequest( sWebRequest );
            sWebRequest = -1;
            }
        }
    else if( sStartSelfTestPending || sEndSelfTestPending ) {

        const char *command;
        
        if( sStartSelfTestPending ) {
            command = "start_self_test";
            sStartSelfTestSent = true;
            sStartSelfTestPending = false;
            }
        else {
            command = "end_self_test";
            sEndSelfTestSent = true;
            sEndSelfTestPending = false;
            }
        

        char *ticketHash = getTicketHash();
            
        char *fullRequestURL = autoSprintf( 
            "%s?action=%s&user_id=%d"
            "&%s",
            serverURL, command, userID, ticketHash );
        delete [] ticketHash;
        
        sWebRequest = startWebRequest( "GET", 
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
                
                sWebRequest = startWebRequest( "GET", 
                                               fullRequestURL, 
                                               NULL );
                
                printf( "Sending web request:  %s\n", fullRequestURL );
                
                delete [] fullRequestURL;
                
                sLastPingTime = currentTime;
                }
            }
        
        }
    
    }



