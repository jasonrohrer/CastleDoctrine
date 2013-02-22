#include "EditHousePage.h"

#include "message.h"


#include "minorGems/game/Font.h"
#include "minorGems/game/game.h"
#include "minorGems/game/drawUtils.h"

#include "minorGems/util/stringUtils.h"



extern Font *mainFont;





EditHousePage::EditHousePage() 
        : mStartHouseMap( NULL ),
          mVaultContents( NULL ),
          mBackpackContents( NULL ),
          mPriceList( NULL ),
          // starts empty
          mPurchaseList( stringDuplicate( "#" ) ),
          mSellList( stringDuplicate( "#" ) ),
          mObjectPicker( 8, 5 ),
          mGridDisplay( 0, 0, &mObjectPicker ),
          mDoneButton( mainFont, 8, -5, translate( "doneEdit" ) ),
          mBackpackButton( mainFont, 8, -3, translate( "loadBackpack" ) ),
          mAuctionButton( mainFont, -8, -5, translate( "openAuctionList" ) ),
          mUndoButton( mainFont, 8, -0.5, translate( "undo" ), 'z', 'Z' ),
          mSuicideButton( mainFont, 8, -0.5, translate( "doneRob" ) ),
          mBlockSuicideButton( false ),
          mGallery( mainFont, -8, 0 ),
          mDone( false ),
          mDead( false ) {

    addComponent( &mDoneButton );
    addComponent( &mSuicideButton );
    addComponent( &mBackpackButton );
    addComponent( &mAuctionButton );
    addComponent( &mUndoButton );
    addComponent( &mGridDisplay );
    addComponent( &mObjectPicker );

    mDoneButton.setMouseOverTip( "" );
    mSuicideButton.setMouseOverTip( "" );
    mUndoButton.setMouseOverTip( "" );
    mBackpackButton.setMouseOverTip( translate( "loadBackpackTip" ) );
    mAuctionButton.setMouseOverTip( translate( "openAuctionListTip" ) );

    mDoneButton.addActionListener( this );
    mSuicideButton.addActionListener( this );
    mBackpackButton.addActionListener( this );
    mAuctionButton.addActionListener( this );
    mUndoButton.addActionListener( this );
    mUndoButton.setVisible( false );
    mGridDisplay.addActionListener( this );
    mObjectPicker.addActionListener( this );

    
    
    addComponent( &mGallery );
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
    if( mSellList != NULL ) {
        delete [] mSellList;
        }
    }



void EditHousePage::setWifeName( const char *inWifeName ) {
    mGridDisplay.setWifeName( inWifeName );
    }

void EditHousePage::setSonName( const char *inSonName ) {
    mGridDisplay.setSonName( inSonName );
    }

void EditHousePage::setDaughterName( const char *inDaughterName ) {
    mGridDisplay.setDaughterName( inDaughterName );
    }


char *EditHousePage::getWifeName() {
    return mGridDisplay.getWifeName();
    }

char *EditHousePage::getSonName() {
    return mGridDisplay.getSonName();
    }

char *EditHousePage::getDaughterName() {
    return mGridDisplay.getDaughterName();
    }



void EditHousePage::setHouseMap( const char *inHouseMap ) {
    if( mStartHouseMap != NULL ) {
        delete [] mStartHouseMap;
        }
    mStartHouseMap = stringDuplicate( inHouseMap );
    
    mGridDisplay.setHouseMap( inHouseMap );

    mUndoButton.setVisible( mGridDisplay.canUndo() );

    mDoneButton.setVisible( 
        mGridDisplay.areMandatoriesPlaced()
        &&
        mGridDisplay.doAllFamilyObjectsHaveExitPath() );
    }



char *EditHousePage::getHouseMap() {
    return mGridDisplay.getHouseMap();
    }



void EditHousePage::setVaultContents( const char *inVaultContents ) {
    if( mVaultContents != NULL ) {
        delete [] mVaultContents;
        }
    mVaultContents = stringDuplicate( inVaultContents );
    }



char *EditHousePage::getVaultContents() {
    return stringDuplicate( mVaultContents );
    }



void EditHousePage::setBackpackContents( const char *inBackpackContents ) {
    if( mBackpackContents != NULL ) {
        delete [] mBackpackContents;
        }
    mBackpackContents = stringDuplicate( inBackpackContents );
    }



char *EditHousePage::getBackpackContents() {
    return stringDuplicate( mBackpackContents );
    }



void EditHousePage::setGalleryContents( const char *inGalleryContents ) {
    
    mGallery.setGalleryContents( inGalleryContents );
    }



char *EditHousePage::getGalleryContents() {
    return mGallery.getGalleryContents();
    }





