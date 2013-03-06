#include "RobHousePage.h"

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


RobHousePage::RobHousePage() 
        : mShowBackpack( true ),
          mGridDisplay( 0, 0 ),
          mDoneButton( mainFont, 8, -4, translate( "suicide" ) ),
          mMusicToggleButton( "musicOn.tga", "musicOff.tga", -8, -6, 1/16.0 ),
          mGallery( mainFont, -8, -1 ),
          mMusicSeed( 0 ),
          mDescription( NULL ),
          mDeathMessage( NULL ) {

    addComponent( &mDoneButton );
    addComponent( &mGallery );
    addComponent( &mGridDisplay );
    addComponent( &mMusicToggleButton );

    mGallery.setAllowEdit( false );
    

    mDoneButton.setMouseOverTip( translate( "suicideTip" ) );
    mDoneButton.addActionListener( this );
    mGridDisplay.addActionListener( this );
    mMusicToggleButton.addActionListener( this );

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


        
RobHousePage::~RobHousePage() {
    
    if( mDescription != NULL ) {
        delete [] mDescription;
        }
    if( mDeathMessage != NULL ) {
        delete [] mDeathMessage;
        }

    for( int i=0; i<NUM_PACK_SLOTS; i++ ) {
        delete mPackSlots[i];
        }
    }



void RobHousePage::showBackpack( char inShow ) {
    for( int i=0; i<NUM_PACK_SLOTS; i++ ) {
        mPackSlots[i]->setVisible( inShow );
        }
    mShowBackpack = inShow;
    }



void RobHousePage::setWifeName( const char *inWifeName ) {
    mGridDisplay.setWifeName( inWifeName );
    }

void RobHousePage::setSonName( const char *inSonName ) {
    mGridDisplay.setSonName( inSonName );
    }

void RobHousePage::setDaughterName( const char *inDaughterName ) {
    mGridDisplay.setDaughterName( inDaughterName );
    }



void RobHousePage::setHouseMap( char *inHouseMap ) {
    mGridDisplay.setHouseMap( inHouseMap );
    
    mGallery.instantFadeOut( mGridDisplay.getAboutToLeave() );

    if( mDeathMessage != NULL ) {
        delete [] mDeathMessage;
        mDeathMessage = NULL;
        }
    }



char *RobHousePage::getHouseMap() {
    return mGridDisplay.getHouseMap();
    }



void RobHousePage::setBackpackContents( char *inBackpackContents ) {

    backpackSlotsFromString( inBackpackContents,
                             mPackSlots );
    }



char *RobHousePage::getBackpackContents() {

    return stringFromInventorySlots( mPackSlots, NUM_PACK_SLOTS );
    }


void RobHousePage::setGalleryContents( char *inGalleryContents ) {

    mGallery.setGalleryContents( inGalleryContents );
    }


void RobHousePage::setWifeMoney( int inMoney ) {
    mGridDisplay.setWifeMoney( inMoney );
    }


void RobHousePage::setMusicSeed( int inMusicSeed ) {
    mMusicSeed = inMusicSeed;
    }



void RobHousePage::setDescription( const char *inDescription ) {
    if( mDescription != NULL ) {
        delete [] mDescription;
        }
    mDescription = stringDuplicate( inDescription );
    }



void RobHousePage::actionPerformed( GUIComponent *inTarget ) {
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
        mGallery.fadeOut( mGridDisplay.getAboutToLeave() );

        // activity on house map
            
        if( mGridDisplay.getToolJustUsed() ) {    
            // empty active slot in backpack
            for( int j=0; j<NUM_PACK_SLOTS; j++ ) {
                if( mPackSlots[j]->getRingOn() ) {
                    mPackSlots[j]->setObject( -1 );
                    mPackSlots[j]->setRingOn( false );
                    break;
                    }
                }
            }
        else {
            // auto turn-off active slot
            // player moved without using it
            for( int j=0; j<NUM_PACK_SLOTS; j++ ) {
                if( mPackSlots[j]->getRingOn() ) {
                    mPackSlots[j]->setRingOn( false );
                    break;
                    }
                }
            }

        actionHappened();

        
        if( mGridDisplay.getSuccess() ) {
            mDone = true;
            clearNotes();
            }
        else if( mGridDisplay.getDead() ) {
            if( mDeathMessage != NULL ) {
                delete [] mDeathMessage;
                }
            
            mDeathMessage = 
                autoSprintf( 
                    "%s  %s",
                    translate( "killedBy" ),
                    getObjectDescription( 
                        mGridDisplay.getDeathSourceID(),
                        mGridDisplay.getDeathSourceState() ) );
            
            mDoneButton.setLabelText( translate( "doneRobDead" ) );
            mDoneButton.setToolTip( "" );
            }
        }
    else {
        // check backpack slots
        for( int i=0; i<NUM_PACK_SLOTS; i++ ) {
            if( inTarget == mPackSlots[i] ) {

                mGridDisplay.stopUsingTool();

                char oldOn = mPackSlots[i]->getRingOn();
                
                // turn all other slots off first (only one ring at a time)
                for( int j=0; j<NUM_PACK_SLOTS; j++ ) {
                    mPackSlots[j]->setRingOn( false );
                    }
                
                if( mPackSlots[i]->getObject() != -1 ) {
                    mPackSlots[i]->setRingOn( !oldOn );

                    if( !oldOn ) {
                        mGridDisplay.startUsingTool( 
                            mPackSlots[i]->getObject() );
                        }
                    }
                else {
                    mPackSlots[i]->setRingOn( false );
                    }
                
                break;
                }
            }
        
        
        }
    
    }


        
void RobHousePage::makeActive( char inFresh ) {
    LiveHousePage::makeActive( inFresh );
    
    
    if( !inFresh ) {
        return;
        }
    
    mDone = false;

    // back to default button text
    mDoneButton.setLabelText( translate( "suicide" ) );
    mDoneButton.setMouseOverTip( translate( "suicideTip" ) );

    // no tool tip
    setToolTip( NULL );



    
    setMusicFromSeed( mMusicSeed );

    mMusicToggleButton.setToggled( musicOff );
    }




void RobHousePage::step() {
    LiveHousePage::step();
    mGridDisplay.setLeaveCanBeShown( ! mGallery.isVisible() );
    }




void RobHousePage::draw( doublePair inViewCenter, 
                         double inViewSize ) {
     
    if( mDescription != NULL ) {
        doublePair labelPos = { 0, 6.75 };
        
        if( strstr( mDescription, "##" ) != NULL ) {
            // two lines, move up a bit
            labelPos.y = 7;
            }

        drawMessage( mDescription, labelPos );
        }

    if( mDeathMessage != NULL ) {
        doublePair labelPos = { 0, -6.25 };
        
        drawMessage( mDeathMessage, labelPos, true );
        }

    
    if( mShowBackpack ) {
        doublePair labelPos = { 8, 5.5 };
        drawMessage( "robBackpack", labelPos );
        }
    }


        

