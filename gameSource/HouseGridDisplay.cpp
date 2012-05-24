#include "HouseGridDisplay.h"

#include "FastBoxBlurFilter.h"


#include "minorGems/util/stringUtils.h"
#include "minorGems/game/game.h"
#include "minorGems/game/gameGraphics.h"
#include "minorGems/game/drawUtils.h"

#include "minorGems/graphics/openGL/KeyboardHandlerGL.h"


#include <math.h>


#include "houseObjects.h"




#include "minorGems/util/random/CustomRandomSource.h"

extern CustomRandomSource randSource;


char HouseGridDisplay::sInstanceCount = 0;

char HouseGridDisplay::sNoiseTileBankPopulated = false;

SpriteHandle HouseGridDisplay::sNoiseTileBank[ NUM_NOISE_TILES ];

int *HouseGridDisplay::sHouseMapNoiseTileIndices = NULL;


SpriteHandle HouseGridDisplay::sDropShadowSprite = NULL;



HouseGridDisplay::HouseGridDisplay( double inX, double inY,
                                    GamePage *inParentPage,
                                    HouseObjectPicker *inPicker )
        : PageComponent( inX, inY ),
          mParentPage( inParentPage ),
          mPicker( inPicker ),
          mHouseMap( NULL ), 
          mHouseMapIDs( NULL ),
          mHouseMapCellStates( NULL ),
          mHouseMapMobileIDs( NULL ),
          mHouseMapMobileCellStates( NULL ),
          mHouseMapNoiseTileIndices( NULL ),
          mSubMapOffsetX( 0 ),
          mSubMapOffsetY( 0 ),
          mHouseSubMapIDs( new int[ HOUSE_D * HOUSE_D ] ),
          mHouseSubMapCellStates( new int[ HOUSE_D * HOUSE_D ] ),
          mHighlightIndex( -1 ), mTileRadius( 0.4375 ),
          mGoalSet( false ),
          mWallShadowSprite( NULL ),
          mAllowPlacement( true ),
          mLastPlacedObject( 0 ) {


    if( !sNoiseTileBankPopulated ) {
        

        unsigned char noisePixels[ 16 * 16 ];
    
        for( int i=0; i<NUM_NOISE_TILES; i++ ) {
            for( int p=0; p<16*16; p++ ) {
        
                // edges of tiles are transparent
                int x = p % 16;
                int y = p / 16;
            
                if( x == 0 || x == 15 ||
                    y == 0 || y == 15 ) {
                    noisePixels[p] = 0;
                    }
                else {
                    noisePixels[p] = randSource.getRandomBoundedInt( 0, 255 );
                    }
            
                }
        
            sNoiseTileBank[i] = fillSpriteAlphaOnly( noisePixels, 16, 16 );
            }


        int dropW = 32;
        int dropH = 32;
        
        unsigned char *dropAlpha = new unsigned char[ dropW * dropH ];
        
        // for blur filter
        int *touchedPixels = new int[ dropW * dropH ];
        int numTouched = 0;

        memset( dropAlpha, 0, dropW * dropH );

        int centerX = dropW / 2;
        int centerY = dropH / 2;
        
        for( int y=0; y<dropH; y++ ) {
            for( int x=0; x<dropW; x++ ) {
                
                int dY = y - centerY;
                int dX = x - centerX;
                
                double distance = sqrt( dY * dY + dX * dX );
                
                int i = y * dropW + x;

                if( distance < 4 ) {
                    dropAlpha[ i ] = 255;
                    }

                if( y > 1 && y < dropH - 1 
                    && 
                    x > 1 && x < dropW - 1 ) {
                    // away from border
                    touchedPixels[ numTouched ] = i;
                    numTouched++;
                    }
                
                }
            }

        FastBoxBlurFilter blur;
        
        
        
        
        for( int i=0; i<20; i++ ) {
            
            blur.applySubRegion( dropAlpha,
                                 touchedPixels,
                                 numTouched,
                                 dropW, dropH );
            }
        
        delete [] touchedPixels;
        

        sDropShadowSprite = fillSpriteAlphaOnly( dropAlpha,
                                                 dropW, 
                                                 dropH );
        delete [] dropAlpha;
        
        sNoiseTileBankPopulated = true;
        }
    
    sInstanceCount++;
    }



HouseGridDisplay::~HouseGridDisplay() {
    if( mHouseMap != NULL ) {
        delete [] mHouseMap;
        }
    if( mHouseMapIDs != NULL ) {
        delete [] mHouseMapIDs;
        }
    if( mHouseMapCellStates != NULL ) {
        delete [] mHouseMapCellStates;
        }

    if( mHouseMapMobileIDs != NULL ) {
        delete [] mHouseMapMobileIDs;
        }
    if( mHouseMapMobileCellStates != NULL ) {
        delete [] mHouseMapMobileCellStates;
        }

    if( mHouseMapNoiseTileIndices != NULL ) {
        delete [] mHouseMapNoiseTileIndices;
        }
    

    delete [] mHouseSubMapIDs;
    delete [] mHouseSubMapCellStates;

    if( mWallShadowSprite != NULL ) {
        freeSprite( mWallShadowSprite );
        mWallShadowSprite = NULL;
        }
    
    sInstanceCount--;
    
    if( sInstanceCount == 0 && sNoiseTileBankPopulated ) {
        
        for( int i=0; i<NUM_NOISE_TILES; i++ ) {
            freeSprite( sNoiseTileBank[i] );
            }
        sNoiseTileBankPopulated = false;

        if( sHouseMapNoiseTileIndices != NULL ) {
            delete [] sHouseMapNoiseTileIndices;
            sHouseMapNoiseTileIndices = NULL;
            }

        freeSprite( sDropShadowSprite );
        }
    }



