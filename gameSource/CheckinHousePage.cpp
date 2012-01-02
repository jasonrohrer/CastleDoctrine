#include "CheckinHousePage.h"
#include "ticketHash.h"
#include "message.h"


#include "minorGems/game/Font.h"
#include "minorGems/game/game.h"

#include "minorGems/util/stringUtils.h"


extern Font *mainFont;


extern char *serverURL;

extern int userID;



CheckinHousePage::CheckinHousePage() 
        : mWebRequest( -1 ),
          mHouseMap( NULL ),
          mStatusError( false ),
          mStatusMessageKey( NULL ),
          mMenuButton( mainFont, 4, -4, translate( "returnMenu" ) ),
          mReturnToMenu( false ) {
    }


        
CheckinHousePage::~CheckinHousePage() {
    if( mWebRequest != -1 ) {
        clearWebRequest( mWebRequest );
        }
    if( mHouseMap != NULL ) {
        delete [] mHouseMap;
        }
    }



char CheckinHousePage::getReturnToMenu() {
    return mReturnToMenu;
    }



void CheckinHousePage::setHouseMap( char *inHouseMap ) {
    if( mHouseMap != NULL ) {
        delete [] mHouseMap;
        }
    mHouseMap = stringDuplicate( inHouseMap );
    }



void CheckinHousePage::step() {
    if( mWebRequest != -1 ) {
            
        int stepResult = stepWebRequest( mWebRequest );

        printf( "StepResult %d\n", stepResult );
                
        switch( stepResult ) {
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
                     
                printf( "Web result = %s\n", result );
   
                if( strstr( result, "DENIED" ) != NULL ) {
                    mStatusError = true;
                    mStatusMessageKey = "houseCheckInFailed";
                    }
                else {
                    // house checked in!
                    

                    mStatusError = false;
                    mStatusMessageKey = "houseCheckedIn";
                    }
                        
                        
                delete [] result;
                }
                break;
            }
        }
    }


        
void CheckinHousePage::draw( doublePair inViewCenter, 
                          double inViewSize ) {

    if( mWebRequest == -1 && 
        mHouseMap == NULL ) {
        
        mMenuButton.draw();
        }
    

    if( mStatusMessageKey != NULL ) {
        doublePair labelPos = { 0, -5 };

        drawMessage( mStatusMessageKey, labelPos, mStatusError );
        }

    }


        
void CheckinHousePage::makeActive() {
    // send back to server            
    char *ticketHash = getTicketHash();
        
            
    
    char *actionString = autoSprintf( 
        "action=end_edit_house&user_id=%d"
        "&%s&houseMap=%s",
        userID, ticketHash, mHouseMap );
    delete [] ticketHash;
            
    
    mWebRequest = startWebRequest( "POST", 
                                   serverURL, 
                                   actionString );
    
    delete [] actionString;
    }



void CheckinHousePage::makeNotActive() {
    }
        

        
void CheckinHousePage::pointerMove( float inX, float inY ) {
    if( mWebRequest != -1 || 
        mHouseMap != NULL ) {
        return;
        }

    mMenuButton.pointerMove( inX, inY );
    }



void CheckinHousePage::pointerDown( float inX, float inY ) {
    if( mWebRequest != -1 || 
        mHouseMap != NULL ) {
        return;
        }

    mMenuButton.pointerDown( inX, inY );
    }



void CheckinHousePage::pointerDrag( float inX, float inY ) {
    if( mWebRequest != -1 || 
        mHouseMap != NULL ) {
        return;
        }

    mMenuButton.pointerDrag( inX, inY );
    }



void CheckinHousePage::pointerUp( float inX, float inY ) {
    if( mWebRequest != -1 || 
        mHouseMap != NULL ) {
        return;
        }

    if( mMenuButton.pointerUp( inX, inY ) ) {
        mReturnToMenu = true;
        }
    }

