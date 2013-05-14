#include "ViewBlueprintPage.h"

#include "message.h"
#include "balance.h"


#include "minorGems/game/Font.h"
#include "minorGems/game/game.h"
#include "minorGems/game/drawUtils.h"

#include "minorGems/util/stringUtils.h"
#include "minorGems/util/SettingsManager.h"



extern Font *mainFont;


extern int diffHighlightsOff;



ViewBlueprintPage::ViewBlueprintPage() 
        : mGridDisplay( 0, 0 ),
          mDoneButton( mainFont, 8, -5, translate( "doneEdit" ) ),
          mDone( false ),
          mDescription( NULL ) {

    addComponent( &mDoneButton );
    addComponent( &mGridDisplay );

    mDoneButton.addActionListener( this );
    }


        
ViewBlueprintPage::~ViewBlueprintPage() {
    if( mDescription != NULL ) {
        delete [] mDescription;
        }
    }


void ViewBlueprintPage::setHouseMap( const char *inHouseMap ) {
    mGridDisplay.setHouseMap( inHouseMap );
    }


void ViewBlueprintPage::setDescription( const char *inDescription ) {
    if( mDescription != NULL ) {
        delete [] mDescription;
        }
    mDescription = stringDuplicate( inDescription );
    }

    

void ViewBlueprintPage::actionPerformed( GUIComponent *inTarget ) {
    if( inTarget == &mDoneButton ) {
        mDone = true;
        }
    }





        
void ViewBlueprintPage::makeActive( char inFresh ) {
    if( !inFresh ) {
        return;
        }
    mDone = false;
    }
        

extern Font *numbersFontFixed;


void ViewBlueprintPage::draw( doublePair inViewCenter, 
                              double inViewSize ) {
    
    if( mDescription != NULL ) {
        doublePair labelPos = { 0, 6.75 };
        
        if( strstr( mDescription, "##" ) != NULL ) {
            // two lines, move up a bit
            labelPos.y = 7;
            }

        drawMessage( mDescription, labelPos );
        }
    }
