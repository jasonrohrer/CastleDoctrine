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
          mBlueprintButton( mainFont, 8, -2.5, 
                            translate( "viewBlueprintShort" ) ),
          mDoneButton( mainFont, 8, -5, translate( "suicide" ) ),
          mSuicideConfirmCheckbox( 8, -4.125, 1/16.0 ),
          mMusicToggleButton( "musicOn.tga", "musicOff.tga", -8, -6, 1/16.0 ),
          mGallery( mainFont, -8, -1 ),
          mMusicSeed( 0 ),
          mDescription( NULL ),
          mDeathMessage( NULL ),
          mStartHouseMap( NULL ) {    

    addComponent( &mBlueprintButton );
    addComponent( &mDoneButton );
    addComponent( &mSuicideConfirmCheckbox );
    addComponent( &mGallery );
    addComponent( &mGridDisplay );
    addComponent( &mMusicToggleButton );

    mGallery.setAllowEdit( false );
    

    mBlueprintButton.setMouseOverTip( translate( "viewBlueprintTip" ) );

    mDoneButton.setMouseOverTip( translate( "unconfirmedSuicideTip" ) );
    mSuicideConfirmCheckbox.setMouseOverTip( 
        translate( "suicideConfirmTip" ) );
    mSuicideConfirmCheckbox.setMouseOverTipB( 
        translate( "suicideConfirmTip" ) );
    
    mBlueprintButton.addActionListener( this );
    mDoneButton.addActionListener( this );
    mSuicideConfirmCheckbox.addActionListener( this );
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

    if( mStartHouseMap != NULL ) {
        delete [] mStartHouseMap;
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

    // disabled in v9
    // can't view blueprints during self-test
    //mBlueprintButton.setVisible( inShow );
    mBlueprintButton.setVisible( false );
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
    if( mStartHouseMap != NULL ) {
        delete [] mStartHouseMap;
        }
    mStartHouseMap = stringDuplicate( inHouseMap );
    
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



char *RobHousePage::getBlueprintMap() {
    return stringDuplicate( mStartHouseMap );
    }



int RobHousePage::getVisibleOffsetX() {
    return mGridDisplay.getVisibleOffsetX();
    }



int RobHousePage::getVisibleOffsetY() {
    return mGridDisplay.getVisibleOffsetY();
    }



void RobHousePage::setBackpackContents( char *inBackpackContents ) {

    inventorySlotsFromString( inBackpackContents,
                              mPackSlots, NUM_PACK_SLOTS );
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
    if( inTarget == &mSuicideConfirmCheckbox ) {
        if( mSuicideConfirmCheckbox.getToggled() ) {
            mDoneButton.setMouseOverTip( 
                translate( "suicideTip" ) );
            }
        else {
            mDoneButton.setMouseOverTip( 
                translate( "unconfirmedSuicideTip" ) );
            }    
        }
    else if( inTarget == &mBlueprintButton ) {
        mViewBlueprint = true;
        }
    else if( inTarget == &mDoneButton ) {
        if( mSuicideConfirmCheckbox.isVisible() && 
            ! mSuicideConfirmCheckbox.getToggled() ) {
            return;
            }
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
            // spend tool from active slot in backpack
            for( int j=0; j<NUM_PACK_SLOTS; j++ ) {
                if( mPackSlots[j]->getRingOn() ) {
                    mPackSlots[j]->addToQuantity( -1 );
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
            mDoneButton.setMouseOverTip( "" );
            mSuicideConfirmCheckbox.setVisible( false );
            mBlueprintButton.setVisible( false );
            }
        }
    else if( ! mGridDisplay.getDead() ) {
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

    mViewBlueprint = false;
    
    
    if( !inFresh ) {
        return;
        }
    
    mDone = false;

    // disabled in v9
    //mBlueprintButton.setVisible( mShowBackpack );
    mBlueprintButton.setVisible( false );
    
    // back to default button text
    mDoneButton.setLabelText( translate( "suicide" ) );
    mDoneButton.setMouseOverTip( translate( "unconfirmedSuicideTip" ) );
    mSuicideConfirmCheckbox.setVisible( true );
    mSuicideConfirmCheckbox.setToggled( false );
    
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


        