void HouseGridDisplay::setHouseMap( char *inHouseMap ) {
    if( mHouseMap != NULL ) {
        delete [] mHouseMap;
        }
    mHouseMap = stringDuplicate( inHouseMap );

    int numTokens;
    char **tokens = split( mHouseMap, "#", &numTokens );
    
    if( mHouseMapIDs != NULL ) {
        delete [] mHouseMapIDs;
        }

    if( mHouseMapCellStates != NULL ) {
        delete [] mHouseMapCellStates;
        }

    if( mHouseMapMobileIDs != NULL ) {
        delete [] mHouseMapMobileIDs;
        }

    if( mHouseMapMobileCellStates != NULL ) {
        delete [] mHouseMapMobileCellStates;
        }

    if( mHouseMapNoiseTileIndices != NULL ) {
        delete [] mHouseMapNoiseTileIndices;
        }
    
    mNumMapSpots = numTokens;
    mFullMapD = (int)sqrt( mNumMapSpots );
    
    mHouseMapIDs = new int[ mNumMapSpots ];
    mHouseMapCellStates = new int[ mNumMapSpots ];
    mHouseMapMobileIDs = new int[ mNumMapSpots ];
    mHouseMapMobileCellStates = new int[ mNumMapSpots ];
    mHouseMapNoiseTileIndices = new int[ mNumMapSpots ];
    
    for( int i=0; i<mNumMapSpots; i++ ) {
        
        mHouseMapIDs[i] = 0;
        mHouseMapMobileIDs[i] = 0;
        mHouseMapCellStates[i] = 0;
        mHouseMapMobileCellStates[i] = 0;
        

        int numObjects;
        char **objectStrings = split( tokens[i], ",", &numObjects );

        // ignore all but two objects
        // second object is mobile object, if present

        for( int j=0; j<numObjects; j++ ) {
            
            int id, state;
                    
            int numRead = sscanf( objectStrings[j], "%d:%d", 
                                  &id, 
                                  &state );
        
            if( numRead < 2 ) {
                // no ":value" present after ID
                // use default
                state = 0;
        
                if( numRead == 0 ) {
                    // reading ID failed?
                    
                    // default
                    id = 0;
                    }
                }
        
            if( id == GOAL_ID ) {
                mGoalIndex = i;
                mGoalSet = true;
                }

            if( isPropertySet( id, state, mobile ) ) {
                mHouseMapMobileIDs[i] = id;
                mHouseMapMobileCellStates[i] = state;
                }
            else {
                // non-mobile
                mHouseMapIDs[i] = id;
                mHouseMapCellStates[i] = state;
                }
            delete [] objectStrings[j];
            }
        delete [] objectStrings;
        

        delete [] tokens[i];
        
        if( sHouseMapNoiseTileIndices == NULL ) {
            
            mHouseMapNoiseTileIndices[i] = 
                randSource.getRandomBoundedInt( 0, NUM_NOISE_TILES - 1 );
            }
        else {
            mHouseMapNoiseTileIndices[i] = sHouseMapNoiseTileIndices[i];
            }
        }
    
    delete [] tokens;


    if( sHouseMapNoiseTileIndices == NULL ) {
        sHouseMapNoiseTileIndices = new int[ mNumMapSpots ];

        memcpy( sHouseMapNoiseTileIndices, mHouseMapNoiseTileIndices,
                mNumMapSpots * sizeof( int ) );
        }


    // center vertically, far left
    setVisibleOffset( 0, ( mFullMapD - HOUSE_D ) / 2 );

    mStartIndex = mFullMapD * ( mFullMapD / 2 );
    
    mRobberIndex = mStartIndex;
    mRobberState = 0;
    mRobberOrientation = 1;
    
    mEditHistory.deleteAll();
    }



void HouseGridDisplay::resetToggledStatesInternal( int inTargetState ) {
    for( int i=0; i<mNumMapSpots; i++ ) {
        
        if( ! isPropertySet( mHouseMapIDs[i], mHouseMapCellStates[i],
                             stuck ) ) {
            
    
            mHouseMapCellStates[i] = inTargetState;
            }

        // same for mobile objects
        if( mHouseMapMobileIDs[i] != 0 
            &&
            ! isPropertySet( mHouseMapMobileIDs[i], 
                             mHouseMapMobileCellStates[i],
                             stuck ) ) {
            
            mHouseMapMobileCellStates[i] = inTargetState;
            }
        
        // leave other states alone
        // example:  walls that were burned down during a previous, successful
        // robbery, but not repaired by owner yet
        }
    }



void HouseGridDisplay::resetToggledStates( int inTargetState ) {
    resetToggledStatesInternal( inTargetState );
    
    copyAllIntoSubCells();

    recomputeWallShadows();
    }



char *HouseGridDisplay::getHouseMap() {
    if( mHouseMap == NULL ) {
        return NULL;
        }
    else {
        


        char **parts = new char*[ mNumMapSpots ];
        

        for( int i=0; i<mNumMapSpots; i++ ) {
            char *mobilePart = NULL;
            char *nonMobilePart = NULL;
            
            
            if( mHouseMapCellStates[i] != 0 && mHouseMapCellStates[i] != 1 ) {
                const char *stuckFlag = "";
                
                if( isPropertySet( mHouseMapIDs[i], mHouseMapCellStates[i],
                                   stuck ) ) {
                    stuckFlag = "!";
                    }

                // not default state, include state
                nonMobilePart = autoSprintf( "%d:%d%s", 
                                             mHouseMapIDs[i],
                                             mHouseMapCellStates[i],
                                             stuckFlag );
                }
            else {
                // one of two default states, skip including it
                nonMobilePart = autoSprintf( "%d", mHouseMapIDs[i] );
                }

            if( mHouseMapMobileIDs[i] != 0 ) {
                // mobile object present
                
                if( mHouseMapMobileCellStates[i] != 0 && 
                    mHouseMapMobileCellStates[i] != 1 ) {
                    
                    const char *stuckFlag = "";
                    
                    if( isPropertySet( mHouseMapMobileIDs[i], 
                                       mHouseMapMobileCellStates[i],
                                       stuck ) ) {
                        stuckFlag = "!";
                        }


                    // not default state, include state
                    mobilePart = 
                        autoSprintf( "%d:%d%s", 
                                     mHouseMapMobileIDs[i],
                                     mHouseMapMobileCellStates[i],
                                     stuckFlag );
                    }
                else {
                    // one of two default states, skip including it
                    mobilePart = autoSprintf( "%d", mHouseMapMobileIDs[i] );
                    }

                }

            if( mobilePart == NULL ) {
                parts[i] = nonMobilePart;
                }
            else {
                if( mHouseMapIDs[i] == 0 && 
                    ( mHouseMapCellStates[i] == 0 || 
                      mHouseMapCellStates[i] == 1 )  ) {
                    // only mobile part present, above empty floor

                    parts[i] = mobilePart;
                    }
                else {
                    // both mobile and non-mobile

                    // comma-separated parts
                    parts[i] = autoSprintf( "%s,%s", 
                                            nonMobilePart, mobilePart );
                    delete [] nonMobilePart;
                    delete [] mobilePart;
                    }
                }
            }
        
        delete [] mHouseMap;
        
        mHouseMap = join( parts, mNumMapSpots, "#" );
        
        for( int i=0; i<mNumMapSpots; i++ ) {
            delete [] parts[i];
            }
        delete [] parts;
        

        return stringDuplicate( mHouseMap );
        }
    }



char *HouseGridDisplay::getEditList() {
    int numEdits = mEditHistory.size();

    if( numEdits == 0 ) {
        return stringDuplicate( "" );
        }
    
    
    SimpleVector<char*> listAccum;
    
    for( int i=0; i<numEdits; i++ ) {
        GridChangeRecord *r = mEditHistory.getElement( i );
        
        char *editString = autoSprintf( "%d@%d", r->newID, r->fullIndex );
    
        listAccum.push_back( editString );
        }
    
    char **listStrings = listAccum.getElementArray();
    
    char *editList = join( listStrings, numEdits, "#" );

    for( int i=0; i<numEdits; i++ ) {
        delete [] listStrings[i];
        }
    delete [] listStrings;

    return editList;
    }



