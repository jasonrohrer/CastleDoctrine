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
          mGalleryContents( NULL ),
          mPriceList( NULL ),
          // starts empty
          mPurchaseList( stringDuplicate( "#" ) ),
          mObjectPicker( 8, 5 ),
          mGridDisplay( 0, 0, &mObjectPicker ),
          mDoneButton( mainFont, 8, -5, translate( "doneEdit" ) ),
          mBackpackButton( mainFont, 8, -3, translate( "loadBackpack" ) ),
          mAuctionButton( mainFont, -8, -5, translate( "openAuctionList" ) ),
          mUndoButton( mainFont, 8, -1, translate( "undo" ), 'z', 'Z' ),
          mGalleryArchive( mainFont, -8, -3 ),
          mDone( false ) {

    addComponent( &mDoneButton );
    addComponent( &mBackpackButton );
    addComponent( &mAuctionButton );
    addComponent( &mUndoButton );
    addComponent( &mGridDisplay );
    addComponent( &mObjectPicker );

    mDoneButton.setMouseOverTip( "" );
    mUndoButton.setMouseOverTip( "" );
    mBackpackButton.setMouseOverTip( translate( "loadBackpackTip" ) );
    mAuctionButton.setMouseOverTip( translate( "openAuctionListTip" ) );

    mDoneButton.addActionListener( this );
    mBackpackButton.addActionListener( this );
    mAuctionButton.addActionListener( this );
    mUndoButton.addActionListener( this );
    mUndoButton.setVisible( false );
    mGridDisplay.addActionListener( this );
    mObjectPicker.addActionListener( this );

    
    doublePair slotCenter = { -8, 5 };
    
    for( int i=0; i<NUM_GALLERY_SLOTS; i++ ) {
        mGallerySlots[i] = 
            new GallerySlotButton( mainFont, slotCenter.x, slotCenter.y,
                                   1 / 16.0 );
        
        addComponent( mGallerySlots[i] );
        mGallerySlots[i]->addActionListener( this );
        mGallerySlots[i]->setVisible( false );

        slotCenter.y -= 2.5;
        }
    
    addComponent( &mGalleryArchive );
    mGalleryArchive.addActionListener( this );

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

    if( mGalleryContents != NULL ) {
        delete [] mGalleryContents;
        }

    if( mPriceList != NULL ) {
        delete [] mPriceList;
        }

    if( mPurchaseList != NULL ) {
        delete [] mPurchaseList;
        }

    for( int i=0; i<NUM_GALLERY_SLOTS; i++ ) {
        delete mGallerySlots[i];
        }

    }



void EditHousePage::setHouseMap( const char *inHouseMap ) {
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
    if( mGalleryContents != NULL ) {
        delete [] mGalleryContents;
        }
    mGalleryContents = stringDuplicate( inGalleryContents );
    
    // clear all
    for( int i=0; i<NUM_GALLERY_SLOTS; i++ ) {
        mGallerySlots[i]->setObject( -1 );
        }
    mGalleryArchive.clearObjects();
    

    if( strcmp( mGalleryContents, "#" ) != 0 ) {
        // non-empty
        
        int numParts;
        char **parts = split( mGalleryContents, "#", &numParts );

        for( int j=0; j<numParts; j++ ) {
            int id;
            sscanf( parts[j], "%d", &id );
                
            if( j < NUM_GALLERY_SLOTS ) {    
                mGallerySlots[j]->setObject( id );
                mGallerySlots[j]->setVisible( true );
                }
            else {
                mGalleryArchive.addObject( id );
                }
            
            delete [] parts[j];
            }
        delete [] parts;

        }

    }



char *EditHousePage::getGalleryContents() {
    SimpleVector<char *> parts;
    
    for( int i=0; i<NUM_GALLERY_SLOTS; i++ ) {
        int id = mGallerySlots[i]->getObject();
        
        if( id != -1 ) {
            parts.push_back( autoSprintf( "%d", id ) );
            }
        }

    if( parts.size() == 0 ) {
        // know that archive is empty too
        return stringDuplicate( "#" );
        }
    
    

    char **partsArray = parts.getElementArray();
    
    
    char *fullString = join( partsArray, parts.size(), "#" );

    for( int i=0; i<NUM_GALLERY_SLOTS; i++ ) {
        delete [] partsArray[i];
        }
    delete [] partsArray;


    char *archiveString = mGalleryArchive.getContentsString();
    
    if( strcmp( archiveString, "#" ) == 0 ) {
        delete [] archiveString;
        
        return fullString;
        }
    else {
        char *finalString = autoSprintf( "%s#%s", fullString, archiveString );
        
        delete [] fullString;
        delete [] archiveString;
        
        return finalString;
        }
    }





char *EditHousePage::getEditList() {
    return mGridDisplay.getEditList();
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
    }



char EditHousePage::houseMapChanged() {
    if( mStartHouseMap == NULL || mMustSelfTest ) {
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
        
        // change to house map
        actionHappened();
        }
    else {
        char hit = false;

        for( int i=0; i<NUM_GALLERY_SLOTS; i++ ) {
            if( inTarget == mGallerySlots[i] ) {
                int thisID = mGallerySlots[i]->getObject();
                
                if( thisID != -1 && i != 0 ) {
                    // swap with next higher slot
                    int topID = mGallerySlots[i-1]->getObject();
                    mGallerySlots[i-1]->setObject( thisID );
                    mGallerySlots[i]->setObject( topID );
                    }
                hit = true;
                break;
                }
            }

        if( !hit ) {
            
            if( inTarget == &mGalleryArchive ) {
                
                int pulledObject =
                    mGalleryArchive.swapSelectedObject( 
                        mGallerySlots[ NUM_GALLERY_SLOTS - 1 ]->getObject() );
                
                mGallerySlots[ NUM_GALLERY_SLOTS - 1 ]->setObject( 
                    pulledObject );

                hit = true;
                }
            }
        
        }
    
    
    }





        
void EditHousePage::makeActive( char inFresh ) {
    LiveHousePage::makeActive( inFresh );
    
    if( !inFresh ) {
        return;
        }
    
    mDone = false;
    mShowLoadBackpack = false;
    mShowAuctions = false;
    }
        



void EditHousePage::draw( doublePair inViewCenter, 
                               double inViewSize ) {
        
    doublePair labelPos = { 0, 6.75 };
    
    drawMessage( "editDescription", labelPos, false );
    

    if( mGallerySlots[0]->isVisible() ) {
        
        labelPos = mGallerySlots[0]->getCenter();
        labelPos.y += 1.5;
        
        drawMessage( "galleryLabel", labelPos, false );
        }
    
    
    if( mGalleryArchive.isVisible() ) {
        
        labelPos = mGalleryArchive.getCenter();
        labelPos.y += 1.5;
        
        drawMessage( "galleryArchiveLabel", labelPos, false );
        }
    

    labelPos.x = 8;
    labelPos.y = 2;
    
    drawMessage( "editBalance", labelPos, false );
    
    labelPos.y = 1.25;

    char *balanceMessage = autoSprintf( "$%d", mLootValue );
    
    drawMessage( balanceMessage, labelPos, false );
    
    delete [] balanceMessage;
    }
