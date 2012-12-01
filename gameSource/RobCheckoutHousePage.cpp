#include "RobCheckoutHousePage.h"
#include "ticketHash.h"
#include "message.h"
#include "nameProcessing.h"
#include "LiveHousePage.h"

#include <time.h>

#include "minorGems/game/Font.h"
#include "minorGems/game/game.h"

#include "minorGems/util/stringUtils.h"


extern Font *mainFont;


extern char *serverURL;

extern int userID;



RobCheckoutHousePage::RobCheckoutHousePage() 
        : mWebRequest( -1 ),
          mOwnerName( NULL ),
          mHouseMap( NULL ),
          mBackpackContents( NULL ),
          mGalleryContents( NULL ),
          mMenuButton( mainFont, 4, -4, translate( "returnMenu" ) ),
          mReturnToMenu( false ),
          mToRobCharacterName( NULL ) {

    addComponent( &mMenuButton );
    mMenuButton.addActionListener( this );
    }


        
RobCheckoutHousePage::~RobCheckoutHousePage() {
    if( mWebRequest != -1 ) {
        clearWebRequest( mWebRequest );
        }
    if( mOwnerName != NULL ) {
        delete [] mOwnerName;
        }
    if( mHouseMap != NULL ) {
        delete [] mHouseMap;
        }
    if( mBackpackContents != NULL ) {
        delete [] mBackpackContents;
        }
    if( mGalleryContents != NULL ) {
        delete [] mGalleryContents;
        }
    if( mToRobCharacterName != NULL ) {
        delete [] mToRobCharacterName;
        }
    }



void RobCheckoutHousePage::setToRobUserID( int inID ) {
    mToRobUserID = inID;
    }


void RobCheckoutHousePage::setToRobCharacterName( const char *inName ) {
    if( mToRobCharacterName != NULL ) {
        delete [] mToRobCharacterName;
        }
    mToRobCharacterName = stringDuplicate( inName );
    }



char RobCheckoutHousePage::getReturnToMenu() {
    return mReturnToMenu;
    }



char *RobCheckoutHousePage::getHouseMap() {
    if( mHouseMap == NULL ) {
        return NULL;
        }
    else {
        return stringDuplicate( mHouseMap );
        }
    }


char *RobCheckoutHousePage::getOwnerName() {
    if( mOwnerName == NULL ) {
        return NULL;
        }
    else {
        return stringDuplicate( mOwnerName );
        }
    }



char *RobCheckoutHousePage::getBackpackContents() {
    if( mBackpackContents == NULL ) {
        return NULL;
        }
    else {
        return stringDuplicate( mBackpackContents );
        }
    }



char *RobCheckoutHousePage::getGalleryContents() {
    if( mGalleryContents == NULL ) {
        return NULL;
        }
    else {
        return stringDuplicate( mGalleryContents );
        }
    }


int RobCheckoutHousePage::getMusicSeed() {
    return mMusicSeed;
    }




void RobCheckoutHousePage::actionPerformed( GUIComponent *inTarget ) {
    if( inTarget == &mMenuButton ) {
        mReturnToMenu = true;
        }
    }


void RobCheckoutHousePage::step() {
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
   
                if( strstr( result, "DENIED" ) != NULL ) {
                    mStatusError = true;
                    mStatusMessageKey = "houseBeingRobbedOrEdited";
                    mMenuButton.setVisible( true );
                    }
                else if( strstr( result, "RECLAIMED" ) != NULL ) {
                    mStatusError = true;
                    mStatusMessageKey = "houseReclaimed";
                    mMenuButton.setVisible( true );
                    }
                else {
                    // house checked out!
                    
                    SimpleVector<char *> *tokens =
                        tokenizeString( result );
                    
                    if( tokens->size() != 6 ||
                        strcmp( *( tokens->getElement( 5 ) ), "OK" ) != 0 ) {
                        mStatusError = true;
                        mStatusMessageKey = "err_badServerResponse";
                        mMenuButton.setVisible( true );
                    
                        for( int i=0; i<tokens->size(); i++ ) {
                            delete [] *( tokens->getElement( i ) );
                            }
                        }
                    else {
                        mOwnerName = nameParse( *( tokens->getElement( 0 ) ) );
                        mHouseMap = *( tokens->getElement( 1 ) );
                        mBackpackContents = *( tokens->getElement( 2 ) );
                        mGalleryContents = *( tokens->getElement( 3 ) );

                        mMusicSeed = 0;
                        sscanf( *( tokens->getElement( 4 ) ),
                                "%d", &mMusicSeed );

                        printf( "OwnerName = %s\n", mOwnerName );
                        printf( "HouseMap = %s\n", mHouseMap );
                        printf( "Backpack = %s\n", mBackpackContents );
                        printf( "Gallery = %s\n", mGalleryContents );
                        printf( "MusicSeed = %d\n", mMusicSeed );
                        

                        delete [] *( tokens->getElement( 4 ) );
                        delete [] *( tokens->getElement( 5 ) );

                        // reset ping time, because house check-out
                        // counts as a ping
                        LiveHousePage::sLastPingTime = time( NULL );
                        }
                    
                    delete tokens;
                    }
                        
                        
                delete [] result;
                }
                break;
            }
        }
    }


        
void RobCheckoutHousePage::makeActive( char inFresh ) {
    if( !inFresh ) {
        return;
        }

    if( mHouseMap != NULL ) {
        delete [] mHouseMap;
        }
    mHouseMap = NULL;

    if( mOwnerName != NULL ) {
        delete [] mOwnerName;
        }
    mOwnerName = NULL;
    
    char *ticketHash = getTicketHash();

    char *fullRequestURL = autoSprintf( 
        "%s?action=start_rob_house&user_id=%d&to_rob_user_id=%d"
        "&to_rob_character_name=%s&%s",
        serverURL, userID, mToRobUserID, mToRobCharacterName, ticketHash );
    delete [] ticketHash;
    
    mWebRequest = startWebRequest( "GET", 
                                   fullRequestURL, 
                                   NULL );
    
    printf( "Starting web request with URL %s\n", 
            fullRequestURL );

    delete [] fullRequestURL;
    
    mMenuButton.setVisible( false );

    mStatusError = false;
    mStatusMessageKey = NULL;

    mReturnToMenu = false;
    }