char HouseGridDisplay::isGoalSet() {
    return mGoalSet;
    }



void HouseGridDisplay::allowPlacement( char inAllow ) {
    mAllowPlacement = inAllow;
    }



int HouseGridDisplay::getLastPlacedObject() {
    return mLastPlacedObject;
    }

    

void HouseGridDisplay::step() {
    }



int HouseGridDisplay::getTileIndex( double inX, double inY ) {

    double relX = inX;
    double relY = inY;
    
    int x = lrint(
        ( relX + 
          ( HOUSE_D * mTileRadius - mTileRadius ) ) / ( 2 * mTileRadius )
        );
    
    int y = lrint( 
        ( relY + 
          ( HOUSE_D * mTileRadius - mTileRadius ) ) / ( 2 * mTileRadius )
        );
    
    if( x >= 0 && x < HOUSE_D 
        &&
        y >= 0 && y < HOUSE_D ) {
        
        return y * HOUSE_D + x;
        }
    else {
        return -1;
        }    
    }



doublePair HouseGridDisplay::getTilePos( int inIndex ) {
    
    int x = inIndex % HOUSE_D;
    int y = inIndex / HOUSE_D;
    


    doublePair tilePos = {  x * 2 * mTileRadius - 
                              ( HOUSE_D * mTileRadius - mTileRadius ), 
                            y * 2 * mTileRadius - 
                              ( HOUSE_D * mTileRadius - mTileRadius ) };

    return tilePos;
    }



doublePair HouseGridDisplay::getTilePosFull( int inFullIndex ) {

    int fullX = inFullIndex % mFullMapD;
    int fullY = inFullIndex / mFullMapD;
    
    

    int xOffsetFromZero = fullX - mSubMapOffsetX;
    int yOffsetFromZero = fullY - mSubMapOffsetY;
    
    
    doublePair tilePos = getTilePos( 0 );
    
    tilePos.x += xOffsetFromZero * 2 * mTileRadius;
    tilePos.y += yOffsetFromZero * 2 * mTileRadius;
    

    return tilePos;
    }



int HouseGridDisplay::getTileNeighbor( int inFullIndex, int inNeighbor ) {
    
    int fullY = inFullIndex / mFullMapD;
    int fullX = inFullIndex % mFullMapD;
    
    int dX[4] = { 0, 1, 0, -1 };
    int dY[4] = { 1, 0, -1, 0 };

    int nY = fullY + dY[ inNeighbor ];

    int nX = fullX + dX[ inNeighbor ];
    
    
    if( nY < 0 || nY >= mFullMapD
        ||
        nX < 0 || nX >= mFullMapD ) {
        
        // out of bounds, empty floor
        return 0;
        }

    return mHouseMapIDs[ nY * mFullMapD + nX ];
    }




int HouseGridDisplay::getTileNeighborStructural( int inFullIndex, 
                                                 int inNeighbor ) {
    int fullY = inFullIndex / mFullMapD;
    int fullX = inFullIndex % mFullMapD;
    
    int dX[4] = { 0, 1, 0, -1 };
    int dY[4] = { 1, 0, -1, 0 };

    int nY = fullY + dY[ inNeighbor ];

    int nX = fullX + dX[ inNeighbor ];
    
    
    if( nY < 0 || nY >= mFullMapD
        ||
        nX < 0 || nX >= mFullMapD ) {
        
        // out of bounds, empty floor
        return false;
        }

    int nIndex = nY * mFullMapD + nX;

    return isPropertySet( mHouseMapIDs[ nIndex ],
                          mHouseMapCellStates[ nIndex ], 
                          structural );
    }



int HouseGridDisplay::getOrientationIndex( int inFullIndex, 
                                           int inTileID, int inTileState ) {
    int numOrientations = 0;
    
    int orientationIndex = 0;
    
    numOrientations = getNumOrientations( inTileID, 0 );            
            
                
    if( numOrientations == 16 ) {
        // full binary LBRT flags based on neighbors of same type 
                
        int neighborsEqual[4] = { 0, 0, 0, 0 };

        for( int n=0; n<4; n++ ) {
            if( getTileNeighbor( inFullIndex, n ) == inTileID ) {
                neighborsEqual[n] = 1;
                }
            }                
                
        orientationIndex = 
            neighborsEqual[3] << 3 |
            neighborsEqual[2] << 2 |
            neighborsEqual[1] << 1 |
            neighborsEqual[0];
        }
    else if( numOrientations == 4 ) {
                
        int numBlockedNeighbors = 0;

        char neighborsBlocked[4] = { false, false, false, false };
                
        int oneBlockedIndex = 0;

        for( int n=0; n<4; n++ ) {
            if( getTileNeighborStructural( inFullIndex, n ) ) {
                numBlockedNeighbors ++;
                        
                neighborsBlocked[n] = true;
                        
                oneBlockedIndex = n;
                }
            }
                
        if( numBlockedNeighbors == 0 || numBlockedNeighbors == 4 ) {
            // default to left-facing when none/all are blocked
            orientationIndex = 3;
            }
        else if( numBlockedNeighbors == 1 ) {
            // face away from only blockage
            switch( oneBlockedIndex ) {
                case 0:
                    orientationIndex = 2;
                    break;
                case 1:
                    orientationIndex = 3;
                    break;
                case 2:
                    orientationIndex = 0;
                    break;
                case 3:
                    orientationIndex = 1;
                    break;
                }                            
            }
        else {
            // blocked on multiple sides
            // face whatever direction is open
                    
            for( int n=0; n<4; n++ ) {
                if( !neighborsBlocked[n] ) {
                    orientationIndex = n;
                    }
                }
            }



        if( isPropertySet( inTileID, inTileState, playerSeeking ) ) {
            int x = inFullIndex % mFullMapD;
            int y = inFullIndex / mFullMapD;
            
            int robberX = mRobberIndex % mFullMapD;
            int robberY = mRobberIndex / mFullMapD;
            
            int dX = robberX - x;
            int dY = robberY - y;
            
            if( dX == 0 && dY == 0 ) {
                // leave environment-based orientation
                }
            else {
                if( abs( dX ) > abs( dY ) ) {
                    // x facing
                
                    if( dX < 0 ) {
                        orientationIndex = 3;
                        }
                    else {
                        orientationIndex = 1;
                        }
                    }
                else {
                    // y facing

                    if( dY < 0 ) {
                        orientationIndex = 2;
                        }
                    else {
                        orientationIndex = 0;
                        }
                    }
                }
            }


        }
    else if( numOrientations == 2 ) {
                
        if( getTileNeighborStructural( inFullIndex, 0 ) && 
            getTileNeighborStructural( inFullIndex, 2 ) ) {
            // blocked on top and bottom
                
            // vertical orientation
            orientationIndex = 0;
            }
        else if( getTileNeighborStructural( inFullIndex, 1 ) && 
                 getTileNeighborStructural( inFullIndex, 3 ) ) {
            /// blocked on left and right
            // horizontal 
            orientationIndex = 1;
            }
        else if( getTileNeighborStructural( inFullIndex, 0 ) || 
                 getTileNeighborStructural( inFullIndex, 2 ) ) {
            // top OR bottom block
            
            // vertical orientation
            orientationIndex = 0;
            }
        else if( getTileNeighborStructural( inFullIndex, 1 ) || 
                 getTileNeighborStructural( inFullIndex, 3 ) ) {
            /// blocked on left OR right
            // horizontal 
            orientationIndex = 1;
            }
        else {
            // default when not blocked at all, horizontal
            orientationIndex = 1;
            }    
        }
    else if( numOrientations == 1 ) {
        orientationIndex = 0;
        }

    return orientationIndex;
    }




