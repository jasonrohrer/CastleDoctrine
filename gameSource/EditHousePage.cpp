#include "EditHousePage.h"

#include "message.h"
#include "balance.h"


#include "minorGems/game/Font.h"
#include "minorGems/game/game.h"
#include "minorGems/game/drawUtils.h"

#include "minorGems/util/stringUtils.h"
#include "minorGems/util/SettingsManager.h"


#include "minorGems/io/file/File.h"



extern Font *mainFont;


extern int diffHighlightsOff;



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
          mJumpToTapesButton( mainFont, 8, -1.75, translate( "jumpToTapes" ) ),
          mSuicideButton( mainFont, 8, -0.5, translate( "suicide" ) ),
          mSuicideConfirmCheckbox( 8, .375, 1/16.0 ),
          mDiffHighlightToggleButton( "diffHighlightsOn.tga", 
                                      "diffHighlightsOff.tga", 
                                      8, -1.75, 1/16.0 ),
          mEyedropperStatus( 6.5, 5, 1/16.0 ),
          mBlockSuicideButton( false ),
          mGallery( mainFont, -8, 0 ),
          mNumberOfTapes( 0 ),
          mJumpToTapes( false ),
          mDone( false ),
          mDead( false ) {

    addComponent( &mDoneButton );
    addComponent( &mJumpToTapesButton );
    addComponent( &mSuicideButton );
    addComponent( &mSuicideConfirmCheckbox );
    addComponent( &mBackpackButton );
    addComponent( &mAuctionButton );
    addComponent( &mUndoButton );
    addComponent( &mGridDisplay );
    addComponent( &mObjectPicker );

    addComponent( &mDiffHighlightToggleButton );
    
    addComponent( &mEyedropperStatus );

    mDoneButton.setMouseOverTip( "" );
    mUndoButton.setMouseOverTip( translate( "undoTip" ) );
    mBackpackButton.setMouseOverTip( translate( "loadBackpackTip" ) );
    mAuctionButton.setMouseOverTip( translate( "openAuctionListTip" ) );

    mJumpToTapesButton.setMouseOverTip( translate( "jumpToTapesTip" ) );
    
    mSuicideButton.setMouseOverTip( translate( "unconfirmedSuicideTip" ) );
    mSuicideConfirmCheckbox.setMouseOverTip( 
        translate( "suicideConfirmTip" ) );
    mSuicideConfirmCheckbox.setMouseOverTipB( 
        translate( "suicideConfirmTip" ) );

    mDoneButton.addActionListener( this );
    
    mJumpToTapesButton.addActionListener( this );
    mSuicideButton.addActionListener( this );
    mSuicideConfirmCheckbox.addActionListener( this );
    mBackpackButton.addActionListener( this );
    mAuctionButton.addActionListener( this );
    mUndoButton.addActionListener( this );
    mUndoButton.setVisible( false );
    mGridDisplay.addActionListener( this );
    mObjectPicker.addActionListener( this );


    mDiffHighlightToggleButton.addActionListener( this );

    mDiffHighlightToggleButton.setMouseOverTip( 
        translate( "diffHighlightsOff" ) );
    mDiffHighlightToggleButton.setMouseOverTipB( 
        translate( "diffHighlightsOn" ) );
    
    
    
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
    mObjectPicker.setWifeName( inWifeName );
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
    
    mChangesCost = 0;
    mDiffHighlightToggleButton.setVisible( false );

    mMapStartedOutEmpty = mGridDisplay.getMapStartedOutEmpty();

    mBackpackOrVaultChanged = false;
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


void EditHousePage::setNumberOfTapes( int inNumber ) {
    mNumberOfTapes = inNumber;
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
    // always allow placement with new accounting method
    mGridDisplay.allowPlacement( true );
        
    // can't afford to place anything, house not edited yet
    // allow suicide
    mSuicideButton.setVisible(
        ! mBlockSuicideButton &&
        ! mUndoButton.isVisible() &&
        mLootValue == 0 );

    if( mSuicideButton.isVisible() ) {
        mSuicideConfirmCheckbox.setVisible( true );
        mSuicideConfirmCheckbox.setToggled( false );
        mSuicideButton.setMouseOverTip( translate( "unconfirmedSuicideTip" ) );
        }
    else {
        mSuicideConfirmCheckbox.setVisible( false );
        }

    checkIfTapesButtonVisible();
    }



void EditHousePage::checkIfDoneButtonVisible() {
    // can't click DONE if house has no goal set
    // or family blocked
    // or spent more than we have on changes to house
    mDoneButton.setVisible( 
        mGridDisplay.areMandatoriesPlaced()
        &&
        mGridDisplay.doAllFamilyObjectsHaveExitPath()
        &&
        mLootValue >= mChangesCost );
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




void EditHousePage::recomputeChangeCost() {
    mChangesCost = 0;
        
    SimpleVector<GridDiffRecord> diffList = mGridDisplay.getEditDiff();
    
    int numRecords = diffList.size();
    
    for( int i=0; i<numRecords; i++ ) {
        GridDiffRecord *r = diffList.getElement( i );
        
        mChangesCost += 
            r->placementCount *
            mObjectPicker.getPrice( r->objectID );
        }
        
    mGridDisplay.setTouchedHighlightRed( mChangesCost > mLootValue );

    mDiffHighlightToggleButton.setVisible( !mMapStartedOutEmpty && 
                                           mChangesCost > 0 );
    }


    

void EditHousePage::actionPerformed( GUIComponent *inTarget ) {
    if( inTarget == &mSuicideConfirmCheckbox ) {
        if( mSuicideConfirmCheckbox.getToggled() ) {
            mSuicideButton.setMouseOverTip( 
                translate( "suicideTip" ) );
            }
        else {
            mSuicideButton.setMouseOverTip( 
                translate( "unconfirmedSuicideTip" ) );
            }    
        }
    else if( inTarget == &mGridDisplay ) {
        int cost = 
            mObjectPicker.getPrice( mGridDisplay.getLastPlacedObject() );

        mUndoButton.setVisible( mGridDisplay.canUndo() );
        
        if( cost != -1 && ! mGridDisplay.wasLastActionPlayerMotion() ) {
            mObjectPicker.useSelectedObject();

            checkIfPlacementAllowed();
            }

        if( mGridDisplay.didLastActionChangeDiff() ) {    
            recomputeChangeCost();
            }
        
        checkIfDoneButtonVisible();

        // change to house map
        actionHappened();
        }
    else if( inTarget == &mDiffHighlightToggleButton ) {
        diffHighlightsOff = mDiffHighlightToggleButton.getToggled();
        
        SettingsManager::setSetting( "diffHighlightsOff", diffHighlightsOff );
        mGridDisplay.toggleTouchedHighlights( ! diffHighlightsOff );
        }
    else if( inTarget == &mBackpackButton ) {
        mShowLoadBackpack = true;
        }
    else if( inTarget == &mAuctionButton ) {
        mShowAuctions = true;
        }
    else if( inTarget == &mJumpToTapesButton ) {
        mJumpToTapes = true;
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
        if( mSuicideConfirmCheckbox.isVisible() && 
            ! mSuicideConfirmCheckbox.getToggled() ) {
            return;
            }
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

        mGridDisplay.undo();
        
        
        mUndoButton.setVisible( mGridDisplay.canUndo() );

        checkIfPlacementAllowed();
                
        recomputeChangeCost();
        checkIfDoneButtonVisible();

        // change to house map
        actionHappened();
        }    
    
    }





        
void EditHousePage::makeActive( char inFresh ) {
    LiveHousePage::makeActive( inFresh );
    
    if( !inFresh ) {
        return;
        }
    
    mJumpToTapes = false;
    mDone = false;
    mDead = false;
    mShowLoadBackpack = false;
    mShowAuctions = false;
    mShowGridObjectPicker = false;

    checkIfDoneButtonVisible();
    checkIfTapesButtonVisible();

    mDiffHighlightToggleButton.setToggled( diffHighlightsOff );
    mGridDisplay.toggleTouchedHighlights( ! mMapStartedOutEmpty && 
                                          ! diffHighlightsOff );
    }



void EditHousePage::step() {
    LiveHousePage::step();
    
    
    checkIfTapesButtonVisible();
    }




void EditHousePage::checkIfTapesButtonVisible() {
    // can jump to tapes as long as no editing done yet and no purchase/sale
    // done yet (so nothing will be lost when we abandon the house edit)
    // AND no background requests to server are pending
    mJumpToTapesButton.setVisible( ! areRequestsPending() && 
                                   mNumberOfTapes > 0 &&
                                   ! mUndoButton.isVisible() &&
                                   ! mBackpackOrVaultChanged );
    }




extern Font *numbersFontFixed;


void EditHousePage::draw( doublePair inViewCenter, 
                               double inViewSize ) {
        
    doublePair labelPos = { 0, 6.75 };
    
    drawMessage( "editDescription", labelPos, false );    

    
    drawBalance( mLootValue, mChangesCost );



    if( ! mGridDisplay.doAllFamilyObjectsHaveExitPath() ) {
        
        // explanation for why Done button hidden

        doublePair buttonPos = mDoneButton.getPosition();
        
        buttonPos.y += 0.5;

        drawMessage( "familyExitMessage", buttonPos, true );
        }
    
    }




void EditHousePage::keyDown( unsigned char inASCII ) {
    if( inASCII == '+' ) {
        saveWholeMapImage();
        }
    
    }




static int nextShotNumber = -1;
static char shotDirExists = false;



void EditHousePage::saveWholeMapImage() {

    File shotDir( NULL, "mapShots" );
    
    if( !shotDirExists && !shotDir.exists() ) {
        shotDir.makeDirectory();
        shotDirExists = shotDir.exists();
        }
    
    if( nextShotNumber < 1 ) {
        if( shotDir.exists() && shotDir.isDirectory() ) {
        
            int numFiles;
            File **childFiles = shotDir.getChildFiles( &numFiles );

            nextShotNumber = 1;

            char *formatString = autoSprintf( "map%%d.tga" );

            for( int i=0; i<numFiles; i++ ) {
            
                char *name = childFiles[i]->getFileName();
                
                int number;
                
                int numRead = sscanf( name, formatString, &number );
                
                if( numRead == 1 ) {
                    
                    if( number >= nextShotNumber ) {
                        nextShotNumber = number + 1;
                        }
                    }
                delete [] name;
                
                delete childFiles[i];
                }
            
            delete [] formatString;
            
            delete [] childFiles;
            }
        }
    

    if( nextShotNumber < 1 ) {
        return;
        }
    
    char *fileName = autoSprintf( "map%05d.tga", nextShotNumber );
    
    nextShotNumber++;
    

    File *file = shotDir.getChildFile( fileName );
    
    delete [] fileName;



    fileName = file->getFullFileName();
    

    delete file;


    

    int oldXOffset = mGridDisplay.getVisibleOffsetX();
    int oldYOffset = mGridDisplay.getVisibleOffsetY();
    
    mGridDisplay.setVisibleOffset( 0, 0 );

    printf( "Old offset = %d,%d, new = %d,%d\n",
            oldXOffset, oldYOffset,
            mGridDisplay.getVisibleOffsetX(),
            mGridDisplay.getVisibleOffsetY() );
    
    


    // this is in view space
    int fullMapD = mGridDisplay.getFullMapD();
    
    Image **tileImages = new Image*[ fullMapD * fullMapD ];
    
    for( int y=0; y<fullMapD; y++ ) {
        // buffer away from edge if possible to avoid edge shadows
        int viewOffsetY = y - 1;
        
        if( viewOffsetY < 0 ) {
            viewOffsetY = 0;
            }
        else if( viewOffsetY > fullMapD - HOUSE_D ) {
            viewOffsetY = fullMapD - HOUSE_D;
            }
        
        for( int x=0; x<fullMapD; x++ ) {
            int viewOffsetX = x - 1;
        
            if( viewOffsetX < 0 ) {
                viewOffsetX = 0;
                }
            else if( viewOffsetX > fullMapD - HOUSE_D ) {
                viewOffsetX = fullMapD - HOUSE_D;
                }
            printf( "Setting offset to %d,%d\n", viewOffsetX, viewOffsetY );
            mGridDisplay.setVisibleOffset( viewOffsetX, viewOffsetY );
            
            mGridDisplay.draw();

            int yDistFromViewEdge = y - viewOffsetY;
            int xDistFromViewEdge = x - viewOffsetX;

            tileImages[y * fullMapD + x] = 
                getScreenRegion( -HOUSE_D * TILE_RADIUS + 
                                 xDistFromViewEdge * TILE_RADIUS * 2,
                                 -HOUSE_D * TILE_RADIUS +
                                 yDistFromViewEdge * TILE_RADIUS * 2,
                                 2 * TILE_RADIUS,
                                 2 * TILE_RADIUS );
            if( y * fullMapD + x == 0 ) {
                //writeTGAFile( fileName, tileImages[0] );
                //return;
                }
            }
        }
    
    
    int tileD = tileImages[0]->getWidth();
    
    int fullImageD = fullMapD * tileD;

    Image *wholeImage = new Image( fullImageD, fullImageD, 3, 0 );
    
    double *channels[3];
    int c;
    for( c=0; c<3; c++ ) {
        channels[c] = wholeImage->getChannel( c );
        }
    

    for( int y=0; y<fullMapD; y++ ) {
        for( int x=0; x<fullMapD; x++ ) {
            // bottom tiles are at y=0
            int tileY = (fullMapD - y) - 1;
            Image *tile = tileImages[tileY * fullMapD + x];
            
            int xOffset = x * tileD;
            int yOffset = y * tileD;
            
            for( c=0; c<3; c++ ) {
                double *tileChannel = tile->getChannel( c );
                
                for( int ty=0; ty<tileD; ty++ ) {
                    memcpy( 
                        &( channels[c][ 
                               (yOffset + ty) * fullImageD + xOffset ] ), 
                        &( tileChannel[ 
                               ty * tileD ] ),
                        tileD * sizeof( double ) );
                    }
                }
            delete tileImages[tileY * fullMapD + x];
            }
        }
    
    delete [] tileImages;
        
    writeTGAFile( fileName, wholeImage );
    
    delete [] fileName;
    delete wholeImage;    
    
    mGridDisplay.setVisibleOffset( oldXOffset, oldYOffset );    
    }

