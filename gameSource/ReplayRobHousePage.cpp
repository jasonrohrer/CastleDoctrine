#include "ReplayRobHousePage.h"
#include "ticketHash.h"

#include "minorGems/game/Font.h"
#include "minorGems/game/game.h"
#include "minorGems/game/drawUtils.h"

#include "minorGems/util/stringUtils.h"


extern Font *mainFont;


extern char *serverURL;

extern int userID;



ReplayRobHousePage::ReplayRobHousePage() 
        : mWebRequest( -1 ),
          mGridDisplay( 0, 0 ),
          mDoneButton( mainFont, 9, -5, translate( "doneEdit" ) ) {

    addComponent( &mDoneButton );
    addComponent( &mGridDisplay );

    mDoneButton.addActionListener( this );
    mGridDisplay.addActionListener( this );
    }


        
ReplayRobHousePage::~ReplayRobHousePage() {
    if( mWebRequest != -1 ) {
        clearWebRequest( mWebRequest );
        }
    }



void ReplayRobHousePage::setHouseMap( char *inHouseMap ) {
    mGridDisplay.setHouseMap( inHouseMap );
    }



void ReplayRobHousePage::setMoveList( char *inMoveList ) {
    mGridDisplay.setMoveList( inMoveList );
    }



void ReplayRobHousePage::actionPerformed( GUIComponent *inTarget ) {
    if( inTarget == &mDoneButton ) {
        mDone = true;
        }
    else if( inTarget == &mGridDisplay ) {
        if( mGridDisplay.getSuccess() ) {
            mDone = true;
            }
        }
    }



void ReplayRobHousePage::step() {
    if( mWebRequest != -1 ) {
            
        int result = stepWebRequest( mWebRequest );
                
        switch( result ) {
            case 0:
                break;
            case -1:
                mStatusError = true;
                mStatusMessageKey = "err_webRequest";
                clearWebRequest( mWebRequest );
                mWebRequest = -1;
                break;
            case 1: {
                char *result = getWebResult( mWebRequest );
                clearWebRequest( mWebRequest );
                mWebRequest = -1;
                        
                if( strstr( result, "DENIED" ) != NULL ) {
                    mStatusError = true;
                    mStatusMessageKey = "houseBeingRobbed";
                    }
                else {
                    // house checked out!
                    
                    //int size = strlen( result );
                    
                    //mHouseMap = new char[ size + 1 ];
                    
                    //sscanf( result, "%s", mHouseMap );
                    }
                        
                        
                delete [] result;
                }
                break;
            }
        }
    }


        
void ReplayRobHousePage::makeActive( char inFresh ) {
    if( !inFresh ) {
        return;
        }
    
    mDone = false;
    }
        