void HouseGridDisplay::drawDropShadow( doublePair inPosition ) {
    setDrawColor( 0, 0, 0, 0.75 );
    
    toggleLinearMagFilter( true );

    drawSprite( sDropShadowSprite, inPosition, 1.0/16.0 );
    
    toggleLinearMagFilter( false );
    }



void HouseGridDisplay::drawRobber( doublePair inPosition ) {
    // first drop shadow
    drawDropShadow( inPosition );
    
    setDrawColor( 1, 1, 1, 1 );
    
    doublePair robberPos = inPosition;
    
    SpriteHandle sprite = 
        getObjectSprite( PLAYER_ID, 
                         mRobberOrientation, 
                         mRobberState );
    
    drawSprite( sprite, inPosition, 1.0/16.0 );
    }





void HouseGridDisplay::drawTiles( char inBeneathShadowsOnly ) {
    for( int y=HOUSE_D-1; y>=0; y-- ) {
        for( int x=0; x<HOUSE_D; x++ ) {

            int i = y * HOUSE_D + x;
            int fullI = subToFull( i );

            int houseTile = mHouseSubMapIDs[i];
            int houseTileState = mHouseSubMapCellStates[i];
            
            char aboveShadows = 
                isSubMapPropertySet( i, structural ) ||
                isSubMapPropertySet( i, shadowMaking );
                        
            
            
            doublePair tilePos = getTilePos( i );
 

            if( inBeneathShadowsOnly && aboveShadows ) {
                // skip this blocking tile

                if( mHighlightIndex != i ) {
                    // nothing left to draw, if no highlight is here
                    i++;
                    continue;
                    }
                }
            else if( ! inBeneathShadowsOnly && ! aboveShadows ) {
                // skip this non-blocking tile

                if( mHighlightIndex != i && fullI != mRobberIndex 
                    && mHouseMapMobileIDs[ fullI ] == 0 ) {
                    // nothing left to draw, if no highlight is here
                    i++;
                    continue;
                    }
                }
            

            int orientationIndex = getOrientationIndex( fullI, houseTile,
                                                        houseTileState );



            // draw "behind" parts of structural tiles before drawing
            // a robber or mobile object in this row
            if( !inBeneathShadowsOnly && aboveShadows 
                && houseTile != 0 ) {
                
                if( isBehindSpritePresent( houseTile, 
                                           houseTileState ) ) {
                    
                    setDrawColor( 1, 1, 1, 1 );
                    
                    
                    SpriteHandle sprite = 
                        getObjectSpriteBehind( houseTile, 
                                               orientationIndex, 
                                               houseTileState );
                
                    drawSprite( sprite, tilePos, 1.0/16.0 );
                    }
                }
            

            char robberDrawn = false;

            // mobile objects above shadows, behind structural tiles in 
            // current row only
            if( ! inBeneathShadowsOnly ) {
                
                if( mHouseMapMobileIDs[fullI] != 0 ) {
                    // mobile object here
                    
                    

                    int mobID = mHouseMapMobileIDs[fullI];
                    int mobState = mHouseMapMobileCellStates[fullI];

                    if( isPropertySet( mobID, mobState, onTopOfPlayer ) ) {
                        
                        if( mRobberIndex == fullI ) {
                            // draw robber under this mobile
                            drawRobber( tilePos );
                            robberDrawn = true;
                            }
                        }
                    

                    int mobOrientation = getOrientationIndex( fullI, mobID,
                                                              mobState );
                    
                    // first drop shadow
                    drawDropShadow( tilePos );

                    setDrawColor( 1, 1, 1, 1 );
                
                    SpriteHandle sprite = 
                        getObjectSprite( mobID, 
                                         mobOrientation, 
                                         mobState );
                
                    drawSprite( sprite, tilePos, 1.0/16.0 );
                    }
                }

            // same for robber (if not already drawn under a mobile)
            if( !inBeneathShadowsOnly && mRobberIndex == fullI
                &&
                ! robberDrawn ) {

                drawRobber( tilePos );
                }



            if( inBeneathShadowsOnly && 
                ! aboveShadows && 
                houseTile != 0 ) {
                
                // now draw tile itself, on top of floor

                if( isSubMapPropertySet( i, mobile ) ) {
                    // drop shadow
                    drawDropShadow( tilePos );
                    }
                

                setDrawColor( 1, 1, 1, 1 );
                
                SpriteHandle sprite = getObjectSprite( houseTile, 
                                                       orientationIndex, 
                                                       houseTileState );
                
                drawSprite( sprite, tilePos, 1.0/16.0 );
                }
            else if( !inBeneathShadowsOnly && aboveShadows 
                     && houseTile != 0 ) {
                // now draw blocking objects on top of floor
                setDrawColor( 1, 1, 1, 1 );
                

                SpriteHandle sprite = getObjectSprite( houseTile, 
                                                       orientationIndex, 
                                                       houseTileState );
                
                drawSprite( sprite, tilePos, 1.0/16.0 );
                }

            
            
            

            
            // no highlight over start, robber, or permanents
            int highlightPick = 0;
            
            if( mHighlightIndex == i &&
                fullI != mStartIndex &&
                fullI != mRobberIndex &&
                ! isSubMapPropertySet( i, permanent ) ) {
                
                if( mPicker != NULL ) {
                    highlightPick = mPicker->getSelectedObject();
                    }
                
                char highlightAboveShadows = 
                    isPropertySet( highlightPick, 0, structural ) ||
                    isPropertySet( highlightPick, 0, shadowMaking );

                if( highlightAboveShadows && inBeneathShadowsOnly 
                    ||
                    ! highlightAboveShadows && ! inBeneathShadowsOnly ) {
                    
                    // only draw each highlight once (along with other objects
                    // that share its shadow-casting status)
                    
                    highlightPick = 0;
                    }
                }


            if( highlightPick != 0 ) {
                    
                if( !mGoalSet ) {
                    // ghost of goal for placement
                    setDrawColor( 1, 1, 1, 0.35 );

                    int ghostOrientation = getOrientationIndex( fullI, GOAL_ID,
                                                                0 );
                    
                    SpriteHandle sprite = getObjectSprite( GOAL_ID, 
                                                           ghostOrientation, 
                                                           0 );
                
                    drawSprite( sprite, tilePos, 1.0/16.0 );
                    }
                else if( houseTile == highlightPick || houseTile == GOAL_ID ) {
                    // darken existing tile to imply removal on click
                    setDrawColor( 0, 0, 0, 0.35 );

                    SpriteHandle sprite = getObjectSprite( houseTile, 
                                                           orientationIndex, 
                                                           0 );
                
                    drawSprite( sprite, tilePos, 1.0/16.0 );
                    }
                else if( houseTile != highlightPick ) {
                    setDrawColor( 1, 1, 1, 0.35 );
                
                    int ghostOrientation = getOrientationIndex( fullI, 
                                                                highlightPick, 
                                                                0 );

                    SpriteHandle sprite = getObjectSprite( highlightPick, 
                                                           ghostOrientation,
                                                           0 );
                    
                    drawSprite( sprite, tilePos, 1.0/16.0 );
                    }
                }

            i++;
            }
        }
    }






