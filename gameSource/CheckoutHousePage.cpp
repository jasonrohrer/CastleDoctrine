#include "CheckoutHousePage.h"
#include "ticketHash.h"
#include "message.h"
#include "LiveHousePage.h"

#include <time.h>


#include "minorGems/game/Font.h"
#include "minorGems/game/game.h"

#include "minorGems/util/stringUtils.h"


extern Font *mainFont;


extern char *serverURL;

extern int userID;



CheckoutHousePage::CheckoutHousePage() 
        : mWebRequest( -1 ),
          mHouseMap( NULL ),
          mVaultContents( NULL ),
          mBackpackContents( NULL ),
          mGalleryContents( NULL ),
          mPriceList( NULL ),
          mMenuButton( mainFont, 4, -4, translate( "returnMenu" ) ),
          mReturnToMenu( false ) {

    addComponent( &mMenuButton );
    mMenuButton.addActionListener( this );
    }


        
CheckoutHousePage::~CheckoutHousePage() {
    if( mWebRequest != -1 ) {
        clearWebRequest( mWebRequest );
        }
    if( mHouseMap != NULL ) {
        delete [] mHouseMap;
        }
    if( mVaultContents != NULL ) {
        delete [] mVaultContents;
        }
    if( mBackpackContents != NULL ) {
        delete [] mBackpackContents;
        }
    if( mGalleryContents != NULL ) {
        delete [] mGalleryContents;
        }
    if( mPriceList != NULL ) {
        delete [] mPriceList;
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



char *CheckoutHousePage::getVaultContents() {
    if( mVaultContents == NULL ) {
        return NULL;
        }
    else {
        return stringDuplicate( mVaultContents );
        }
    }



char *CheckoutHousePage::getBackpackContents() {
    if( mBackpackContents == NULL ) {
        return NULL;
        }
    else {
        return stringDuplicate( mBackpackContents );
        }
    }



char *CheckoutHousePage::getGalleryContents() {
    if( mGalleryContents == NULL ) {
        return NULL;
        }
    else {
        return stringDuplicate( mGalleryContents );
        }
    }



char *CheckoutHousePage::getPriceList() {
    if( mPriceList == NULL ) {
        return NULL;
        }
    else {
        return stringDuplicate( mPriceList );
        }
    }



int CheckoutHousePage::getLootValue() {
    return mLootValue;
    }


char CheckoutHousePage::getMustSelfTest() {
    return mMustSelfTest;
    }


int CheckoutHousePage::getMusicSeed() {
    return mMusicSeed;
    }



void CheckoutHousePage::actionPerformed( GUIComponent *inTarget ) {
    if( inTarget == &mMenuButton ) {
        mReturnToMenu = true;
        }
    }


void CheckoutHousePage::step() {
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
                    mStatusMessageKey = "houseBeingRobbed";
                    mMenuButton.setVisible( true );
                    }
                else {
                    // house checked out!

                    SimpleVector<char *> *lines = 
                        tokenizeString( result );
                    
                    if( lines->size() != 9
                        ||
                        strcmp( *( lines->getElement( 8 ) ), "OK" ) != 0 ) {

                        setStatus( "err_badServerResponse", true );
                        mMenuButton.setVisible( true );
                    
                        for( int i=0; i<lines->size(); i++ ) {
                            delete [] *( lines->getElement( i ) );
                            }
                        }
                    else {
                        mHouseMap = *( lines->getElement( 0 ) );
                        mVaultContents = *( lines->getElement( 1 ) );
                        mBackpackContents = *( lines->getElement( 2 ) );
                        mGalleryContents = *( lines->getElement( 3 ) );
                        mPriceList = *( lines->getElement( 4 ) );
                        
                        mLootValue = 0;
                        sscanf( *( lines->getElement( 5 ) ),
                                "%d", &mLootValue );

                        int selfTestValue = 0;
                        sscanf( *( lines->getElement( 6 ) ),
                                "%d", &selfTestValue );

                        if( selfTestValue == 1 ) {
                            mMustSelfTest = true;
                            }
                        else {
                            mMustSelfTest = false;
                            }
                        
                        mMusicSeed = 0;
                        sscanf( *( lines->getElement( 7 ) ),
                                "%d", &mMusicSeed );

                        delete [] *( lines->getElement( 5 ) );
                        delete [] *( lines->getElement( 6 ) );
                        delete [] *( lines->getElement( 7 ) );
                        delete [] *( lines->getElement( 8 ) );
                        
                        printf( "HouseMap = %s\n", mHouseMap );
                        printf( "Vault = %s\n", mVaultContents );
                        printf( "Backpack = %s\n", mBackpackContents );
                        printf( "Gallery = %s\n", mGalleryContents );
                        printf( "PriceList = %s\n", mPriceList );
                        printf( "LootValue = %d\n", mLootValue );
                        printf( "MusicSeed = %d\n", mMusicSeed );

                        // reset ping time, because house check-out
                        // counts as a ping
                        LiveHousePage::sLastPingTime = time( NULL );
                        }
                    delete lines;
                    }
                        
                        
                delete [] result;
                }
                break;
            }
        }
    }


        
void CheckoutHousePage::makeActive( char inFresh ) {
    if( !inFresh ) {
        return;
        }

    if( mHouseMap != NULL ) {
        delete [] mHouseMap;
        }
    mHouseMap = NULL;

    if( mVaultContents != NULL ) {
        delete [] mVaultContents;
        }
    mVaultContents = NULL;

    if( mBackpackContents != NULL ) {
        delete [] mBackpackContents;
        }
    mBackpackContents = NULL;

    if( mGalleryContents != NULL ) {
        delete [] mGalleryContents;
        }
    mGalleryContents = NULL;

    if( mPriceList != NULL ) {
        delete [] mPriceList;
        }
    mPriceList = NULL;
    
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
    
    mMenuButton.setVisible( false );

    mStatusError = false;
    mStatusMessageKey = NULL;

    mReturnToMenu = false;
    }

