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
          mMenuButton( mainFont, 4, -4, translate( "returnMenu" ) ),
          mReturnToMenu( false ) {

    addComponent( &mMenuButton );
    mMenuButton.addActionListener( this );
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



void CheckinHousePage::actionPerformed( GUIComponent *inTarget ) {
    if( inTarget == &mMenuButton ) {
        mReturnToMenu = true;
        }
    }


void CheckinHousePage::step() {
    if( mWebRequest != -1 ) {
            
        int stepResult = stepWebRequest( mWebRequest );

        switch( stepResult ) {
            case 0:
                break;
            case -1:
                mStatusError = true;
                mStatusMessageKey = "err_webRequest";
                clearWebRequest( mWebRequest );
                mWebRequest = -1;
                mMenuButton.setVisible( true );
                break;
            case 1: {
                char *result = getWebResult( mWebRequest );
                clearWebRequest( mWebRequest );
                mWebRequest = -1;
                     
                printf( "Web result = %s\n", result );
   
                if( strstr( result, "OK" ) != NULL ) {
                    // house checked in!
                    

                    mStatusError = false;
                    mStatusMessageKey = "houseCheckedIn";
                    mReturnToMenu = true;
                    }
                else {
                    mStatusError = true;
                    mStatusMessageKey = "houseCheckInFailed";
                    mMenuButton.setVisible( true );
                    }
                
                        
                        
                delete [] result;
                }
                break;
            }
        }
    }



        
void CheckinHousePage::makeActive( char inFresh ) {
    if( !inFresh ) {
        return;
        }
    
    // send back to server            
    char *ticketHash = getTicketHash();
        
            
    
    char *actionString = autoSprintf( 
        "action=end_edit_house&user_id=%d"
        "&%s&house_map=%s",
        userID, ticketHash, mHouseMap );
    delete [] ticketHash;
            
    
    mWebRequest = startWebRequest( "POST", 
                                   serverURL, 
                                   actionString );
    
    printf( "Starting web request %s %s\n", serverURL, actionString );

    delete [] actionString;

    mReturnToMenu = false;
    mStatusError = false;
    mStatusMessageKey = NULL;

    mMenuButton.setVisible( false );
    }