void HouseGridDisplay::draw() {
    if( mHouseMap == NULL ) {
        return;
        }

    
    // draw border
    doublePair center = { 0, 0 };
    
    setDrawColor( 1, 1, 1, 1 );
    drawSquare( center, HOUSE_D * mTileRadius + .125 );

    
    enableScissor( -HOUSE_D * mTileRadius,
                   -HOUSE_D * mTileRadius,
                   2 * ( HOUSE_D * mTileRadius ),
                   2 * ( HOUSE_D * mTileRadius ) );

    // draw full grid of floor under everything
    SpriteHandle sprite = getObjectSprite( 0, 0, 0 );
    setDrawColor( 1, 1, 1, 1 );
    for( int y=HOUSE_D-1; y>=0; y-- ) {
        for( int x=0; x<HOUSE_D; x++ ) {
            drawSprite( sprite, getTilePos( y * HOUSE_D + x ), 1.0/16.0 );
            }
        }
    

    
    // draw house parts that are under shadows (non-structural parts)
    drawTiles( true );

    
    // draw start under shadows
    int startSubIndex = fullToSub( mStartIndex );
    if( startSubIndex != -1 ) {
        setDrawColor( 1, 1, 1, 1 );

        SpriteHandle sprite = getObjectSprite( START_ID, 
                                               0, 
                                               0 );
        drawSprite( sprite, getTilePos( startSubIndex ), 1.0/16.0 );
        }


    
    setDrawColor( 0, 0, 0, 0.75 );

    toggleLinearMagFilter( true );
    // draw wall shadows over floor
    drawSprite( mWallShadowSprite, center, 
                1.0 * 2 * mTileRadius / 8.0 );
    toggleLinearMagFilter( false );

    



    // draw structural tiles above shadows
    drawTiles( false );



    // finally, walk along bottom edge and draw one last row of tiles, if 
    // present (this "completes" bottom visible row of tiles by drawing
    // overhangs from tiles that are south of them)

    if( mSubMapOffsetY > 0 ) {
        
        int y = mSubMapOffsetY - 1;
        
        for( int x=0; x<HOUSE_D; x++ ) {    
        
            int fullI = y * mFullMapD + mSubMapOffsetX + x;
                
            int houseTile = mHouseMapIDs[ fullI ];
            int houseTileState = mHouseMapCellStates[ fullI ];

            int orientationIndex = getOrientationIndex( fullI, houseTile,
                                                        houseTileState );

            doublePair tilePos = getTilePosFull( fullI );

            setDrawColor( 1, 1, 1, 1 );
                

            SpriteHandle sprite = getObjectSprite( houseTile, 
                                                   orientationIndex, 
                                                   houseTileState );
            
            drawSprite( sprite, tilePos, 1.0/16.0 );
            }
        }



    
    int i = 0;
    for( int y=0; y<HOUSE_D; y++ ) {
        for( int x=0; x<HOUSE_D; x++ ) {
            int fullI = subToFull( i );

            doublePair tilePos = getTilePos( i );

            setDrawColor( 1, 1, 1, 0.0625 );
            drawSprite( sNoiseTileBank[ mHouseMapNoiseTileIndices[fullI] ],
                        tilePos, 1.0/16.0 );
            
            i++;
            }
        }
            

    /*
    if( mGoalSet ) {    
        int goalSubIndex = fullToSub( mGoalIndex );
        
        if( goalSubIndex != -1 ) {    
            setDrawColor( 1, 1, 0, 1 );
            drawSquare( getTilePos( goalSubIndex ), 0.75 * mTileRadius );
            }
        }
    */
    

    

    disableScissor();
    }



void HouseGridDisplay::pointerOver( float inX, float inY ) {
    mHighlightIndex = getTileIndex( inX, inY );

    if( mHighlightIndex != -1 && mParentPage != NULL ) {

        int fullI = subToFull( mHighlightIndex );
        
        
        const char *nonMobileDescription; 

        if( fullI == mStartIndex ) {
            nonMobileDescription = translate( "startTileDescription" );
            }
        else { 
            nonMobileDescription = 
                getObjectDescription( 
                    mHouseSubMapIDs[ mHighlightIndex ],
                    mHouseSubMapCellStates[ mHighlightIndex ] );
            }
        

        if( mHouseMapMobileIDs[ fullI ] != 0 ) {
            
            const char *mobileDescription = 
                getObjectDescription( 
                    mHouseMapMobileIDs[ fullI ],
                    mHouseMapMobileCellStates[ fullI ] );
        
            char *tip = autoSprintf( "%s  /  %s",
                                     nonMobileDescription, mobileDescription );
            
            mParentPage->setToolTipDirect( tip );
            
            delete [] tip;
            }
        else {
            mParentPage->setToolTipDirect( (char*)nonMobileDescription );
            }
        
        }
    }



void HouseGridDisplay::pointerMove( float inX, float inY ) {
    pointerOver( inX, inY );
    }

void HouseGridDisplay::pointerUp( float inX, float inY ) {
    pointerOver( inX, inY );

    int index = getTileIndex( inX, inY );
    
    mHighlightIndex = index;
    
    if( index != -1 && 
        mPointerDownIndex != -1 &&
        mDraggedAway ) {
    
        // pointer dropped on grid after being dragged somewhere else
        int fullIndex = subToFull( index );
        
        
        if( !mGoalSet && 
            fullIndex != mStartIndex &&
            fullIndex != mRobberIndex ) {
            // goal dragged/dropped here
            mHouseSubMapIDs[ index ] = GOAL_ID;
            mGoalIndex = fullIndex;
            mGoalSet = true;
            mLastPlacedObject = GOAL_ID;
            
            logEdit( fullIndex, GOAL_ID );

            // changes reset state
            mHouseSubMapCellStates[ index ] = 0;

            // clear mobile objects
            mHouseMapMobileIDs[ fullIndex ] = 0;
            mHouseMapMobileCellStates[ fullIndex ] = 0;

            copySubCellBack( index );
            fireActionPerformed( this );
            }

        }
    
    }



