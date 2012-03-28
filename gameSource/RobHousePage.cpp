#include "RobHousePage.h"
#include "ticketHash.h"

#include "message.h"

#include "minorGems/game/Font.h"
#include "minorGems/game/game.h"
#include "minorGems/game/drawUtils.h"

#include "minorGems/util/stringUtils.h"


#include <time.h>



extern Font *mainFont;


extern char *serverURL;

extern int userID;



RobHousePage::RobHousePage( const char *inDoneButtonKey ) 
        : mWebRequest( -1 ),
          mGridDisplay( 0, 0 ),
          mDoneButton( mainFont, 8, -5, translate( inDoneButtonKey ) ),
          mDescription( NULL ) {

    addComponent( &mDoneButton );
    addComponent( &mGridDisplay );

    mDoneButton.addActionListener( this );
    mGridDisplay.addActionListener( this );
    }


        
RobHousePage::~RobHousePage() {
    if( mWebRequest != -1 ) {
        clearWebRequest( mWebRequest );
        }

    if( mDescription != NULL ) {
        delete [] mDescription;
        }
    }



void RobHousePage::setHouseMap( char *inHouseMap ) {
    mGridDisplay.setHouseMap( inHouseMap );
    }



char *RobHousePage::getHouseMap() {
    return mGridDisplay.getHouseMap();
    }



void RobHousePage::setDescription( const char *inDescription ) {
    if( mDescription != NULL ) {
        delete [] mDescription;
        }
    mDescription = stringDuplicate( inDescription );
    }



void RobHousePage::actionPerformed( GUIComponent *inTarget ) {
    if( inTarget == &mDoneButton ) {
        mDone = true;
        }
    else if( inTarget == &mGridDisplay ) {
        if( mGridDisplay.getSuccess() ) {
            mDone = true;
            }
        else {
            // send ping on user activity every 30 seconds
            if( time( NULL ) > mLastPingTime + 30 
                &&
                mWebRequest == -1 ) {
                
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

                mLastPingTime = time( NULL );
                }
            }
        
        }
    }



void RobHousePage::step() {
    if( mWebRequest != -1 ) {
            
        int result = stepWebRequest( mWebRequest );
        
        if( result != 0 ) {
            clearWebRequest( mWebRequest );
            mWebRequest = -1;
            }
        }
    }


        
void RobHousePage::makeActive( char inFresh ) {
    if( !inFresh ) {
        return;
        }
    
    mLastPingTime = time( NULL );

    mDone = false;
    }



void RobHousePage::draw( doublePair inViewCenter, 
                               double inViewSize ) {
     
    if( mDescription != NULL ) {
        doublePair labelPos = { 0, 6.75 };
        
        drawMessage( mDescription, labelPos, false );
        }
    }

        