char *EditHousePage::getEditList() {
    return mGridDisplay.getEditList();
    }

char *EditHousePage::getFamilyExitPaths() {
    return mGridDisplay.getFamilyExitPaths();
    }


char EditHousePage::getWifeLiving() {
    return mGridDisplay.getWifeLiving();
    }


char *EditHousePage::getPurchaseList() {
    return stringDuplicate( mPurchaseList );
    }


void EditHousePage::setPurchaseList( const char *inPurchaseList ) {
    if( mPurchaseList != NULL ) {
        delete [] mPurchaseList;
        }
    mPurchaseList = stringDuplicate( inPurchaseList );
    }




char *EditHousePage::getSellList() {
    return stringDuplicate( mSellList );
    }


void EditHousePage::setSellList( const char *inSellList ) {
    if( mSellList != NULL ) {
        delete [] mSellList;
        }
    mSellList = stringDuplicate( inSellList );
    }




void EditHousePage::setPriceList( const char *inPriceList ) {
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
    mBlockSuicideButton = false;

    checkIfPlacementAllowed();
    }



void EditHousePage::setMustSelfTest( char inMustSelfTest ) {
    mMustSelfTest = inMustSelfTest;
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

    
    // can't afford to place anything, house not edited yet
    // allow suicide
    mSuicideButton.setVisible(
        ! mBlockSuicideButton &&
        ! mUndoButton.isVisible() &&
        mLootValue == 0 );
    }



char EditHousePage::houseMapChanged() {

    char *editList = getEditList();
    
    int comp = strcmp( editList, "" );
    
    delete [] editList;

    if( comp != 0 ) {
        // some edits to report, whether or not map was actually changed
        // by edits, count it as changed
        return true;
        }
    

    if( mStartHouseMap == NULL || mMustSelfTest ) {
        return true;
        }

    char *newMap = mGridDisplay.getHouseMap();

    comp = strcmp( newMap, mStartHouseMap );

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
        mDoneButton.setVisible( 
            mGridDisplay.areMandatoriesPlaced()
            &&
            mGridDisplay.doAllFamilyObjectsHaveExitPath() );

        int cost = 
            mObjectPicker.getPrice( mGridDisplay.getLastPlacedObject() );

        mUndoButton.setVisible( mGridDisplay.canUndo() );
        
        if( cost != -1 ) {
            mLootValue -= cost;
            mObjectPicker.useSelectedObject();

            checkIfPlacementAllowed();
            }

        // change to house map
        actionHappened();
        }
    else if( inTarget == &mBackpackButton ) {
        mShowLoadBackpack = true;
        }
    else if( inTarget == &mAuctionButton ) {
        mShowAuctions = true;
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
    else if( inTarget == &mSuicideButton ) {
        mGridDisplay.resetToggledStates( 0 );

        mDead = true;
        mDone = true;
        }
    else if( inTarget == &mObjectPicker ) {
        if( mObjectPicker.shouldShowGridView() ) {
            mShowGridObjectPicker = true;
            }
        else {
            // change in picked object
            checkIfPlacementAllowed();
            }
        }
    else if( inTarget == &mUndoButton ) {
        
        mBlockSuicideButton = true;

        int cost = mGridDisplay.undo();
        
        mLootValue += cost;

        mUndoButton.setVisible( mGridDisplay.canUndo() );

        checkIfPlacementAllowed();

        mDoneButton.setVisible( 
            mGridDisplay.areMandatoriesPlaced()
            &&
            mGridDisplay.doAllFamilyObjectsHaveExitPath() );
        
        // change to house map
        actionHappened();
        }    
    
    }





        
void EditHousePage::makeActive( char inFresh ) {
    LiveHousePage::makeActive( inFresh );
    
    if( !inFresh ) {
        return;
        }
    
    mDone = false;
    mDead = false;
    mShowLoadBackpack = false;
    mShowAuctions = false;
    mShowGridObjectPicker = false;
    }
        



void EditHousePage::draw( doublePair inViewCenter, 
                               double inViewSize ) {
        
    doublePair labelPos = { 0, 6.75 };
    
    drawMessage( "editDescription", labelPos, false );    

    labelPos.x = 8;
    labelPos.y = 2;
    
    drawMessage( "editBalance", labelPos, false );
    
    labelPos.y = 1.25;

    char *balanceMessage = autoSprintf( "$%d", mLootValue );
    
    drawMessage( balanceMessage, labelPos, false );
    
    delete [] balanceMessage;

    if( ! mGridDisplay.doAllFamilyObjectsHaveExitPath() ) {
        
        // explanation for why Done button hidden

        doublePair buttonPos = mDoneButton.getPosition();

        drawMessage( "familyExitMessage", buttonPos, true );
        }
    
    }