void HouseGridDisplay::pointerDrag( float inX, float inY ) {
    pointerOver( inX, inY );
    int index = getTileIndex( inX, inY );

    mHighlightIndex = index;

    if( index != -1 && 
        mPointerDownIndex != -1 &&
        index != mPointerDownIndex  ) {

        mDraggedAway = true;

        int fullIndex = subToFull( index );

        if( fullIndex != mStartIndex &&
            fullIndex != mGoalIndex &&
            fullIndex != mRobberIndex &&
            mPointerDownObjectID != -1 ) {
            
            int old = mHouseSubMapIDs[ index ];

            if( mPlaceOnDrag && 
                old != mPointerDownObjectID ) {
                
                // drag-place
                if( mAllowPlacement ) {
                    mHouseSubMapIDs[ index ] = mPointerDownObjectID;
                    mLastPlacedObject = mPointerDownObjectID;

                    logEdit( fullIndex, mPointerDownObjectID );
                    
                    // changes reset state
                    mHouseSubMapCellStates[ index ] = 0;

                    // clear mobile objects
                    mHouseMapMobileIDs[ fullIndex ] = 0;
                    mHouseMapMobileCellStates[ fullIndex ] = 0;
                    
                    copySubCellBack( index );
                    fireActionPerformed( this );
                    }
                }
            else if( !mPlaceOnDrag && old == mPointerDownObjectID ) {
                // drag-erase of like-objects
                mHouseSubMapIDs[ index ] = 0;
                mLastPlacedObject = 0;

                logEdit( fullIndex, 0 );

                // changes reset state
                mHouseSubMapCellStates[ index ] = 0;

                // clear mobile objects
                mHouseMapMobileIDs[ fullIndex ] = 0;
                mHouseMapMobileCellStates[ fullIndex ] = 0;

                copySubCellBack( index );
                fireActionPerformed( this );
                }
            
            
            }

        }
    }



void HouseGridDisplay::pointerDown( float inX, float inY ) {
    int index = getTileIndex( inX, inY );
    
    mPointerDownIndex = index;
    mDraggedAway = false;
    mPointerDownObjectID = -1;
    
    if( index == -1 ) {
        return;
        }
    
    if( isSubMapPropertySet( index, permanent ) ) {
        // ignore mouse activity on permanent tiles
        return;
        }

    int fullIndex = subToFull( index );
    printf( "pointer down at full index %d (%d,%d)\n", fullIndex,
            index % HOUSE_D, index / HOUSE_D );
    
    if( fullIndex == mRobberIndex ) {
        // don't allow clicks on current robber position
        return;
        }
    

    
    

    if( !mGoalSet && fullIndex != mStartIndex ) {
        // goal set here
        mHouseSubMapIDs[ index ] = GOAL_ID;
        mGoalIndex = fullIndex;
        mGoalSet = true;
        mLastPlacedObject = GOAL_ID;
        
        logEdit( fullIndex, GOAL_ID );

        // changes reset state
        mHouseSubMapCellStates[ index ] = 0;

        // clear mobile objects
        mHouseMapMobileIDs[ fullIndex ] = 0;
        mHouseMapMobileCellStates[ fullIndex ] = 0;

        copySubCellBack( index );
        fireActionPerformed( this );
        }
    else if( fullIndex != mStartIndex && fullIndex != mGoalIndex ) {
    
        if( mPicker != NULL ) {
            
        
            int old = mHouseSubMapIDs[ index ];

            int picked = mPicker->getSelectedObject();
            
        
            if( old != picked ) {
                // place mode (or replace mode)
                if( mAllowPlacement ) {
                    mHouseSubMapIDs[ index ] = picked;
                    mLastPlacedObject = picked;
                    mPointerDownObjectID = picked;
                    mPlaceOnDrag = true;

                    logEdit( fullIndex, picked );
                    
                    // changes reset state
                    mHouseSubMapCellStates[ index ] = 0;

                    // clear mobile objects
                    mHouseMapMobileIDs[ fullIndex ] = 0;
                    mHouseMapMobileCellStates[ fullIndex ] = 0;

                    copySubCellBack( index );
                    fireActionPerformed( this );
                    }
                }
            else {
                // erase mode
                mHouseSubMapIDs[ index ] = 0;
                mLastPlacedObject = 0;
                
                // only allow erase of this object ID on drag
                mPointerDownObjectID = picked;
                mPlaceOnDrag = false;
                
                logEdit( fullIndex, 0 );

                // changes reset state
                mHouseSubMapCellStates[ index ] = 0;

                // clear mobile objects
                mHouseMapMobileIDs[ fullIndex ] = 0;
                mHouseMapMobileCellStates[ fullIndex ] = 0;

                copySubCellBack( index );
                fireActionPerformed( this );
                }
            }
        }
    else if( mGoalSet && fullIndex == mGoalIndex ) {
        // goal moving!
        mHouseSubMapIDs[ index ] = 0;
        mGoalSet = false;
        mLastPlacedObject = 0;

        logEdit( fullIndex, 0 );

        // changes reset state
        mHouseSubMapCellStates[ index ] = 0;

        // clear mobile objects
        mHouseMapMobileIDs[ fullIndex ] = 0;
        mHouseMapMobileCellStates[ fullIndex ] = 0;

        copySubCellBack( index );
        fireActionPerformed( this );
        }
    
    pointerOver( inX, inY );
    }



// arrow key movement
void HouseGridDisplay::specialKeyDown( int inKeyCode ) {
    
    int oldX = mRobberIndex % mFullMapD;
    int oldY = mRobberIndex / mFullMapD;
    
    
    int newX = oldX;
    int newY = oldY;
    

    if( inKeyCode == MG_KEY_LEFT ) {
        if( newX > 0 ) {
            newX--;
            }
        else if( mRobberIndex == mStartIndex ) {
            robberTriedToLeave();
            }
        }
    else if( inKeyCode == MG_KEY_RIGHT ) {
        if( newX < mFullMapD - 1 ) {
            newX++;
            }
        }
    else if( inKeyCode == MG_KEY_DOWN ) {
        if( newY > 0 ) {
            newY--;
            }
        }
    else if( inKeyCode == MG_KEY_UP ) {
        if( newY < mFullMapD - 1 ) {
            newY++;
            }
        }
    
    
    
    int newRobberIndex = newY * mFullMapD + newX;
    
    if( ! isSubMapPropertySet( fullToSub( newRobberIndex ), blocking ) ) {
        
        // did not hit wall, can actually move here
        moveRobber( newRobberIndex );
        mLastPlacedObject = 0;
        fireActionPerformed( this );
        }
    
    }



