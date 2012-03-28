#include "EditHousePage.h"
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



EditHousePage::EditHousePage() 
        : mWebRequest( -1 ),
          mStartHouseMap( NULL ),
          mGridDisplay( 0, 0 ),
          mDoneButton( mainFont, 8, -5, translate( "doneEdit" ) ) {

    addComponent( &mDoneButton );
    addComponent( &mGridDisplay );

    mDoneButton.addActionListener( this );
    mGridDisplay.addActionListener( this );
    }


        
EditHousePage::~EditHousePage() {
    if( mWebRequest != -1 ) {
        clearWebRequest( mWebRequest );
        }

    if( mStartHouseMap != NULL ) {
        delete [] mStartHouseMap;
        }
    }



void EditHousePage::setHouseMap( char *inHouseMap ) {
    if( mStartHouseMap != NULL ) {
        delete [] mStartHouseMap;
        }
    mStartHouseMap = stringDuplicate( inHouseMap );
    
    mGridDisplay.setHouseMap( inHouseMap );
    }



char *EditHousePage::getHouseMap() {
    return mGridDisplay.getHouseMap();
    }



char EditHousePage::houseMapChanged() {
    if( mStartHouseMap == NULL ) {
        return true;
        }

    char *newMap = mGridDisplay.getHouseMap();

    int comp = strcmp( newMap, mStartHouseMap );
    
    delete [] newMap;
    
    if( comp == 0 ) {
        return false;
        }
    return true;
    }



void EditHousePage::actionPerformed( GUIComponent *inTarget ) {
    if( inTarget == &mGridDisplay ) {
        // can't click DONE if house has no goal set
        mDoneButton.setVisible( mGridDisplay.isGoalSet() );

        // change to house map

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
    else if( inTarget == &mDoneButton ) {
        mDone = true;
        }
    }



void EditHousePage::step() {
    if( mWebRequest != -1 ) {
            
        int result = stepWebRequest( mWebRequest );
          
        if( result != 0 ) {
            clearWebRequest( mWebRequest );
            mWebRequest = -1;
            }
        }
    }


        
void EditHousePage::makeActive( char inFresh ) {
    if( !inFresh ) {
        return;
        }
    
    mLastPingTime = time( NULL );

    mDone = false;
    }
        



void EditHousePage::draw( doublePair inViewCenter, 
                               double inViewSize ) {
        
    doublePair labelPos = { 0, 6.75 };
    
    drawMessage( translate( "editDescription" ), labelPos, false );
    }
