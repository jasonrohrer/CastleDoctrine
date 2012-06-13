#include "StaleHousePage.h"

#include "minorGems/game/Font.h"
#include "minorGems/game/game.h"

#include "minorGems/util/stringUtils.h"


extern Font *mainFont;


StaleHousePage::StaleHousePage() 
        : mDoneButton( mainFont, 4, -4, translate( "doneStale" ) ),
          mDone( false ) {

    addComponent( &mDoneButton );
    mDoneButton.addActionListener( this );
    }



char StaleHousePage::getDone() {
    return mDone;
    }



void StaleHousePage::actionPerformed( GUIComponent *inTarget ) {
    if( inTarget == &mDoneButton ) {
        mDone = true;
        }
    }



void StaleHousePage::makeActive( char inFresh ) {
    if( !inFresh ) {
        return;
        }
    
    mDone = false;
    
    mStatusError = true;
    mStatusMessageKey = "houseStale";
    }