void HouseGridDisplay::specialKeyUp( int inKeyCode ) {
    }



void HouseGridDisplay::moveRobber( int inNewIndex ) {
    
    if( mRobberIndex != inNewIndex ) {
        
        int oldX = mRobberIndex % mFullMapD;
        int oldY = mRobberIndex / mFullMapD;
        int newX = inNewIndex % mFullMapD;
        int newY = inNewIndex / mFullMapD;
        
        int dX = newX - oldX;
        
        int dY = newY - oldY;
        
        if( dX != 0 ) {
            if( dX == 1 ) {
                mRobberOrientation = 1;
                }
            else { 
                mRobberOrientation = 3;
                }
            }
        else if( dY != 0 ) {
            if( dY == 1 ) {
                mRobberOrientation = 0;
                }
            else { 
                mRobberOrientation = 2;
                }
            }
        


        mRobberIndex = inNewIndex;

        // if robber too close to edge, move view to keep robber on screen
        int robSubIndex = fullToSub( mRobberIndex );
        int robSubY = robSubIndex / HOUSE_D;
        int robSubX = robSubIndex % HOUSE_D;
    

        int xExtra = 0;
        int yExtra = 0;

        if( robSubX > HOUSE_D - 3 ) {
            xExtra = robSubX - (HOUSE_D - robSubX);
            xExtra /= 2;

            if( xExtra + mSubMapOffsetX + HOUSE_D > mFullMapD ) {
                xExtra = mFullMapD - ( mSubMapOffsetX + HOUSE_D );
                }
            }
        else if( robSubX < 2 ) {
            xExtra = robSubX - (HOUSE_D - robSubX);
            xExtra /= 2;

            if( xExtra + mSubMapOffsetX < 0 ) {
                xExtra = -mSubMapOffsetX;
                }        
            }
    
        if( robSubY > HOUSE_D - 3 ) {
            yExtra = robSubY - (HOUSE_D - robSubY);
            yExtra /= 2;

            if( yExtra + mSubMapOffsetY + HOUSE_D > mFullMapD ) {
                yExtra = mFullMapD - ( mSubMapOffsetY + HOUSE_D );
                }        
            }
        else if( robSubY < 2 ) {
            yExtra = robSubY - (HOUSE_D - robSubY);
            yExtra /= 2;

            if( yExtra + mSubMapOffsetY < 0 ) {
                yExtra = -mSubMapOffsetY;
                }        
            }


        if( xExtra != 0 || yExtra != 0 ) {
            setVisibleOffset( mSubMapOffsetX + xExtra,
                              mSubMapOffsetY + yExtra );
            }
        }
    }



char HouseGridDisplay::isSubMapPropertySet( int inSubCellIndex, 
                                            propertyID inProperty ) {
    return isPropertySet( mHouseSubMapIDs[ inSubCellIndex ],
                          mHouseSubMapCellStates[ inSubCellIndex ],
                          inProperty );
    }



int HouseGridDisplay::subToFull( int inSubCellIndex ) {
    int x = inSubCellIndex % HOUSE_D;
    int y = inSubCellIndex / HOUSE_D;

    int bigY = y + mSubMapOffsetY;
    int bigX = x + mSubMapOffsetX;

    return bigY * mFullMapD + bigX;
    }



int HouseGridDisplay::fullToSub( int inFullCellIndex ) {
    int bigX = inFullCellIndex % mFullMapD;
    int bigY = inFullCellIndex / mFullMapD;

    int x = bigX - mSubMapOffsetX;
    int y = bigY - mSubMapOffsetY;

    if( y >= 0 && y < HOUSE_D && 
        x >= 0 && x < HOUSE_D ) {
        return y * HOUSE_D + x;
        }
    return -1;
    }




void HouseGridDisplay::copySubCellBack( int inSubCellIndex ) {
    int bigIndex = subToFull( inSubCellIndex );
    
    mHouseMapIDs[ bigIndex ] = 
        mHouseSubMapIDs[ inSubCellIndex ];
            
    mHouseMapCellStates[ bigIndex ] = 
        mHouseSubMapCellStates[ inSubCellIndex ];
    
    recomputeWallShadows();
    }



void HouseGridDisplay::copyAllIntoSubCells() {
    for( int y=0; y<HOUSE_D; y++ ) {
        int bigY = y + mSubMapOffsetY;

        for( int x=0; x<HOUSE_D; x++ ) {
            int bigX = x + mSubMapOffsetX;

            int subIndex = y * HOUSE_D + x;
            
            int bigIndex = bigY * mFullMapD + bigX;

            mHouseSubMapIDs[ subIndex ] = 
                mHouseMapIDs[ bigIndex ];
            
            mHouseSubMapCellStates[ subIndex ] = 
                mHouseMapCellStates[ bigIndex ];
            }
        }
    }



void HouseGridDisplay::setVisibleOffset( int inXOffset, int inYOffset ) {
    
    mSubMapOffsetX = inXOffset;
    mSubMapOffsetY = inYOffset;

    copyAllIntoSubCells();

    recomputeWallShadows();
    }







