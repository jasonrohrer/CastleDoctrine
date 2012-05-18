#include "RobHousePage.h"

#include "message.h"

#include "minorGems/game/Font.h"
#include "minorGems/game/game.h"
#include "minorGems/game/drawUtils.h"

#include "minorGems/util/stringUtils.h"


#include <time.h>


extern Font *mainFont;


extern char *serverURL;

extern int userID;



RobHousePage::RobHousePage( const char *inDoneButtonKey ) 
        : mGridDisplay( 0, 0, this ),
          mDoneButton( mainFont, 8, -5, translate( inDoneButtonKey ) ),
          mDoneButtonKey( inDoneButtonKey ),
          mDescription( NULL ) {

    addComponent( &mDoneButton );
    addComponent( &mGridDisplay );

    mDoneButton.addActionListener( this );
    mGridDisplay.addActionListener( this );
    }


        
RobHousePage::~RobHousePage() {
    if( mWebRequest != -1 ) {
        clearWebRequest( mWebRequest );
        }

    if( mDescription != NULL ) {
        delete [] mDescription;
        }
    }



void RobHousePage::setHouseMap( char *inHouseMap ) {
    mGridDisplay.setHouseMap( inHouseMap );
    }



char *RobHousePage::getHouseMap() {
    return mGridDisplay.getHouseMap();
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
            mLastActionTime = time( NULL );
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
        
        drawMessage( mDescription, labelPos, false );
        }
    }

        

