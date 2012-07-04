#include "RobCheckinHousePage.h"
#include "ticketHash.h"
#include "message.h"


#include "minorGems/game/Font.h"
#include "minorGems/game/game.h"

#include "minorGems/util/stringUtils.h"


extern Font *mainFont;


extern char *serverURL;

extern int userID;



RobCheckinHousePage::RobCheckinHousePage() 
        : mWebRequest( -1 ),
          mBackpackContents( NULL ),
          mMoveList( NULL ),
          mHouseMap( NULL ),
          mSuccess( false ),
          mHomeButton( mainFont, 4, -4, translate( "returnHome" ) ),
          mStartOverButton( mainFont, 4, -4, translate( "startOver" ) ),
          mReturnToHome( false ),
          mStartOver( true ) {

    addComponent( &mHomeButton );
    mHomeButton.addActionListener( this );

    addComponent( &mStartOverButton );
    mStartOverButton.addActionListener( this );


    doublePair slotCenter = { -8, 2 };

    int numVaultRows = NUM_VAULT_SLOTS / NUM_PACK_SLOTS;
    
    int slot = 0;
    
    for( int r=0; r<numVaultRows; r++ ) {
        slotCenter.x = -8;

        for( int i=0; i<NUM_PACK_SLOTS; i++ ) {
            
            mVaultSlots[slot] = 
                new InventorySlotButton( mainFont, slotCenter.x, slotCenter.y,
                                         1 / 16.0 );
            slotCenter.x += 1.5;
        
            addComponent( mVaultSlots[slot] );
            mVaultSlots[slot]->addActionListener( this );
            mVaultSlots[slot]->setVisible( false );
            slot ++;
            }
        slotCenter.y -= 1.5;
        }



    }


        
RobCheckinHousePage::~RobCheckinHousePage() {
    if( mWebRequest != -1 ) {
        clearWebRequest( mWebRequest );
        }
    if( mBackpackContents != NULL ) {
        delete [] mBackpackContents;
        }
    if( mHouseMap != NULL ) {
        delete [] mHouseMap;
        }
    if( mMoveList != NULL ) {
        delete [] mMoveList;
        }

    for( int i=0; i<NUM_VAULT_SLOTS; i++ ) {
        delete mVaultSlots[i];
        }
    }



char RobCheckinHousePage::getReturnToHome() {
    return mReturnToHome;
    }



char RobCheckinHousePage::getStartOver() {
    return mStartOver;
    }



void RobCheckinHousePage::setBackpackContents( char *inBackpackContents ) {
    if( mBackpackContents != NULL ) {
        delete [] mBackpackContents;
        }
    mBackpackContents = stringDuplicate( inBackpackContents );
    }



void RobCheckinHousePage::setHouseMap( char *inHouseMap ) {
    if( mHouseMap != NULL ) {
        delete [] mHouseMap;
        }
    mHouseMap = stringDuplicate( inHouseMap );
    }



void RobCheckinHousePage::setMoveList( char *inMoveList ) {
    if( mMoveList != NULL ) {
        delete [] mMoveList;
        }
    mMoveList = stringDuplicate( inMoveList );
    }



void RobCheckinHousePage::setSuccess( char inSuccess ) {
    mSuccess = inSuccess;
    }



void RobCheckinHousePage::actionPerformed( GUIComponent *inTarget ) {
    if( inTarget == &mHomeButton ) {
        mReturnToHome = true;
        }
    else if( inTarget == &mStartOverButton ) {
        mStartOver = true;
        }
    }


void RobCheckinHousePage::step() {
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
                mHomeButton.setVisible( true );
                break;
            case 1: {
                char *result = getWebResult( mWebRequest );
                clearWebRequest( mWebRequest );
                mWebRequest = -1;
                     
                printf( "Web result = %s\n", result );
   
                if( strstr( result, "DENIED" ) != NULL ) {
                    mStatusError = true;
                    mStatusMessageKey = "houseCheckInFailed";
                    mHomeButton.setVisible( true );
                    }
                else {
                    // house checked in!
                    
                    SimpleVector<char *> *tokens =
                        tokenizeString( result );
                    
                    if( tokens->size() != 3 ||
                        strcmp( *( tokens->getElement( 2 ) ), "OK" ) != 0 ) {
                        mStatusError = true;
                        mStatusMessageKey = "err_badServerResponse";
                        mHomeButton.setVisible( true );                    
                        }
                    else {
                        int value;

                        sscanf( *( tokens->getElement( 0 ) ), 
                                "%d", &value );
                        

                        
                        if( mSuccess != 0 ) {
                            mStatusError = false;

                            char *robReport = 
                                autoSprintf( translate( "robSuccess" ), 
                                             value );
                            
                            setStatusDirect( robReport, false );
                            
                            delete [] robReport;

                            vaultSlotsFromString( *( tokens->getElement( 1 ) ),
                                                  mVaultSlots );
                            for( int i=0; i<NUM_VAULT_SLOTS; i++ ) {
                                if( mVaultSlots[i]->getQuantity() == 0 ) {
                                    mVaultSlots[i]->setVisible( false );
                                    }
                                else {
                                    mVaultSlots[i]->setVisible( true );
                                    }
                                }
                            
                            mHomeButton.setVisible( true );
                            }
                        else {
                            mStatusError = true;
                            mStatusMessageKey = "deathMessage";
                            mStartOverButton.setVisible( true );
                            }
                        
                        }

                    for( int i=0; i<tokens->size(); i++ ) {
                        delete [] *( tokens->getElement( i ) );
                        }
                    delete tokens;
                    }
                        
                        
                delete [] result;
                }
                break;
            }
        }
    }



        
void RobCheckinHousePage::makeActive( char inFresh ) {
    if( !inFresh ) {
        return;
        }
    
    // send back to server            
    char *ticketHash = getTicketHash();
        
            
    
    char *actionString = autoSprintf( 
        "action=end_rob_house&user_id=%d"
        "&%s"
        "&success=%d"
        "&backpack_contents=%s"
        "&move_list=%s"
        "&house_map=%s",
        userID, ticketHash, mSuccess, mBackpackContents, 
        mMoveList, mHouseMap );
    delete [] ticketHash;
            
    
    mWebRequest = startWebRequest( "POST", 
                                   serverURL, 
                                   actionString );
    
    printf( "Starting web request %s %s\n", serverURL, actionString );

    delete [] actionString;

    mReturnToHome = false;
    mStartOver = false;
    
    mStatusError = false;
    mStatusMessageKey = NULL;

    setStatusDirect( NULL, false );
    

    mHomeButton.setVisible( false );
    mStartOverButton.setVisible( false );

    for( int i=0; i<NUM_VAULT_SLOTS; i++ ) {
        mVaultSlots[i]->setVisible( false );
        }
    }


