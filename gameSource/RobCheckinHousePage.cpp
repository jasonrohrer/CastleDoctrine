#include "RobCheckinHousePage.h"
#include "ticketHash.h"
#include "message.h"


#include "minorGems/game/Font.h"
#include "minorGems/game/game.h"

#include "minorGems/util/stringUtils.h"


extern Font *mainFont;


extern char *serverURL;

extern int userID;


#define SLOT_Y 5.5


RobCheckinHousePage::RobCheckinHousePage() 
        : mWebRequest( -1 ),
          mBackpackContents( NULL ),
          mMoveList( NULL ),
          mHouseMap( NULL ),
          mWifePresent( false ),
          mWifeRobbed( false ),
          mAnyFamilyKilled( false ),
          mSuccess( false ),
          mHomeButton( mainFont, 4, -4, translate( "returnHome" ) ),
          mStartOverButton( mainFont, 4, -4, translate( "startOver" ) ),
          mGalleryDisplay( mainFont, 0, 0 ),
          mReturnToHome( false ),
          mStartOver( true ) {

    addComponent( &mHomeButton );
    mHomeButton.addActionListener( this );

    addComponent( &mStartOverButton );
    mStartOverButton.addActionListener( this );

    addComponent( &mGalleryDisplay );
    

    mHomeButton.setMouseOverTip( "" );


    doublePair slotCenter = { -5.25, SLOT_Y };

    int numVaultRows = NUM_VAULT_SLOTS / NUM_PACK_SLOTS;
    
    int slot = 0;
    
    for( int r=0; r<numVaultRows; r++ ) {
        slotCenter.x = -5.25;

        for( int i=0; i<NUM_PACK_SLOTS; i++ ) {
            
            mVaultSlots[slot] = 
                new InventorySlotButton( mainFont, slotCenter.x, slotCenter.y,
                                         1 / 32.0 );
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



void RobCheckinHousePage::setWifePresent( char inPresent ) {
    mWifePresent = inPresent;
    }



void RobCheckinHousePage::setWifeRobbed( char inRobbed ) {
    mWifeRobbed = inRobbed;
    }



void RobCheckinHousePage::setAnyFamilyKilled( char inKilled ) {
    mAnyFamilyKilled = inKilled;
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
                    
                    if( tokens->size() != 4 ||
                        strcmp( *( tokens->getElement( 3 ) ), "OK" ) != 0 ) {
                        mStatusError = true;
                        mStatusMessageKey = "err_badServerResponse";
                        mHomeButton.setVisible( true );                    
                        }
                    else {
                        sscanf( *( tokens->getElement( 0 ) ), 
                                "%d", &mMoneyTaken );
                        
                        if( mSuccess != 0 ) {
                            mStatusError = false;
                            
                            vaultSlotsFromString( *( tokens->getElement( 1 ) ),
                                                  mVaultSlots );
                            
                            int numVisible = 0;
                            
                            for( int i=0; i<NUM_VAULT_SLOTS; i++ ) {
                                if( mVaultSlots[i]->getQuantity() == 0 ) {
                                    mVaultSlots[i]->setVisible( false );
                                    }
                                else {
                                    mVaultSlots[i]->setVisible( true );
                                    numVisible ++;
                                    }
                                }
                            
                            
                            if( numVisible < NUM_PACK_SLOTS && 
                                numVisible > 0 ) {
                                // less than one full row
                                // recenter them
                                
                                double length = (numVisible - 1) * 1.5;
                                
                                doublePair slotCenter = { -(length/2), 
                                                          SLOT_Y };
                                
                                for( int i=0; i<numVisible; i++ ) {
                                    mVaultSlots[i]->setPosition( 
                                        slotCenter.x,
                                        slotCenter.y );
                                    
                                    slotCenter.x += 1.5;
                                    }
                                }
                            
                            char *galleryString = *( tokens->getElement( 2 ) );

                            
                            if( strcmp( galleryString, "#" ) != 0 ) {
                                
                                int numParts;
                                char **parts = split( galleryString, 
                                                      "#", &numParts );

                                for( int j=0; j<numParts; j++ ) {
                                    int id;
                                    sscanf( parts[j], "%d", &id );
                                    
                                    mGalleryDisplay.addObject( id );
                                    
                                    delete [] parts[j];
                                    }
                                delete [] parts;
                                }
                            
                            if( numVisible > 0 ) {
                                // put gallery widget below last row of
                                // slots
                                
                                doublePair lastRowCenter =
                                    mVaultSlots[numVisible-1]->getCenter();
                            
                                mGalleryDisplay.setPosition( 
                                    0, 
                                    lastRowCenter.y - 2.5 );
                                }
                            else {
                                // put right below robbery result text
                                mGalleryDisplay.setPosition( 0, 4.5 );
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
        "&wife_present=%d"
        "&wife_robbed=%d"
        "&any_family_kiled=%d"
        "&backpack_contents=%s"
        "&move_list=%s"
        "&house_map=%s",
        userID, ticketHash, 
        mSuccess, mWifePresent, mWifeRobbed, mAnyFamilyKilled,
        mBackpackContents, 
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



    // reset slot positioning and visibility

    doublePair slotCenter = { -5.25, SLOT_Y };

    int numVaultRows = 
        NUM_VAULT_SLOTS / NUM_PACK_SLOTS;
    
    int slot = 0;
                                
    for( int r=0; r<numVaultRows; r++ ) {
        slotCenter.x = -5.25;
 
        for( int i=0; i<NUM_PACK_SLOTS; i++ ) {
                                        
            mVaultSlots[slot]->setPosition(
                slotCenter.x, slotCenter.y );
            
            mVaultSlots[slot]->setVisible( false );
            
            slotCenter.x += 1.5;
            slot ++;
            }
        slotCenter.y -= 1.5;
        }

    mGalleryDisplay.clearObjects();
    }




void RobCheckinHousePage::draw( doublePair inViewCenter, 
                                double inViewSize ) {
    
    if( ! mHomeButton.isVisible() || mStatusError ) {
        return;
        }
    

    const char *robReportKey;
    
    int numGalleryItems = mGalleryDisplay.getObjectCount();

    if( ! mVaultSlots[0]->isVisible() && numGalleryItems == 0 ) {
        robReportKey = "robSuccess";
        }
    else {
        int totalQuantity = 0;
        for( int i=0; i<NUM_VAULT_SLOTS; i++ ) {
            if( mVaultSlots[i]->isVisible() ) {
                totalQuantity += mVaultSlots[i]->getQuantity();
                }
            else {
                break;
                }
            }

        totalQuantity += numGalleryItems;
        
        if( totalQuantity > 1 ) {
            robReportKey = "robStuffSuccess";
            }
        else {
            robReportKey = "robOneItemSuccess";
            }
        }
    
    
    char *robReport = autoSprintf( translate( robReportKey ), mMoneyTaken );
        
    
    doublePair labelPos = { 0, 6.75 };
    
    drawMessage( robReport, labelPos, false );
    
    delete [] robReport;
    }


