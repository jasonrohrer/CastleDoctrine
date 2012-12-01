#include "RobHousePage.h"

#include "message.h"

#include "minorGems/game/Font.h"
#include "minorGems/game/game.h"
#include "minorGems/game/drawUtils.h"

#include "minorGems/util/stringUtils.h"




extern Font *mainFont;


extern char *serverURL;

extern int userID;



RobHousePage::RobHousePage( const char *inDoneButtonKey ) 
        : mShowBackpack( true ),
          mGridDisplay( 0, 0 ),
          mDoneButton( mainFont, 8, -4, translate( inDoneButtonKey ) ),
          mGallery( mainFont, -8, -1 ),
          mMusicSeed( 0 ),
          mDoneButtonKey( inDoneButtonKey ),
          mDescription( NULL ) {

    addComponent( &mDoneButton );
    addComponent( &mGallery );
    addComponent( &mGridDisplay );


    mGallery.setAllowEdit( false );
    

    mDoneButton.setMouseOverTip( "" );
    mDoneButton.addActionListener( this );
    mGridDisplay.addActionListener( this );


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




void RobHousePage::setHouseMap( char *inHouseMap ) {
    mGridDisplay.setHouseMap( inHouseMap );
    
    mGallery.instantFadeOut( mGridDisplay.getAboutToLeave() );
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
        }
    else if( inTarget == &mGridDisplay ) {
        mGallery.fadeOut( mGridDisplay.getAboutToLeave() );
        
        if( mGridDisplay.getSuccess() ) {
            mDone = true;
            }
        else if( mGridDisplay.getDead() ) {
            char *deathMessage = 
                autoSprintf( 
                    "%s  %s",
                    translate( "killedBy" ),
                    getObjectDescription( 
                        mGridDisplay.getDeathSourceID(),
                        mGridDisplay.getDeathSourceState() ) );
            
            setToolTipDirect( deathMessage );
            delete [] deathMessage;

            mDoneButton.setLabelText( translate( "doneRobDead" ) );
            }
        else {
            
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
            }        
        }
    else {
        // check backpack slots
        for( int i=0; i<NUM_PACK_SLOTS; i++ ) {
            if( inTarget == mPackSlots[i] ) {
                
                char oldOn = mPackSlots[i]->getRingOn();
                
                // turn all other slots off first (only one ring at a time)
                for( int j=0; j<NUM_PACK_SLOTS; j++ ) {
                    
                    if( mPackSlots[j]->getRingOn() ) {
                        mGridDisplay.stopUsingTool( 
                            mPackSlots[j]->getObject() );
                        }
                    
                    mPackSlots[j]->setRingOn( false );
                    }
                
                if( mPackSlots[i]->getObject() != -1 ) {
                    mPackSlots[i]->setRingOn( !oldOn );

                    if( oldOn ) {
                        mGridDisplay.stopUsingTool( 
                            mPackSlots[i]->getObject() );
                        }
                    else {
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
    mDoneButton.setLabelText( translate( mDoneButtonKey ) );

    // no tool tip
    setToolTipDirect( NULL );
    }



void RobHousePage::draw( doublePair inViewCenter, 
                               double inViewSize ) {
     
    if( mDescription != NULL ) {
        doublePair labelPos = { 0, 6.75 };
        
        drawMessage( mDescription, labelPos );
        }
    
    if( mShowBackpack ) {
        doublePair labelPos = { 8, 5.5 };
        drawMessage( "robBackpack", labelPos );
        }
    }


        

