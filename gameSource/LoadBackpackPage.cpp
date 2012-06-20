#include "LoadBackpackPage.h"

#include "message.h"


#include "minorGems/game/Font.h"
#include "minorGems/game/game.h"
#include "minorGems/game/drawUtils.h"

#include "minorGems/util/stringUtils.h"


#include <time.h>


extern Font *mainFont;





LoadBackpackPage::LoadBackpackPage() 
        : mVaultContents( NULL ),
          mBackpackContents( NULL ),
          // starts empty
          mPurchaseList( stringDuplicate( "#" ) ),
          mToolPicker( 8, 5, this, true ),
          mDoneButton( mainFont, 8, -5, translate( "doneEdit" ) ),
          mUndoButton( mainFont, 8, -1, translate( "undo" ), 'z', 'Z' ),
          mDone( false ) {

    addComponent( &mDoneButton );
    addComponent( &mUndoButton );
    addComponent( &mToolPicker );

    mDoneButton.addActionListener( this );
    mUndoButton.addActionListener( this );
    mUndoButton.setVisible( false );
    mToolPicker.addActionListener( this );
    }


        
LoadBackpackPage::~LoadBackpackPage() {
    
    if( mVaultContents != NULL ) {
        delete [] mVaultContents;
        }

    if( mBackpackContents != NULL ) {
        delete [] mBackpackContents;
        }


    if( mPurchaseList != NULL ) {
        delete [] mPurchaseList;
        }
    }



void LoadBackpackPage::setVaultContents( char *inVaultContents ) {
    if( mVaultContents != NULL ) {
        delete [] mVaultContents;
        }
    mVaultContents = stringDuplicate( inVaultContents );
    }



char *LoadBackpackPage::getVaultContents() {
    return stringDuplicate( mVaultContents );
    }



void LoadBackpackPage::setBackpackContents( char *inBackpackContents ) {
    if( mBackpackContents != NULL ) {
        delete [] mBackpackContents;
        }
    mBackpackContents = stringDuplicate( inBackpackContents );
    }



char *LoadBackpackPage::getBackpackContents() {
    return stringDuplicate( mBackpackContents );
    }




char *LoadBackpackPage::getPurchaseList() {
    return stringDuplicate( mPurchaseList );
    }


void LoadBackpackPage::setPurchaseList( char *inPurchaseList ) {
    if( mPurchaseList != NULL ) {
        delete [] mPurchaseList;
        }
    mPurchaseList = stringDuplicate( inPurchaseList );
    }



void LoadBackpackPage::setPriceList( char *inPriceList ) {
    mToolPicker.setPriceList( inPriceList );
    }



void LoadBackpackPage::setLootValue( int inLootValue ) {
    mLootValue = inLootValue;
    }





void LoadBackpackPage::actionPerformed( GUIComponent *inTarget ) {
    if( inTarget == &mDoneButton ) {
        mDone = true;
        }
    else if( inTarget == &mToolPicker ) {
        // nothing yet
        }
    else if( inTarget == &mUndoButton ) {
        // figure out how undo makes sense here
        
        }
    
    }





        
void LoadBackpackPage::makeActive( char inFresh ) {
    LiveHousePage::makeActive( inFresh );
    
    if( !inFresh ) {
        return;
        }
    
    mDone = false;
    }
        



void LoadBackpackPage::draw( doublePair inViewCenter, 
                             double inViewSize ) {
        
    doublePair labelPos = { 0, 6.75 };
    
    drawMessage( translate( "loadBackpackDescription" ), labelPos, false );
    


    labelPos.x = 8;
    labelPos.y = 2;
    
    drawMessage( translate( "editBalance" ), labelPos, false );
    
    labelPos.y = 1.25;

    char *balanceMessage = autoSprintf( "$%d", mLootValue );
    
    drawMessage( balanceMessage, labelPos, false );
    
    delete [] balanceMessage;
    }