void HouseGridDisplay::recomputeWallShadows() {
    if( mWallShadowSprite != NULL ) {
        freeSprite( mWallShadowSprite );
        mWallShadowSprite = NULL;
        }
    



    int blowUpFactor = 8;
    int blownUpSize = HOUSE_D * blowUpFactor;

    double log2size = log( blownUpSize ) / log( 2 );
    
    int next2PowerSize = (int)( ceil( log2size  ) );
    
    int paddedSize = blownUpSize;

    if( next2PowerSize != log2size ) {
        paddedSize = (int)( pow( 2, next2PowerSize ) );
        }
    
    int blowUpBorder = ( paddedSize - blownUpSize ) / 2;
    

    int numBlowupPixels = paddedSize * paddedSize;

    // opt:  do all this processing with uchars instead of doubles
    unsigned char *fullGridChannelsBlownUpAlpha =
        new unsigned char[ numBlowupPixels ];

    int *touchIndices = new int[ numBlowupPixels ];

    int numTouched = 0;

    memset( fullGridChannelsBlownUpAlpha, 255, numBlowupPixels );

    
    for( int y=0; y<HOUSE_D; y++ ) {
        
        for( int x=0; x<HOUSE_D; x++ ) {
            
            unsigned char alphaValue = 0;
            
            int flipY = HOUSE_D - y - 1;
            
            int subIndex = flipY * HOUSE_D + x;
            

            if( isSubMapPropertySet( subIndex, shadowMaking ) ) {
                alphaValue = 255;
                }
            
            for( int blowUpY= y * blowUpFactor; 
                 blowUpY< y * blowUpFactor + blowUpFactor; 
                 blowUpY++ ) {

                int padY = blowUpY + blowUpBorder;

                for( int blowUpX= x * blowUpFactor; 
                     blowUpX< x * blowUpFactor + blowUpFactor; 
                     blowUpX++ ) {
                    
                    int padX = blowUpX + blowUpBorder;

                    int imageIndex = padY * paddedSize + padX;
                    
                    fullGridChannelsBlownUpAlpha[ imageIndex ] = alphaValue;
                    
                    if( padY > 0 && padY < paddedSize - 1
                        &&
                        padX > 0 && padX < paddedSize - 1 ) {
                        
                        // apply blur filter to non-border pixels
                        touchIndices[numTouched] = imageIndex;
                        numTouched++;
                        }
                    else {
                        // set all border pixels to black
                        // and don't apply blur filter to those
                        fullGridChannelsBlownUpAlpha[ imageIndex ] = 255;
                        }
                    }
                }
            }
        }

    

    FastBoxBlurFilter filter2;

    for( int f=0; f<8; f++ ) {
        
        filter2.applySubRegion( fullGridChannelsBlownUpAlpha, 
                                touchIndices,
                                numTouched,
                                paddedSize, paddedSize );
        }
    
    
    // clear border again
    for( int y=0; y<paddedSize; y++ ) {
        for( int x=0; x<blowUpBorder; x++ ) {
            fullGridChannelsBlownUpAlpha[ y * paddedSize + x ] = 0;
            }
        for( int x=paddedSize - blowUpBorder; x<paddedSize; x++ ) {
            fullGridChannelsBlownUpAlpha[ y * paddedSize + x ] = 0;
            }
        }
    for( int x=0; x<paddedSize; x++ ) {
        for( int y=0; y<blowUpBorder; y++ ) {
            fullGridChannelsBlownUpAlpha[ y * paddedSize + x ] = 0;
            }
        for( int y=paddedSize - blowUpBorder; y<paddedSize; y++ ) {
            fullGridChannelsBlownUpAlpha[ y * paddedSize + x ] = 0;
            }
        }
        
    mWallShadowSprite = 
        fillSpriteAlphaOnly( fullGridChannelsBlownUpAlpha, 
                             paddedSize, paddedSize );
    
    delete [] fullGridChannelsBlownUpAlpha;
    delete [] touchIndices;






    /*
    int blowUpFactor = 4;
    int blownUpSize = HOUSE_D * blowUpFactor;

    int numBlowupPixels = blownUpSize * blownUpSize;
    
    // opt:  no need to operate on all four channels
    // just process alpha channel now

    // opt:  do all this processing with uchars instead of doubles
    unsigned char *fullGridChannelsBlownUpAlpha =
        new unsigned char[ numBlowupPixels ];

    memset( fullGridChannelsBlownUpAlpha, 0, numBlowupPixels );
    

    for( int y=0; y<HOUSE_D; y++ ) {
        for( int x=0; x<HOUSE_D; x++ ) {
            
            char hit = ( mHouseSubMapIDs[ y * HOUSE_D + x ] != 0 );
            

            if( hit ) {
                
                for( int by = y * blowUpFactor; 
                     by < (y+1) * blowUpFactor; by++ ) {
                    for( int bx = x * blowUpFactor; 
                         bx < (x+1) * blowUpFactor; bx++ ) {
                        
                        fullGridChannelsBlownUpAlpha[ by * blownUpSize + bx ]
                            = 255;
                        }
                    }
                }
            }
        }
    
    int numTouchedPixels = ( blownUpSize - 2 ) * ( blownUpSize - 2 );
    
    int *touchedPixels = new int[ numTouchedPixels ];
    
    int i = 0;
    for( int y=0; y<blownUpSize; y++ ) {
        for( int x=0; x<blownUpSize; x++ ) {
    
            if( y > 1 && y < blownUpSize - 1
                &&
                x > 1 && x < blownUpSize - 1 ) {
                
                // apply blur to all but edge pixels
                touchedPixels[ i ] = y * blownUpSize + x;
                i++;
                }
            else {
                // set edges to black
                fullGridChannelsBlownUpAlpha[ y * blownUpSize + x ] = 255;
                }
            }
        }

    FastBoxBlurFilter blur;
    
    blur.applySubRegion( fullGridChannelsBlownUpAlpha,
                         touchedPixels,
                         numTouchedPixels,
                         blownUpSize, blownUpSize );
    
    
    mWallShadowSprite = fillSpriteAlphaOnly( fullGridChannelsBlownUpAlpha,
                                             blownUpSize, 
                                             blownUpSize );

    delete [] fullGridChannelsBlownUpAlpha;
    */
    }




void HouseGridDisplay::logEdit( int inFullIndex, int inNewID ) {
    GridChangeRecord r;
    
    r.fullIndex = inFullIndex;
    r.oldID = mHouseMapIDs[ inFullIndex ];
    r.oldState = mHouseMapCellStates[ inFullIndex ];

    r.oldMobileID = mHouseMapMobileIDs[ inFullIndex ];
    r.oldMobileState = mHouseMapMobileCellStates[ inFullIndex ];

    r.newID = inNewID;
    r.robberIndex = mRobberIndex;
    
    r.subMapOffsetX = mSubMapOffsetX;
    r.subMapOffsetY = mSubMapOffsetY;

    mEditHistory.push_back( r );
    }



char HouseGridDisplay::canUndo() {
    return ( mEditHistory.size() > 0 );
    }

        
// returns cost of change that was undone
int HouseGridDisplay::undo() {
    int numSteps = mEditHistory.size();
    
    if( numSteps == 0 ) {
        return 0;
        }
    

    GridChangeRecord *r = mEditHistory.getElement( numSteps - 1 );
    
    
    mHouseMapIDs[ r->fullIndex ] = r->oldID;
    mHouseMapCellStates[ r->fullIndex ] = r->oldState;

    mHouseMapMobileIDs[ r->fullIndex ] = r->oldMobileID;
    mHouseMapMobileCellStates[ r->fullIndex ] = r->oldMobileState;
    

    

    // reset to 0 state (owner presentation) for all
    // non-stuck objects
    // NOT just object that was touched by Undo
    
    // as things are changing around during edits, all bets are off after
    // an undo, and it's best to revert everything back to state-0 to ensure
    // consistency
    resetToggledStatesInternal( 0 );
    

    mRobberIndex = r->robberIndex;
    
    if( r->newID == GOAL_ID ) {
        // this change set the goal
        // undoing it means goal not set
        mGoalSet = false;
        }
    else if( r->oldID == GOAL_ID ) {
        // rolling back to a placed goal
        mGoalSet = true;
        mGoalIndex = r->fullIndex;
        }
    
    // force copy-back and shadow recompute, plus possible view move
    setVisibleOffset( r->subMapOffsetX, r->subMapOffsetY );
    

    int cost = mPicker->getPrice( r->newID );
    
    if( cost == -1 ) {
        cost = 0;
        }
    
    

    mEditHistory.deleteElement( numSteps - 1 );

    return cost;
    }

