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
          mVaultContents( NULL ),
          mBackpackContents( NULL ),
          mPriceList( NULL ),
          // starts empty
          mPurchaseList( stringDuplicate( "#" ) ),
          mObjectPicker( 8, 5 ),
          mGridDisplay( 0, 0, &mObjectPicker ),
          mDoneButton( mainFont, 8, -5, translate( "doneEdit" ) ),
          mBackpackButton( mainFont, 8, -3, translate( "loadBackpack" ) ),
          mUndoButton( mainFont, 8, -1, translate( "undo" ), 'z', 'Z' ),
          mDone( false ) {

    addComponent( &mDoneButton );
    addComponent( &mBackpackButton );
    addComponent( &mUndoButton );
    addComponent( &mGridDisplay );
    addComponent( &mObjectPicker );

    mDoneButton.addActionListener( this );
    mBackpackButton.addActionListener( this );
    mUndoButton.addActionListener( this );
    mUndoButton.setVisible( false );
    mGridDisplay.addActionListener( this );
    mObjectPicker.addActionListener( this );
    }


        
EditHousePage::~EditHousePage() {
    
    if( mStartHouseMap != NULL ) {
        delete [] mStartHouseMap;
        }

    if( mVaultContents != NULL ) {
        delete [] mVaultContents;
        }

    if( mBackpackContents != NULL ) {
        delete [] mBackpackContents;
        }

    if( mPriceList != NULL ) {
        delete [] mPriceList;
        }

    if( mPurchaseList != NULL ) {
        delete [] mPurchaseList;
        }
    }



void EditHousePage::setHouseMap( char *inHouseMap ) {
    if( mStartHouseMap != NULL ) {
        delete [] mStartHouseMap;
        }
    mStartHouseMap = stringDuplicate( inHouseMap );
    
    mGridDisplay.setHouseMap( inHouseMap );

    mUndoButton.setVisible( mGridDisplay.canUndo() );
    }



char *EditHousePage::getHouseMap() {
    return mGridDisplay.getHouseMap();
    }



void EditHousePage::setVaultContents( char *inVaultContents ) {
    if( mVaultContents != NULL ) {
        delete [] mVaultContents;
        }
    mVaultContents = stringDuplicate( inVaultContents );
    }



char *EditHousePage::getVaultContents() {
    return stringDuplicate( mVaultContents );
    }



void EditHousePage::setBackpackContents( char *inBackpackContents ) {
    if( mBackpackContents != NULL ) {
        delete [] mBackpackContents;
        }
    mBackpackContents = stringDuplicate( inBackpackContents );
    }



char *EditHousePage::getBackpackContents() {
    return stringDuplicate( mBackpackContents );
    }





char *EditHousePage::getEditList() {
    return mGridDisplay.getEditList();
    }


char *EditHousePage::getPurchaseList() {
    return stringDuplicate( mPurchaseList );
    }


void EditHousePage::setPurchaseList( char *inPurchaseList ) {
    if( mPurchaseList != NULL ) {
        delete [] mPurchaseList;
        }
    mPurchaseList = stringDuplicate( inPurchaseList );
    }




void EditHousePage::setPriceList( char *inPriceList ) {
    if( mPriceList != NULL ) {
        delete [] mPriceList;
        }
    mPriceList = stringDuplicate( inPriceList );

    mObjectPicker.setPriceList( inPriceList );
    }



char *EditHousePage::getPriceList() {
    return stringDuplicate( mPriceList );
    }



void EditHousePage::setLootValue( int inLootValue ) {
    mLootValue = inLootValue;
    checkIfPlacementAllowed();
    }



void EditHousePage::checkIfPlacementAllowed() {
    int placementCost = 
        mObjectPicker.getPrice( mObjectPicker.getSelectedObject() );
    
    if( placementCost == -1 ) {
        mGridDisplay.allowPlacement( true );
        }
    else if( mLootValue >= placementCost ) {
        mGridDisplay.allowPlacement( true );
        }
    else {
        // not enough money
        mGridDisplay.allowPlacement( false );
        }
    }



char EditHousePage::houseMapChanged() {
    if( mStartHouseMap == NULL ) {
        return true;
        }

    char *newMap = mGridDisplay.getHouseMap();

    int comp = strcmp( newMap, mStartHouseMap );

    if( comp != 0 ) {
        
        printf( "House maps differ.  Old:\n%s\n\nNew:\n%s\n\n", mStartHouseMap,
                newMap );
        }
    
    
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

        int cost = 
            mObjectPicker.getPrice( mGridDisplay.getLastPlacedObject() );
        
        if( cost != -1 ) {
            mLootValue -= cost;
            checkIfPlacementAllowed();
            }
        mUndoButton.setVisible( mGridDisplay.canUndo() );

        // change to house map
        mLastActionTime = time( NULL );
        }
    else if( inTarget == &mBackpackButton ) {
        mShowLoadBackpack = true;
        }
    else if( inTarget == &mDoneButton ) {
        
        // Reset any states
        // that were toggled by the last robber.

        // We show the house-as-robbed view to the owner once,
        // until they perform their first complete edit, and THEN toggle
        // everything back.

        mGridDisplay.resetToggledStates( 0 );


        mDone = true;
        }
    else if( inTarget == &mObjectPicker ) {
        // change in picked object
        checkIfPlacementAllowed();
        }
    else if( inTarget == &mUndoButton ) {
        
        int cost = mGridDisplay.undo();
        
        mLootValue += cost;
        
        checkIfPlacementAllowed();

        mUndoButton.setVisible( mGridDisplay.canUndo() );

        mDoneButton.setVisible( mGridDisplay.isGoalSet() );
        }
    
    }





        
void EditHousePage::makeActive( char inFresh ) {
    LiveHousePage::makeActive( inFresh );
    
    if( !inFresh ) {
        return;
        }
    
    mDone = false;
    mShowLoadBackpack = false;
    }
        



void EditHousePage::draw( doublePair inViewCenter, 
                               double inViewSize ) {
        
    doublePair labelPos = { 0, 6.75 };
    
    drawMessage( translate( "editDescription" ), labelPos, false );
    


    labelPos.x = 8;
    labelPos.y = 2;
    
    drawMessage( translate( "editBalance" ), labelPos, false );
    
    labelPos.y = 1.25;

    char *balanceMessage = autoSprintf( "$%d", mLootValue );
    
    drawMessage( balanceMessage, labelPos, false );
    
    delete [] balanceMessage;
    }
