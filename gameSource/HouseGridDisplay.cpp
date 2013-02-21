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
                                    HouseObjectPicker *inPicker )
        : PageComponent( inX, inY ),
          mPicker( inPicker ),
          mHideRobber( false ),
          mWifeMoney( 0 ),
          mWifeName( NULL ),
          mSonName( NULL ),
          mDaughterName( NULL ),
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
          mHighlightIndex( -1 ),
          mPointerDownIndex( -1 ),
          mDraggedAway( false ),
          mPointerDownObjectID( -1 ),
          mPlaceOnDrag( false ),
          mTileRadius( 0.4375 ),
          mStartIndex( -1 ),
          mGoalIndex( -1 ),
          mMandatoryNeedsPlacing( false ),
          mMandatoryToPlaceID( -1 ),
          mAllFamilyObjectsHaveExitPath( false ),
          mPointerInside( false ),
          mWallShadowSprite( NULL ),
          mAllowPlacement( true ),
          mLastPlacedObject( 0 ),
          mToolTargetSprite( loadSprite( "toolTarget.tga" ) ),
          mToolTargetBorderSprite( loadSprite( "toolTargetBorder.tga" ) ),
          mToolTargetPickedFullIndex( -1 ) {


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
    if( mWifeName != NULL ) {
        delete [] mWifeName;
        }
    if( mSonName != NULL ) {
        delete [] mSonName;
        }
    if( mDaughterName != NULL ) {
        delete [] mDaughterName;
        }
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

    freeSprite( mToolTargetSprite );
    freeSprite( mToolTargetBorderSprite );

    for( int i=0; i<mFamilyExitPaths.size(); i++ ) {
        delete [] *( mFamilyExitPaths.getElement( i ) );
        }
    mFamilyExitPaths.deleteAll();
    mFamilyExitPathLengths.deleteAll();
    }



void HouseGridDisplay::setWifeName( const char *inWifeName ) {
    if( mWifeName != NULL ) {
        delete [] mWifeName;
        }
    mWifeName = stringDuplicate( inWifeName );
    }


void HouseGridDisplay::setSonName( const char *inSonName ) {
    if( mSonName != NULL ) {
        delete [] mSonName;
        }
    mSonName = stringDuplicate( inSonName );
    }


void HouseGridDisplay::setDaughterName( const char *inDaughterName ) {
    if( mDaughterName != NULL ) {
        delete [] mDaughterName;
        }
    mDaughterName = stringDuplicate( inDaughterName );
    }

    



void HouseGridDisplay::setHouseMap( const char *inHouseMap ) {
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
                }

            if( isPropertySet( id, state, mobile ) ) {
                mHouseMapMobileIDs[i] = id;
                
                
                if( isPropertySet( id, state, interactingWithPlayer ) &&
                    ! isPropertySet( id, state, stuck ) ) {
                    // left over state where this mobile interacted with
                    // last robber

                    // but that robber no longer present, so discard
                    // this state
                    state = 0;
                    }

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
    mLastRobberIndex = mStartIndex;
    mRobberState = 0;
    mRobberOrientation = 1;
    
    mEditHistory.deleteAll();

    checkExitPaths();
    }



