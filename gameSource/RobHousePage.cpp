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
        : mGridDisplay( 0, 0 ),
          mDoneButton( mainFont, 8, -4, translate( inDoneButtonKey ) ),
          mDoneButtonKey( inDoneButtonKey ),
          mDescription( NULL ) {

    addComponent( &mDoneButton );
    addComponent( &mGridDisplay );

    mDoneButton.addActionListener( this );
    mGridDisplay.addActionListener( this );


    doublePair slotCenter = { 7.25, 4 };

    int currentSlot = 0;
    for( int c=0; c<2; c++ ) {
        
        for( int i=0; i<NUM_PACK_SLOTS / 2; i++ ) {
            
            mPackSlots[currentSlot] = 
                new InventorySlotButton( mainFont, 
                                         slotCenter.x, slotCenter.y,
                                         1 / 16.0 );
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
    }




void RobHousePage::setHouseMap( char *inHouseMap ) {
    mGridDisplay.setHouseMap( inHouseMap );
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
            actionHappened();
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
    
    doublePair labelPos = { 8, 5.5 };
    drawMessage( "robBackpack", labelPos );
    }

        

