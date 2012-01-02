#include "CheckoutHousePage.h"
#include "ticketHash.h"
#include "message.h"


#include "minorGems/game/Font.h"
#include "minorGems/game/game.h"

#include "minorGems/util/stringUtils.h"


extern Font *mainFont;


extern char *serverURL;

extern int userID;



CheckoutHousePage::CheckoutHousePage() 
        : mWebRequest( -1 ),
          mHouseMap( NULL ),
          mStatusError( false ),
          mStatusMessageKey( NULL ),
          mMenuButton( mainFont, 4, -4, translate( "returnMenu" ) ),
          mReturnToMenu( false ) {
    }


        
CheckoutHousePage::~CheckoutHousePage() {
    if( mWebRequest != -1 ) {
        clearWebRequest( mWebRequest );
        }
    if( mHouseMap != NULL ) {
        delete [] mHouseMap;
        }
    }



char CheckoutHousePage::getReturnToMenu() {
    return mReturnToMenu;
    }



char *CheckoutHousePage::getHouseMap() {
    if( mHouseMap == NULL ) {
        return NULL;
        }
    else {
        return stringDuplicate( mHouseMap );
        }
    }



void CheckoutHousePage::step() {
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
                    mStatusMessageKey = "houseBeingRobbed";
                    }
                else {
                    // house checked out!
                    
                    int size = strlen( result );
                    
                    mHouseMap = new char[ size + 1 ];
                    
                    sscanf( result, "%s", mHouseMap );
                    printf( "HouseMap = %s\n", mHouseMap );
                    }
                        
                        
                delete [] result;
                }
                break;
            }
        }
    }


        
void CheckoutHousePage::draw( doublePair inViewCenter, 
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


        
void CheckoutHousePage::makeActive() {
    char *ticketHash = getTicketHash();

    char *fullRequestURL = autoSprintf( 
        "%s?action=start_edit_house&user_id=%d"
        "&%s",
        serverURL, userID, ticketHash );
    delete [] ticketHash;
    
    mWebRequest = startWebRequest( "GET", 
                                   fullRequestURL, 
                                   NULL );
    
    printf( "Starting web request with URL %s\n", 
            fullRequestURL );

    delete [] fullRequestURL;
    }



void CheckoutHousePage::makeNotActive() {
    }
        

        
void CheckoutHousePage::pointerMove( float inX, float inY ) {
    if( mWebRequest != -1 || 
        mHouseMap != NULL ) {
        return;
        }

    mMenuButton.pointerMove( inX, inY );
    }



void CheckoutHousePage::pointerDown( float inX, float inY ) {
    if( mWebRequest != -1 || 
        mHouseMap != NULL ) {
        return;
        }

    mMenuButton.pointerDown( inX, inY );
    }



void CheckoutHousePage::pointerDrag( float inX, float inY ) {
    if( mWebRequest != -1 || 
        mHouseMap != NULL ) {
        return;
        }

    mMenuButton.pointerDrag( inX, inY );
    }



void CheckoutHousePage::pointerUp( float inX, float inY ) {
    if( mWebRequest != -1 || 
        mHouseMap != NULL ) {
        return;
        }

    if( mMenuButton.pointerUp( inX, inY ) ) {
        mReturnToMenu = true;
        }
    }

