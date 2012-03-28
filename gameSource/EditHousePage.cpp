#include "EditHousePage.h"

#include "message.h"


#include "minorGems/game/Font.h"
#include "minorGems/game/game.h"
#include "minorGems/game/drawUtils.h"

#include "minorGems/util/stringUtils.h"


#include <time.h>


extern Font *mainFont;





EditHousePage::EditHousePage() 
        : mStartHouseMap( NULL ),
          mGridDisplay( 0, 0 ),
          mDoneButton( mainFont, 8, -5, translate( "doneEdit" ) ) {

    addComponent( &mDoneButton );
    addComponent( &mGridDisplay );

    mDoneButton.addActionListener( this );
    mGridDisplay.addActionListener( this );
    }


        
EditHousePage::~EditHousePage() {
    if( mWebRequest != -1 ) {
        clearWebRequest( mWebRequest );
        }

    if( mStartHouseMap != NULL ) {
        delete [] mStartHouseMap;
        }
    }



void EditHousePage::setHouseMap( char *inHouseMap ) {
    if( mStartHouseMap != NULL ) {
        delete [] mStartHouseMap;
        }
    mStartHouseMap = stringDuplicate( inHouseMap );
    
    mGridDisplay.setHouseMap( inHouseMap );
    }



char *EditHousePage::getHouseMap() {
    return mGridDisplay.getHouseMap();
    }



char EditHousePage::houseMapChanged() {
    if( mStartHouseMap == NULL ) {
        return true;
        }

    char *newMap = mGridDisplay.getHouseMap();

    int comp = strcmp( newMap, mStartHouseMap );
    
    delete [] newMap;
    
    if( comp == 0 ) {
        return false;
        }
    return true;
    }



void EditHousePage::actionPerformed( GUIComponent *inTarget ) {
    if( inTarget == &mGridDisplay ) {
        // can't click DONE if house has no goal set
        mDoneButton.setVisible( mGridDisplay.isGoalSet() );

        // change to house map
        mLastActionTime = time( NULL );
        }
    else if( inTarget == &mDoneButton ) {
        mDone = true;
        }
    }





        
void EditHousePage::makeActive( char inFresh ) {
    LiveHousePage::makeActive( inFresh );
    
    if( !inFresh ) {
        return;
        }
    
    mDone = false;
    }
        



void EditHousePage::draw( doublePair inViewCenter, 
                               double inViewSize ) {
        
    doublePair labelPos = { 0, 6.75 };
    
    drawMessage( translate( "editDescription" ), labelPos, false );
    }