void HouseGridDisplay::setWifeMoney( int inMoney ) {
    mWifeMoney = inMoney;
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



char *HouseGridDisplay::getWifeName() {
    if( mWifeName == NULL ) {
        return NULL;
        }
    else {
        return stringDuplicate( mWifeName );
        }
    }

char *HouseGridDisplay::getSonName() {
    if( mSonName == NULL ) {
        return NULL;
        }
    else {
        return stringDuplicate( mSonName );
        }
    }

char *HouseGridDisplay::getDaughterName() {
    if( mDaughterName == NULL ) {
        return NULL;
        }
    else {
        return stringDuplicate( mDaughterName );
        }
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
                    if( nonMobilePart != NULL ) {
                        delete [] nonMobilePart;
                        }
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



char *HouseGridDisplay::getFamilyExitPaths() {
    int numPaths = mFamilyExitPaths.size();
    
    if( numPaths == 0 ) {
        return stringDuplicate( "##" );
        }
    
    SimpleVector<char*> mainListAccum;

    for( int i=0; i<numPaths; i++ ) {
        GridPos *moveList = *( mFamilyExitPaths.getElement( i ) );
        int length = *( mFamilyExitPathLengths.getElement( i ) );
        
        SimpleVector<char *> subListAccum;
        
        for( int m=0; m<length; m++ ) {
            char *posString = autoSprintf( "%d", posToIndex( moveList[m] ) );
            
            subListAccum.push_back( posString );
            }

        char **subListStrings = subListAccum.getElementArray();
    
        char *posList = join( subListStrings, length, "#" );

        for( int i=0; i<length; i++ ) {
            delete [] subListStrings[i];
            }
        delete [] subListStrings;

        mainListAccum.push_back( posList );
        }


    char **listStrings = mainListAccum.getElementArray();
    
    char *pathsList = join( listStrings, numPaths, "##" );

    for( int i=0; i<numPaths; i++ ) {
        delete [] listStrings[i];
        }
    delete [] listStrings;

    return pathsList;
    }



char HouseGridDisplay::getWifeLiving() {
    for( int i=0; i<mNumMapSpots; i++ ) {
        int id = mHouseMapIDs[i];
        int state = mHouseMapCellStates[i];
        
        if( isPropertySet( id, state, wife ) ) {
            
            if( isPropertySet( id, state, deadFamily ) ) {    
                return false;
                }
            else if( isPropertySet( id, state, family ) ) {
                return true;
                }
            }
        }

    return false;
    }



char HouseGridDisplay::areMandatoriesPlaced() {
    return !mMandatoryNeedsPlacing;
    }


char HouseGridDisplay::doAllFamilyObjectsHaveExitPath() {
    return mAllFamilyObjectsHaveExitPath;
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
        
        // out of bounds
        return -1;
        }

    return nY * mFullMapD + nX;
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
    
    numOrientations = getNumOrientations( inTileID, inTileState );            
            
                
    if( numOrientations == 16 ) {
        // full binary LBRT flags based on neighbors of same type
        // OR in groupWith set for our tile
        // (different states okay, but only if tile in that state has
        //  the same number of orientations)

        int neighborsEqual[4] = { 0, 0, 0, 0 };

        for( int n=0; n<4; n++ ) {
            
            int neighborIndex = getTileNeighbor( inFullIndex, n );
            
            if( neighborIndex != -1 &&
                isInGroup( inTileID, mHouseMapIDs[neighborIndex] ) && 
                getNumOrientations( 
                    mHouseMapIDs[neighborIndex], 
                    mHouseMapCellStates[neighborIndex] ) == numOrientations ) {

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



        if( isPropertySet( inTileID, inTileState, playerSeeking ) ||
            isPropertySet( inTileID, inTileState, playerAvoiding ) ) {
            
            int x = inFullIndex % mFullMapD;
            int y = inFullIndex / mFullMapD;
            
            int robberX = mRobberIndex % mFullMapD;
            int robberY = mRobberIndex / mFullMapD;
            
            int dX = robberX - x;
            int dY = robberY - y;
            
            if( isPropertySet( inTileID, inTileState, playerAvoiding ) ) {
                // face away from player
                dX *= -1;
                dY *= -1;
                }

            if( dX == 0 && dY == 0 ) {
                // same square as player
                // face same direction as player
                orientationIndex = mRobberOrientation;
                // but only if player facing left/right
                // (otherwise, always face right)
                if( orientationIndex != 1 && orientationIndex != 3 ) {
                    orientationIndex = 1;
                    }
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
    setDrawColor( 0, 0, 0, 1 );
    
    toggleLinearMagFilter( true );

    drawSprite( sDropShadowSprite, inPosition, 1.0/16.0 );
    
    toggleLinearMagFilter( false );
    }




void HouseGridDisplay::drawDarkHaloBehind( int inObjectID, int inOrientation,
                                           int inState, 
                                           doublePair inPosition ) {
    
    setDrawColor( 1, 1, 1, 0.5 );

    SpriteHandle haloSprite = 
        getObjectHaloSprite( inObjectID, 
                             inOrientation, 
                             inState );
    
    toggleLinearMagFilter( true );

    drawSprite( haloSprite, inPosition, 1.0/32.0 );

    toggleLinearMagFilter( false );
    }



void HouseGridDisplay::drawRobber( doublePair inPosition ) {
    if( mHideRobber ) {
        return;
        }

    if( ! isPropertySet( PLAYER_ID, mRobberState, noDropShadow ) ) {
        // first drop shadow
        drawDropShadow( inPosition );
        }
        
    
    if( isPropertySet( PLAYER_ID, mRobberState, darkHaloBehind ) ) {
        drawDarkHaloBehind( PLAYER_ID, 
                            mRobberOrientation, 
                            mRobberState,
                            inPosition );
        }

    setDrawColor( 1, 1, 1, 1 );

    SpriteHandle sprite = 
        getObjectSprite( PLAYER_ID, 
                         mRobberOrientation, 
                         mRobberState );
    
    drawSprite( sprite, inPosition, 1.0/32.0 );
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
                     
            char deadFamilyObject =
                isSubMapPropertySet( i, deadFamily );

            char familyObject = 
                isSubMapPropertySet( i, family ) ||
                deadFamilyObject;
            
            char forceUnderShadowsObject = 
                isSubMapPropertySet( i, forceUnderShadows ) ||
                deadFamilyObject;
            

            doublePair tilePos = getTilePos( i );
 

            if( inBeneathShadowsOnly && aboveShadows ) {
                // skip this blocking tile
                
                // but draw any parts under shadows
                if( isUnderSpritePresent( houseTile, 
                                          houseTileState ) ) {
                    
                    int orientationIndex = 
                        getOrientationIndex( fullI, 
                                             houseTile, houseTileState );
                    
                    setDrawColor( 1, 1, 1, 1 );
                    
                    
                    SpriteHandle sprite = 
                        getObjectSpriteUnder( houseTile, 
                                               orientationIndex, 
                                               houseTileState );
                
                    drawSprite( sprite, tilePos, 1.0/32.0 );
                    }
                

                if( mHighlightIndex != i && fullI != mRobberIndex 
                    && mHouseMapMobileIDs[ fullI ] == 0
                    && ! familyObject ) {
                    // nothing left to draw, if no highlight is here
                    // no mobile or family object is here
                    i++;
                    continue;
                    }
                }
            else if( ! inBeneathShadowsOnly && ! aboveShadows ) {
                // skip this non-blocking tile

                if( mHighlightIndex != i && fullI != mRobberIndex 
                    && mHouseMapMobileIDs[ fullI ] == 0
                    && ! familyObject ) {
                    // nothing left to draw, if no highlight is here
                    // no mobile or family object is here
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
                
                    drawSprite( sprite, tilePos, 1.0/32.0 );
                    }
                }
            

            // also draw beneath shadow tiles under robber or mobile
            // even if robber/mobile is under shadows itself (robber/mobile
            // is always on top of wires, switches, etc.)
            
            if( inBeneathShadowsOnly && 
                ! aboveShadows &&
                ! familyObject &&
                houseTile != 0 ) {
                
                // now draw tile itself, on top of floor

                if( isSubMapPropertySet( i, darkHaloBehind ) ) {
                    
                    drawDarkHaloBehind( houseTile, orientationIndex,
                                        houseTileState,
                                        tilePos );
                    }

                if( isSubMapPropertySet( i, mobile ) &&
                    ! isSubMapPropertySet( i, noDropShadow ) ) {
                    // drop shadow
                    drawDropShadow( tilePos );
                    }
                

                setDrawColor( 1, 1, 1, 1 );
                
                SpriteHandle sprite = getObjectSprite( houseTile, 
                                                       orientationIndex, 
                                                       houseTileState );
                
                drawSprite( sprite, tilePos, 1.0/32.0 );
                }




            char robberDrawn = false;

            // mobile objects above shadows (unless forced under, 
            // behind structural tiles in current row only
            
                
            if( mHouseMapMobileIDs[fullI] != 0 ) {
                // mobile object here
                    
                    

                int mobID = mHouseMapMobileIDs[fullI];
                int mobState = mHouseMapMobileCellStates[fullI];
                
                
                // all stuck mobiles are dead and under shadows too
                char mobForcedUnderShadows = 
                    isPropertySet( mobID, mobState, stuck )
                    ||
                    isPropertySet( mobID, mobState, forceUnderShadows );
                

                if( ( ! inBeneathShadowsOnly && ! mobForcedUnderShadows )
                    ||
                    ( inBeneathShadowsOnly && mobForcedUnderShadows ) ) {

                    int mobOrientation = getOrientationIndex( fullI, mobID,
                                                              mobState );
                    
                    if( isPropertySet( mobID, mobState, darkHaloBehind ) ) {
                        
                        drawDarkHaloBehind( mobID, mobOrientation, mobState,
                                            tilePos );
                        }

                    if( ! isPropertySet( mobID, mobState, noDropShadow ) ) {
                        // first drop shadow
                        drawDropShadow( tilePos );
                        }
                        
                    setDrawColor( 1, 1, 1, 1 );
                
                    SpriteHandle sprite = 
                        getObjectSprite( mobID, 
                                         mobOrientation, 
                                         mobState );
                
                    drawSprite( sprite, tilePos, 1.0/32.0 );
                    }
                }
            

            // family objects draw like mobiles, but they're not
            // (because they can co-occupy a space with a mobile,
            //  and they move differently)
            if( familyObject ) {
                
                if( ( ! inBeneathShadowsOnly && ! forceUnderShadowsObject )
                    ||
                    ( inBeneathShadowsOnly && forceUnderShadowsObject ) ) {  
                    
                    // draw now

                    if( isPropertySet( houseTile, houseTileState, 
                                       darkHaloBehind ) ) {
                        
                        drawDarkHaloBehind( houseTile, orientationIndex,
                                            houseTileState,
                                            tilePos );
                        }
                        
                    drawDropShadow( tilePos );
                    
                    setDrawColor( 1, 1, 1, 1 );
                    
                    SpriteHandle sprite = getObjectSprite( houseTile, 
                                                           orientationIndex, 
                                                           houseTileState );
                    
                    drawSprite( sprite, tilePos, 1.0/32.0 );
                    }
                }


            // same for robber (if not already drawn under a mobile)
            if( !inBeneathShadowsOnly && mRobberIndex == fullI
                &&
                ! robberDrawn ) {

                if( ! isPropertySet( PLAYER_ID, 
                                     mRobberState, forceUnderShadows ) ) {
                    drawRobber( tilePos );
                    }
                }
            else if( inBeneathShadowsOnly && mRobberIndex == fullI 
                     &&
                     ! robberDrawn ) {
                if( isPropertySet( PLAYER_ID, 
                                   mRobberState, forceUnderShadows ) ) {
                    drawRobber( tilePos );
                    }
                }
            
                
                



            if( !inBeneathShadowsOnly && aboveShadows 
                     && houseTile != 0 ) {
                // now draw blocking objects on top of floor

                if( isSubMapPropertySet( i, darkHaloBehind ) ) {
                    
                    drawDarkHaloBehind( houseTile, orientationIndex,
                                            houseTileState,
                                            tilePos );
                    }


                setDrawColor( 1, 1, 1, 1 );
                

                SpriteHandle sprite = getObjectSprite( houseTile, 
                                                       orientationIndex, 
                                                       houseTileState );
                
                drawSprite( sprite, tilePos, 1.0/32.0 );
                }

            

            // draw target highlights on top of tiles or mobile objects
            if( inBeneathShadowsOnly && mHouseMapMobileIDs[fullI] != 0 ) {
                // don't draw any target highlights here
                // because there's a mobile here that will be drawn later
                // with any target highlight on top

                }
            else if( mToolTargetFullIndices.getElementIndex( fullI ) != -1 ) {
                
                // look at tile to the south
                // if it's blocking, draw a half-strength highlight here
                // (because we're going to overlay the other half-strength
                //  layer later, after southern blocking tile is drawn).
                // But if southern tile is not blocking, draw full-strength
                // now (because no additional overlay is coming later

                char southBlocking = true;
                
                int southI = fullI - mFullMapD;

                if( southI >= 0 ) {
                    southBlocking = 
                        isPropertySet( mHouseMapIDs[ southI ],
                                       mHouseMapCellStates[ southI ],
                                       blocking );
                    }
                
                double fade = 0.5;
                
                if( southBlocking ) {
                    fade = 0.25;
                    }

                if( fullI == mToolTargetPickedFullIndex ) {
                    setDrawColor( 1, 0, 0, fade );
                    }
                else {
                    setDrawColor( 1, 1, 1, fade );
                    }

                drawSprite( mToolTargetSprite, tilePos, 
                            1.0 / 16.0 );

                setDrawColor( 1, 1, 1, 1 );
                drawSprite( mToolTargetBorderSprite, tilePos, 
                            1.0 / 16.0 );
                }
            

            
            

            
            // no highlight over start, robber, or permanents
            int highlightPick = -1;
            
            if( mHighlightIndex == i &&
                fullI != mStartIndex &&
                fullI != mRobberIndex &&
                ! isSubMapPropertySet( i, permanent ) ) {
                
                if( mPicker != NULL ) {
                    highlightPick = mPicker->getSelectedObject();
                    }
                else {
                    
                    char highlightAboveShadows = 
                        isPropertySet( highlightPick, 0, structural ) ||
                        isPropertySet( highlightPick, 0, shadowMaking );

                    if( ( highlightAboveShadows && inBeneathShadowsOnly ) 
                        ||
                        ( ! highlightAboveShadows && 
                          ! inBeneathShadowsOnly ) ) {
                    
                        // only draw each highlight once 
                        // (along with other objects
                        //  that share its shadow-casting status)
                    
                        highlightPick = -1;
                        }
                    }
                }


            if( highlightPick != -1 ) {
                int mobID = mHouseMapMobileIDs[fullI];
                
                if( mMandatoryNeedsPlacing ) {
                    // ghost of to-place mandatory for placement
                    setDrawColor( 1, 1, 1, 0.35 );

                    int ghostOrientation = 
                        getOrientationIndex( fullI, 
                                             mMandatoryToPlaceID,
                                             0 );
                    
                    SpriteHandle sprite = getObjectSprite( 
                        mMandatoryToPlaceID, 
                        ghostOrientation, 
                        0 );
                
                    drawSprite( sprite, tilePos, 1.0/32.0 );
                    }
                else if( 
                    // placing empty floor, and current tile not empty floor
                    ( highlightPick == 0 && houseTile != 0 ) ||
                    // not placing empty, and current tile matches placement
                    // (erase-same behavior)
                    ( highlightPick != 0 && 
                      houseTile == highlightPick ) ||
                    // mobile here
                    // placing empty floor or erase mode
                    ( mobID != 0 &&
                      ( highlightPick == 0 || highlightPick == mobID ) ) ||
                    // family here (clicking always removes them)
                    isSubMapPropertySet( i, family ) ||
                    // vault here (clicking always removes it)
                    houseTile == GOAL_ID ) {

                    


                    // darken existing tile to imply removal on click
                    setDrawColor( 0, 0, 0, 0.35 );
                    
                    SpriteHandle sprite;

                    if( isUnderSpritePresent( houseTile, houseTileState ) ) {
                        sprite = getObjectSpriteUnder( houseTile, 
                                                       orientationIndex,
                                                       houseTileState );
                        drawSprite( sprite, tilePos, 1.0/32.0 );
                        }
                    if( isBehindSpritePresent( houseTile, houseTileState ) ) {
                        sprite = getObjectSpriteBehind( houseTile, 
                                                        orientationIndex,
                                                        houseTileState );
                        drawSprite( sprite, tilePos, 1.0/32.0 );
                        }
                    
                    
                    if( houseTile != 0 ) {
                        
                        sprite = getObjectSprite( houseTile, 
                                                  orientationIndex, 
                                                  houseTileState );
                        
                        drawSprite( sprite, tilePos, 1.0/32.0 );
                        }

                    
                    if( mobID != 0 ) {
                        int mobState = mHouseMapMobileCellStates[fullI];
                        
                        int mobOrientation = getOrientationIndex( fullI, mobID,
                                                                  mobState );

                        sprite = getObjectSprite( mobID, 
                                                  mobOrientation, 
                                                  mobState);
                                                  
                        drawSprite( sprite, tilePos, 1.0/32.0 );
                        }                    

                    }
                else if( houseTile != highlightPick ) {
                    setDrawColor( 1, 1, 1, 0.35 );
                
                    int ghostOrientation = getOrientationIndex( fullI, 
                                                                highlightPick, 
                                                                0 );

                    SpriteHandle sprite;


                    if( isUnderSpritePresent( highlightPick, 0 ) ) {
                        sprite = getObjectSpriteUnder( highlightPick, 
                                                       ghostOrientation,
                                                       0 );
                        drawSprite( sprite, tilePos, 1.0/32.0 );
                        }
                    if( isBehindSpritePresent( highlightPick, 0 ) ) {
                        sprite = getObjectSpriteBehind( highlightPick, 
                                                       ghostOrientation,
                                                       0 );
                        drawSprite( sprite, tilePos, 1.0/32.0 );
                        }


                    sprite = getObjectSprite( highlightPick, 
                                              ghostOrientation,
                                              0 );
                    
                    drawSprite( sprite, tilePos, 1.0/32.0 );
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
            drawSprite( sprite, getTilePos( y * HOUSE_D + x ), 1.0/32.0 );
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
        drawSprite( sprite, getTilePos( startSubIndex ), 1.0/32.0 );
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
            
            drawSprite( sprite, tilePos, 1.0/32.0 );
            }
        }



    // draw target highlights one more time, faint
    // so they "show through" overlapping tiles and objects a bit
    // only do this if south tile below highlight is blocking

    for( int i=0; i<mToolTargetFullIndices.size(); i++ ) {
        int fullI = *( mToolTargetFullIndices.getElement(i) );
        
        int subI = fullToSub( fullI );
        
        if( subI != -1 ) {
            char southBlocking = true;
        
            int southI = fullI - mFullMapD;
            
            if( southI >= 0 ) {
                southBlocking = 
                    isPropertySet( mHouseMapIDs[ southI ],
                                   mHouseMapCellStates[ southI ],
                                   blocking );
                }

            if( southBlocking ) {
                
                doublePair tilePos = getTilePos( subI );
                
                if( fullI == mToolTargetPickedFullIndex ) {
                    setDrawColor( 1, 0, 0, 0.25 );
                    }
                else {
                    setDrawColor( 1, 1, 1, 0.25 );
                    }

                drawSprite( mToolTargetSprite, tilePos, 
                            1.0 / 16.0 );
                
                setDrawColor( 1, 1, 1, 0.25 );
                drawSprite( mToolTargetBorderSprite, tilePos, 
                            1.0 / 16.0 );
                }
            }
        }
    


    
    // noise over everything
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
            

    

    

    disableScissor();
    }



void HouseGridDisplay::pointerOver( float inX, float inY ) {
    mHighlightIndex = getTileIndex( inX, inY );

    if( mHighlightIndex != -1 ) {
        mPointerInside = true;
        
        int fullI = subToFull( mHighlightIndex );
        
        
        char *nonMobileDescription; 

        if( fullI == mStartIndex && mHouseMapIDs[fullI] == 0 ) {
            nonMobileDescription = 
                stringDuplicate( translate( "startTileDescription" ) );
            }
        else {                
            nonMobileDescription = 
                stringDuplicate( 
                    getObjectDescription( 
                        mHouseSubMapIDs[ mHighlightIndex ],
                        mHouseSubMapCellStates[ mHighlightIndex ] ) );


            if( isPropertySet( mHouseMapIDs[fullI], mHouseMapCellStates[fullI],
                               wife ) ) {

                char *nameInserted =
                    autoSprintf( nonMobileDescription, mWifeName );
                
                delete [] nonMobileDescription;
                nonMobileDescription = nameInserted;
                }
            if( isPropertySet( mHouseMapIDs[fullI], mHouseMapCellStates[fullI],
                               son ) ) {
                
                char *nameInserted =
                    autoSprintf( nonMobileDescription, mSonName );
                
                delete [] nonMobileDescription;
                nonMobileDescription = nameInserted;
                }
            if( isPropertySet( mHouseMapIDs[fullI], mHouseMapCellStates[fullI],
                               daughter ) ) {

                char *nameInserted =
                    autoSprintf( nonMobileDescription, mDaughterName );
                
                delete [] nonMobileDescription;
                nonMobileDescription = nameInserted;
                }
            


            if( mWifeMoney > 0 && 
                isPropertySet( mHouseMapIDs[fullI], mHouseMapCellStates[fullI],
                               wife ) ) {

                char *fullDescription =
                    autoSprintf( translate( "wifeHolding" ), 
                                 nonMobileDescription, mWifeMoney );
                
                delete [] nonMobileDescription;
                nonMobileDescription = fullDescription;
                }
            }
        

        if( mHouseMapMobileIDs[ fullI ] != 0 ) {
            
            const char *mobileDescription = 
                getObjectDescription( 
                    mHouseMapMobileIDs[ fullI ],
                    mHouseMapMobileCellStates[ fullI ] );
        
            char *tip = autoSprintf( "%s  /  %s",
                                     nonMobileDescription, mobileDescription );
            
            setToolTip( tip );
            
            delete [] tip;
            }
        else {
            setToolTip( nonMobileDescription );
            }
        
        delete [] nonMobileDescription;
        }
    else if( mPointerInside ) {
        // just moved off grid
        setToolTip( NULL );
        
        mPointerInside = false;
        }
    }



void HouseGridDisplay::placeMandatory( int inFullIndex, int inIndex ) {
    
    int oldID = mHouseSubMapIDs[ inIndex ];
    int oldState = mHouseSubMapCellStates[ inIndex ];
    
    mHouseSubMapIDs[ inIndex ] = mMandatoryToPlaceID;
    
    logEdit( inFullIndex, mMandatoryToPlaceID );

    
    if( mMandatoryToPlaceID == GOAL_ID ) {
        mGoalIndex = inFullIndex;
        }
    if( isPropertySet( oldID, oldState, mandatory ) ) {
        mMandatoryToPlaceID = oldID;
        }
    else {
        mMandatoryNeedsPlacing = false;
        }
    
    mLastPlacedObject = mMandatoryToPlaceID;    
    
    
    // changes reset state
    mHouseSubMapCellStates[ inIndex ] = 0;
    
    // clear mobile objects
    mHouseMapMobileIDs[ inFullIndex ] = 0;
    mHouseMapMobileCellStates[ inFullIndex ] = 0;
    
    copySubCellBack( inIndex );
    fireActionPerformed( this );
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
        mDraggedAway &&
        ! isSubMapPropertySet( index, permanent ) ) {
    
        // pointer dropped on grid after being dragged somewhere else
        int fullIndex = subToFull( index );
        
        
        if( mMandatoryNeedsPlacing && 
            fullIndex != mStartIndex &&
            fullIndex != mRobberIndex ) {
            // mandatory dragged/dropped here

            placeMandatory( fullIndex, index );
            }

        }
    
    }



void HouseGridDisplay::pointerDrag( float inX, float inY ) {
    pointerOver( inX, inY );
    int index = getTileIndex( inX, inY );

    mHighlightIndex = index;

    if( index != -1 && 
        mPointerDownIndex != -1 &&
        index != mPointerDownIndex &&
        mPointerDownObjectID != -1 &&
        ! isSubMapPropertySet( index, permanent ) &&
        ! isSubMapPropertySet( index, mandatory ) ) {

        mDraggedAway = true;

        int fullIndex = subToFull( index );

        if( fullIndex != mRobberIndex &&
            fullIndex != mStartIndex ) {
            
            int old = mHouseSubMapIDs[ index ];
            int oldMobile = mHouseMapMobileIDs[ fullIndex ];

            // placement of floor is always drag-place
            if( mPlaceOnDrag &&
                ( mPointerDownObjectID == 0 
                  ||
                  ( old != mPointerDownObjectID &&
                    oldMobile != mPointerDownObjectID ) ) ) {
                
                // drag-place
                if( mAllowPlacement ) {
                    mHouseSubMapIDs[ index ] = mPointerDownObjectID;
                    mLastPlacedObject = mPointerDownObjectID;

                    // avoid logging floor-on-floor placement as edits
                    if( old != mPointerDownObjectID || 
                        oldMobile != mPointerDownObjectID ) {

                        logEdit( fullIndex, mPointerDownObjectID );
                        }
                    
                    // changes reset state
                    mHouseSubMapCellStates[ index ] = 0;

                    // clear mobile objects
                    mHouseMapMobileIDs[ fullIndex ] = 0;
                    mHouseMapMobileCellStates[ fullIndex ] = 0;
                    
                    copySubCellBack( index );
                    fireActionPerformed( this );
                    }
                }
            else if( !mPlaceOnDrag && 
                     ( old == mPointerDownObjectID ||
                       oldMobile == mPointerDownObjectID ) ) {
                
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
    

    
    

    if( mMandatoryNeedsPlacing && fullIndex != mStartIndex ) {
        placeMandatory( fullIndex, index );
        }
    else if( !mMandatoryNeedsPlacing && 
             isPropertySet( mHouseMapIDs[ fullIndex ],
                            mHouseMapCellStates[ fullIndex ], mandatory ) ) {
        // mandatory moving!
        mMandatoryToPlaceID = mHouseSubMapIDs[ index ];
        mMandatoryNeedsPlacing = true;
        
        mHouseSubMapIDs[ index ] = 0;
 
        mLastPlacedObject = 0;

        logEdit( fullIndex, 0 );

        // changes reset state
        mHouseSubMapCellStates[ index ] = 0;

        
        // Don't clear mobile objects just because mandatory picked up 
        // from there
        

        copySubCellBack( index );
        fireActionPerformed( this );
        }
    else if( fullIndex != mStartIndex ) {
    
        if( mPicker != NULL ) {
            
        
            int old = mHouseSubMapIDs[ index ];
            int oldMobile = mHouseMapMobileIDs[ fullIndex ];
            
            int picked = mPicker->getSelectedObject();
            
            // floor placement always counts as place mode
            if( picked == 0 
                ||
                ( old != picked && oldMobile != picked ) ) {
                // place mode (or replace mode)
                if( mAllowPlacement ) {
                    mHouseSubMapIDs[ index ] = picked;
                    mLastPlacedObject = picked;
                    mPointerDownObjectID = picked;
                    mPlaceOnDrag = true;

                    // avoid logging floor-on-floor placement as edits
                    if( old != picked || oldMobile != picked ) {    
                        logEdit( fullIndex, picked );
                        }
                    
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
    
    pointerOver( inX, inY );
    }




void HouseGridDisplay::keyDown( unsigned char inASCII ) {    
    switch( inASCII ) {
        case 'w':
        case 'W':
        case 'i':
        case 'I':
            specialKeyDown( MG_KEY_UP );
            break;
        case 'a':
        case 'A':
        case 'j':
        case 'J':
            specialKeyDown( MG_KEY_LEFT );
            break;
        case 's':
        case 'S':
        case 'k':
        case 'K':
            specialKeyDown( MG_KEY_DOWN );
            break;
        case 'd':
        case 'D':
        case 'l':
        case 'L':
            specialKeyDown( MG_KEY_RIGHT );
            break;
        }
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
    
    if( newRobberIndex != mRobberIndex &&
        ! isSubMapPropertySet( fullToSub( newRobberIndex ), blocking ) ) {
        
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
        

        mLastRobberIndex = mRobberIndex;
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
    
    checkExitPaths();

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
    recomputeWallShadows();
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
    
    
    // set border regions to solid black to avoid light halo
    for( int y=0; y<paddedSize; y++ ) {
        for( int x=0; x<blowUpBorder; x++ ) {
            fullGridChannelsBlownUpAlpha[ y * paddedSize + x ] = 255;
            }
        for( int x=paddedSize - blowUpBorder; x<paddedSize; x++ ) {
            fullGridChannelsBlownUpAlpha[ y * paddedSize + x ] = 255;
            }
        }
    for( int x=0; x<paddedSize; x++ ) {
        for( int y=0; y<blowUpBorder; y++ ) {
            fullGridChannelsBlownUpAlpha[ y * paddedSize + x ] = 255;
            }
        for( int y=paddedSize - blowUpBorder; y<paddedSize; y++ ) {
            fullGridChannelsBlownUpAlpha[ y * paddedSize + x ] = 255;
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
    
    mLastRobberIndex = mRobberIndex;
    mRobberIndex = r->robberIndex;
    
    
    if( isPropertySet( r->newID, 0, mandatory ) ) {
        // this change placed a mandatory object
        // undoing it means that mandatory still needs placing
        mMandatoryNeedsPlacing = true;
        mMandatoryToPlaceID = r->newID;
        }
    else if( isPropertySet( r->oldID, 0, mandatory ) ) {
        // rolling back to a placed mandatory
        mMandatoryNeedsPlacing = false;
        if( r->oldID == GOAL_ID ) {
            mGoalIndex = r->fullIndex;
            }
        }
    
    // force copy-back and shadow recompute, plus possible view move
    setVisibleOffset( r->subMapOffsetX, r->subMapOffsetY );
    

    int cost = mPicker->getPrice( r->newID );
    
    if( cost == -1 ) {
        cost = 0;
        }
    
    

    mEditHistory.deleteElement( numSteps - 1 );


    checkExitPaths();
    
    return cost;
    }




void HouseGridDisplay::setTargetHighlights( 
    SimpleVector<int> *inToolTargetFullIndices ) {

    mToolTargetFullIndices.deleteAll();
    
    mToolTargetPickedFullIndex = -1;

    int *array = inToolTargetFullIndices->getElementArray();
    int size = inToolTargetFullIndices->size();
    
    mToolTargetFullIndices.push_back( array, size );
    
    delete [] array;
    }



void HouseGridDisplay::setPickedTargetHighlight( int inPickedFullIndex ) {
    mToolTargetPickedFullIndex = inPickedFullIndex;
    }




char *HouseGridDisplay::getBlockedMap() {
    char *blockedMap = new char[mNumMapSpots];
    for( int i=0; i<mNumMapSpots; i++ ) {
        if( !isPropertySet( mHouseMapIDs[i], mHouseMapCellStates[i],
                            family ) &&
            !isPropertySet( mHouseMapIDs[i], mHouseMapCellStates[i],
                            mobile ) &&
            mHouseMapIDs[i] != 0 ) {
            // not mobile, not family, not bare floor
            blockedMap[i] = true;
            }
        else {
            blockedMap[i] = false;
            }
        }
    return blockedMap;
    }



void HouseGridDisplay::checkExitPaths() {
    
    char *blockedMap = getBlockedMap();
    
    GridPos goalPos = { mStartIndex % mFullMapD, mStartIndex / mFullMapD };
    

    mAllFamilyObjectsHaveExitPath = true;
    
    for( int i=0; i<mFamilyExitPaths.size(); i++ ) {
        delete [] *( mFamilyExitPaths.getElement( i ) );
        }
    mFamilyExitPaths.deleteAll();
    mFamilyExitPathLengths.deleteAll();
    
    
    for( int i=0; i<mNumMapSpots; i++ ) {
        if( isPropertySet( mHouseMapIDs[i], 
                           mHouseMapCellStates[i],
                           family ) ) {
            
            GridPos startPos = { i % mFullMapD, i / mFullMapD };
            
            int numStepsToGoal;
            GridPos *fullPath;
            
            char found = pathFind( mFullMapD, mFullMapD,
                                   blockedMap, 
                                   startPos, goalPos, 
                                   &numStepsToGoal,
                                   &fullPath );
            
            if( found && numStepsToGoal != 0 ) {
                mFamilyExitPaths.push_back( fullPath );
                mFamilyExitPathLengths.push_back( numStepsToGoal );
                }
            if( !found ) {
                mAllFamilyObjectsHaveExitPath = false;
                break;
                }
            }
        }
    
    delete [] blockedMap;
    }



int HouseGridDisplay::posToIndex( GridPos inPos ) {
    return mFullMapD * inPos.y + inPos.x;
    }

    
