#include "HouseGridDisplay.h"


#include "minorGems/util/stringUtils.h"
#include "minorGems/game/gameGraphics.h"
#include "minorGems/game/drawUtils.h"

#include "minorGems/graphics/openGL/KeyboardHandlerGL.h"


#include <math.h>


#include "houseObjects.h"


#define GOAL_ID  999
#define START_ID  997



HouseGridDisplay::HouseGridDisplay( double inX, double inY )
        : PageComponent( inX, inY ),
          mHouseMap( NULL ), 
          mHouseMapIDs( NULL ),
          mHouseMapCellStates( NULL ),
          mSubMapOffsetX( 0 ),
          mSubMapOffsetY( 0 ),
          mHouseSubMapIDs( new int[ HOUSE_D * HOUSE_D ] ),
          mHouseSubMapCellStates( new int[ HOUSE_D * HOUSE_D ] ),
          mHighlightIndex( -1 ), mTileRadius( 0.4375 ),
          mGoalSet( false ),
          mWallShadowSprite( NULL ) {

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

    delete [] mHouseSubMapIDs;
    delete [] mHouseSubMapCellStates;

    if( mWallShadowSprite != NULL ) {
        freeSprite( mWallShadowSprite );
        mWallShadowSprite = NULL;
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
    
    mNumMapSpots = numTokens;
    mFullMapD = (int)sqrt( mNumMapSpots );
    
    mHouseMapIDs = new int[ mNumMapSpots ];
    mHouseMapCellStates = new int[ mNumMapSpots ];
    
    for( int i=0; i<mNumMapSpots; i++ ) {
        int numRead = sscanf( tokens[i], "%d:%d", 
                              &( mHouseMapIDs[i] ), 
                              &( mHouseMapCellStates[i] ) );
        
        if( numRead < 2 ) {
            // no ":value" present after ID
            // use default
            mHouseMapCellStates[i] = 1;
        
            if( numRead == 0 ) {
                // reading ID failed?
                
                // default
                mHouseMapIDs[i] = 0;
                }
            }
        
        if( mHouseMapIDs[i] == GOAL_ID ) {
            mGoalIndex = i;
            mGoalSet = true;
            }
        

        delete [] tokens[i];
        }
    
    delete [] tokens;


    // center vertically, far left
    setVisibleOffset( 0, ( mFullMapD - HOUSE_D ) / 2 );

    mStartIndex = mFullMapD * ( mFullMapD / 2 );
    
    mRobberIndex = mStartIndex;
    }



char *HouseGridDisplay::getHouseMap() {
    if( mHouseMap == NULL ) {
        return NULL;
        }
    else {
        
        char **parts = new char*[ mNumMapSpots ];
        

        for( int i=0; i<mNumMapSpots; i++ ) {
            if( mHouseMapCellStates[i] != 1 ) {
                // not default state, include state
                parts[i] = autoSprintf( "%d:%d", 
                                        mHouseMapIDs[i],
                                        mHouseMapCellStates[i] );
                }
            else {
                // default state, skip including it
                parts[i] = autoSprintf( "%d", mHouseMapIDs[i] );
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



char HouseGridDisplay::isGoalSet() {
    return mGoalSet;
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



int HouseGridDisplay::getTileNeighbor( int inIndex, int inNeighbor ) {
    int fullIndex = subToFull( inIndex );
    
    int fullY = fullIndex / mFullMapD;
    int fullX = fullIndex % mFullMapD;
    
    int dX[4] = { 0, 1, 0, -1 };
    int dY[4] = { 1, 0, -1, 0 };

    int nY = fullY + dY[ inNeighbor ];

    int nX = fullX + dX[ inNeighbor ];
    
    
    if( nY < 0 || nY >= mFullMapD
        ||
        nX < 0 || nX >= mFullMapD ) {
        
        // out of bounds, exterior wall
        return 998;
        }

    return mHouseMapIDs[ nY * mFullMapD + nX ];
    }





void HouseGridDisplay::drawTiles( char inNonBlockingOnly ) {
    int i = 0;
    for( int y=0; y<HOUSE_D; y++ ) {
        for( int x=0; x<HOUSE_D; x++ ) {

            int houseTile = mHouseSubMapIDs[i];
            
            char blockingProperty = isSubMapPropertySet( i, blocking );
                                                   

            if( inNonBlockingOnly && blockingProperty ) {
                // skip this blocking tile
                
                i++;
                continue;
                }
            else if( ! inNonBlockingOnly && ! blockingProperty ) {
                // skip this non-blocking tile
                i++;
                continue;
                }
            
            
            doublePair tilePos = getTilePos( i );
 
            int numOrientations = 0;
            
            int orientationIndex = 0;

            numOrientations = getNumOrientations( houseTile, 0 );
            
            
                
            if( numOrientations == 16 ) {
                // full binary LBRT flags based on neighbors of same type 
                
                int neighborsEqual[4] = { 0, 0, 0, 0 };
                
                for( int n=0; n<4; n++ ) {
                    if( getTileNeighbor( i, n ) == houseTile ) {
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
                    if( getTileNeighbor( i, n ) != 0 ) {
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
                }
            else if( numOrientations == 2 ) {
                
                if( getTileNeighbor( i, 0 ) != 0 && 
                    getTileNeighbor( i, 2 ) != 0 ) {
                    // blocked on top and bottom
                
                    // vertical orientation
                    orientationIndex = 0;
                    }
                else {
                    // horizontal 
                    orientationIndex = 1;
                    }
                }
            else if( numOrientations == 1 ) {
                orientationIndex = 0;
                }
            
            

                

            
            // draw empty floor, even under non-blocking objects
            if( inNonBlockingOnly && !blockingProperty  ) {
                
                setDrawColor( 1, 1, 1, 1 );
                
                SpriteHandle sprite = 
                    getObjectSprite( 0, 0, 0 );
                
                drawSprite( sprite, tilePos, 1.0/16.0 );
                
                /*
                if( houseTile == GOAL_ID ) {
                    // draw goal here, so highlight can draw over it

                    setDrawColor( 1, 1, 0, 1 );
                    drawSquare( tilePos, 0.75 * mTileRadius );
                    }
                */
                }
            /*
            else if( houseTile == GOAL_ID ) {
                // goal position found
                mGoalIndex = subToFull( i );

                // draw empty floor under it
                setDrawColor( 0.25, 0.25, 0.25, 1 );
                drawSquare( tilePos, mTileRadius );
                }
            */
            if( inNonBlockingOnly && 
                ! blockingProperty && 
                houseTile != 0 ) {
                
                // now draw tile itself, on top of floor
                setDrawColor( 1, 1, 1, 1 );
                
                SpriteHandle sprite = getObjectSprite( houseTile, 
                                                       orientationIndex, 0 );
                
                drawSprite( sprite, tilePos, 1.0/16.0 );
                }
            else if( !inNonBlockingOnly && houseTile != 0 ) {
                // now draw non-blocking objects on top of floor
                setDrawColor( 1, 1, 1, 1 );
                
                SpriteHandle sprite = getObjectSprite( houseTile, 
                                                       orientationIndex, 0 );
                
                drawSprite( sprite, tilePos, 1.0/16.0 );
                }
            
            

            
            // no highlight over start, robber, or permanents
            int fullI = subToFull( i );
            if( mHighlightIndex == i &&
                fullI != mStartIndex &&
                fullI != mRobberIndex &&
                ! isSubMapPropertySet( i, permanent ) ) {

                if( !mGoalSet ) {
                    // ghost of goal for placement
                    setDrawColor( 1, 1, 1, 0.35 );

                    SpriteHandle sprite = getObjectSprite( GOAL_ID, 
                                                           0, 
                                                           0 );
                
                    drawSprite( sprite, tilePos, 1.0/16.0 );
                    }
                else if( houseTile == 0 ) {
                    setDrawColor( 1, 1, 1, 0.35 );
                
                    SpriteHandle sprite = getObjectSprite( 1, 
                                                           orientationIndex,
                                                           0 );
                    
                    drawSprite( sprite, tilePos, 1.0/16.0 );
                    }
                else {
                    setDrawColor( 0, 0, 0, 0.35 );
                    
                    SpriteHandle sprite = getObjectSprite( houseTile, 
                                                           orientationIndex, 
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
    drawSquare( center, HOUSE_D * mTileRadius + mTileRadius / 4 );
    
    
    // draw house
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

    drawTiles( false );
    



    /*
    if( mGoalSet ) {    
        int goalSubIndex = fullToSub( mGoalIndex );
        
        if( goalSubIndex != -1 ) {    
            setDrawColor( 1, 1, 0, 1 );
            drawSquare( getTilePos( goalSubIndex ), 0.75 * mTileRadius );
            }
        }
    */
    

    int robSubIndex = fullToSub( mRobberIndex );
    if( robSubIndex != -1 ) {    
        setDrawColor( 0, 0, 1, 1 );
        drawSquare( getTilePos( robSubIndex ), (4/7.0 ) * mTileRadius );
        }
    }



void HouseGridDisplay::pointerMove( float inX, float inY ) {
    mHighlightIndex = getTileIndex( inX, inY );
    }

void HouseGridDisplay::pointerDown( float inX, float inY ) {
    mHighlightIndex = getTileIndex( inX, inY );
    }

void HouseGridDisplay::pointerDrag( float inX, float inY ) {
    mHighlightIndex = getTileIndex( inX, inY );
    }

void HouseGridDisplay::pointerUp( float inX, float inY ) {

    int index = getTileIndex( inX, inY );
    
    if( index == -1 ) {
        return;
        }
    
    if( isSubMapPropertySet( index, permanent ) ) {
        // ignore mouse activity on permanent tiles
        return;
        }

    if( subToFull( index ) == mRobberIndex ) {
        // don't allow clicks on current robber position
        return;
        }
    
        
    mHighlightIndex = index;

    int fullIndex = subToFull( index );
    

    if( !mGoalSet ) {
        // goal set here
        mHouseSubMapIDs[ index ] = GOAL_ID;
        mGoalIndex = fullIndex;
        mGoalSet = true;
        copySubCellBack( index );
        fireActionPerformed( this );
        }
    else if( fullIndex != mStartIndex && fullIndex != mGoalIndex ) {
    
        int old = mHouseSubMapIDs[ index ];
        
        if( old == 0 ) {
            mHouseSubMapIDs[ index ] = 1;
            copySubCellBack( index );
            fireActionPerformed( this );
            }
        else if( old == 1 ) {
            mHouseSubMapIDs[ index ] = 0;
            copySubCellBack( index );
            fireActionPerformed( this );
            }
        }
    else if( mGoalSet && fullIndex == mGoalIndex ) {
        // goal moving!
        mHouseSubMapIDs[ index ] = 0;
        mGoalSet = false;
        copySubCellBack( index );
        fireActionPerformed( this );
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

        fireActionPerformed( this );
        }
    
    }



void HouseGridDisplay::specialKeyUp( int inKeyCode ) {
    }



void HouseGridDisplay::moveRobber( int inNewIndex ) {
    
    if( mRobberIndex != inNewIndex ) {
        
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




void HouseGridDisplay::setVisibleOffset( int inXOffset, int inYOffset ) {
    for( int y=0; y<HOUSE_D; y++ ) {
        int bigY = y + inYOffset;

        for( int x=0; x<HOUSE_D; x++ ) {
            int bigX = x + inXOffset;

            int subIndex = y * HOUSE_D + x;
            
            int bigIndex = bigY * mFullMapD + bigX;

            mHouseSubMapIDs[ subIndex ] = 
                mHouseMapIDs[ bigIndex ];
            
            mHouseSubMapCellStates[ subIndex ] = 
                mHouseMapCellStates[ bigIndex ];
            }
        }
    
    mSubMapOffsetX = inXOffset;
    mSubMapOffsetY = inYOffset;

    recomputeWallShadows();
    }




#include "FastBoxBlurFilter.h"



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
            

            if( isSubMapPropertySet( subIndex, blocking ) ) {
                // all blocking objects leave shadows
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
