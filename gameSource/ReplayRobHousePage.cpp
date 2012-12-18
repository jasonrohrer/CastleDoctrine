#include "ReplayRobHousePage.h"


#include "message.h"
#include "seededMusic.h"
#include "musicPlayer.h"

#include "minorGems/game/Font.h"
#include "minorGems/game/game.h"
#include "minorGems/game/drawUtils.h"

#include "minorGems/util/stringUtils.h"
#include "minorGems/util/SettingsManager.h"


extern Font *mainFont;


extern char *serverURL;

extern int userID;

extern int musicOff;


ReplayRobHousePage::ReplayRobHousePage() 
        : mWebRequest( -1 ),
          mMusicSeed( 0 ),
          mGridDisplay( 0, 0 ),
          mDoneButton( mainFont, 8, -5, translate( "doneEdit" ) ),
          mMusicToggleButton( "musicOn.tga", "musicOff.tga", 8, -3, 1/16.0 ),
          mDescription( NULL ) {

    addComponent( &mDoneButton );
    addComponent( &mGridDisplay );
    addComponent( &mMusicToggleButton );


    mDoneButton.addActionListener( this );
    mGridDisplay.addActionListener( this );
    mMusicToggleButton.addActionListener( this );

    mDoneButton.setMouseOverTip( "" );

    mMusicToggleButton.setMouseOverTip( translate( "musicOff" ) );
    mMusicToggleButton.setMouseOverTipB( translate( "musicOn" ) );

    
    doublePair slotCenter = { 7.25, 4 };

    int currentSlot = 0;
    for( int c=0; c<2; c++ ) {
        
        for( int i=0; i<NUM_PACK_SLOTS / 2; i++ ) {
            
            mPackSlots[currentSlot] = 
                new InventorySlotButton( mainFont, 
                                         slotCenter.x, slotCenter.y,
                                         1 / 32.0 );
            slotCenter.y -= 1.5;
            
            addComponent( mPackSlots[currentSlot] );
            mPackSlots[currentSlot]->addActionListener( this );
            currentSlot++;
            }
        
        // next column
        slotCenter.y = 4;
        slotCenter.x += 1.5;
        }
    }


        
ReplayRobHousePage::~ReplayRobHousePage() {
    if( mWebRequest != -1 ) {
        clearWebRequest( mWebRequest );
        }

    if( mDescription != NULL ) {
        delete [] mDescription;
        }

    for( int i=0; i<NUM_PACK_SLOTS; i++ ) {
        delete mPackSlots[i];
        }
    }



void ReplayRobHousePage::setLog( RobberyLog inLog ) {
    mGridDisplay.setHouseMap( inLog.houseMap );
    mGridDisplay.setMoveList( inLog.moveList );
    mGridDisplay.setWifeMoney( inLog.wifeMoney );
    
    mMusicSeed = inLog.musicSeed;
    
    if( mDescription != NULL ) {
        delete [] mDescription;
        }
    
    mDescription = autoSprintf( translate( "replayDescription" ),
                                inLog.robberName,
                                inLog.victimName,
                                inLog.lootValue );

    backpackSlotsFromString( inLog.backpackContents, mPackSlots );
    }



void ReplayRobHousePage::actionPerformed( GUIComponent *inTarget ) {
    if( inTarget == &mDoneButton ) {
        mDone = true;
        clearNotes();
        }
    else if( inTarget == &mMusicToggleButton ) {
        musicOff = mMusicToggleButton.getToggled();
        
        if( musicOff ) {
            setMusicLoudness( 0 );
            }
        else {
            setMusicLoudness( 1 );
            }
        SettingsManager::setSetting( "musicOff", musicOff );
        }
    else if( inTarget == &mGridDisplay ) {
        if( mGridDisplay.getSuccess() ) {
            mDone = true;
            clearNotes();
            }
        else {
            int pickedID = mGridDisplay.getToolIDJustPicked();
            
            if( pickedID != -1 ) {
                // select one of our matching backpack slots
                for( int j=0; j<NUM_PACK_SLOTS; j++ ) {
                    if( mPackSlots[j]->getObject() == pickedID ) {
                        mPackSlots[j]->setRingOn( true );
                        break;
                        }
                    }
                }
            else if( mGridDisplay.getToolJustUsed() ) {
                // clear our selected slot
                for( int j=0; j<NUM_PACK_SLOTS; j++ ) {
                    if( mPackSlots[j]->getRingOn() ) {
                        mPackSlots[j]->setObject( -1 );
                        mPackSlots[j]->setRingOn( false );
                        break;
                        }
                    }
                }
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



void ReplayRobHousePage::draw( doublePair inViewCenter, 
                               double inViewSize ) {
        
    if( mDescription != NULL ) {
        doublePair labelPos = { 0, 7 };
        
        drawMessage( mDescription, labelPos, false );
        }

    doublePair labelPos = { 8, 5.5 };
    drawMessage( "robBackpack", labelPos );
    }



        
void ReplayRobHousePage::makeActive( char inFresh ) {
    if( !inFresh ) {
        return;
        }
    
    mDone = false;
    
    setMusicFromSeed( mMusicSeed );
    
    mMusicToggleButton.setToggled( musicOff );
    }
        

