#include "HouseGridDisplay.h"


#include "minorGems/util/stringUtils.h"
#include "minorGems/game/gameGraphics.h"
#include "minorGems/game/drawUtils.h"

#include "minorGems/graphics/openGL/KeyboardHandlerGL.h"


#include <math.h>


#include "houseObjects.h"



HouseGridDisplay::HouseGridDisplay( double inX, double inY )
        : PageComponent( inX, inY ),
          mHouseMap( NULL ), 
          mHouseMapIDs( NULL ),
          mHouseMapCellStates( NULL ),
          mSubMapOffsetX( 0 ),
          mSubMapOffsetY( 0 ),
          mHouseSubMapIDs( new int[ HOUSE_D * HOUSE_D ] ),
          mHouseSubMapCellStates( new int[ HOUSE_D * HOUSE_D ] ),
          mHighlightIndex( -1 ), mTileRadius( 0.4375 ) {

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
        

        delete [] tokens[i];
        }
    
    delete [] tokens;


    // center vertically, far left
    setVisibleOffset( 0, ( mFullMapD - HOUSE_D ) / 2 );

    mStartIndex = mFullMapD * ( mFullMapD / 2 );
    mGoalIndex = mFullMapD * ( mFullMapD / 2 ) + mFullMapD - 1;
    
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





void HouseGridDisplay::draw() {
    if( mHouseMap == NULL ) {
        return;
        }

    
    // draw border
    doublePair center = { 0, 0 };
    
    setDrawColor( 1, 1, 1, 1 );
    drawSquare( center, HOUSE_D * mTileRadius + mTileRadius / 4 );
    

    // draw house
    int i = 0;
    for( int y=0; y<HOUSE_D; y++ ) {
        for( int x=0; x<HOUSE_D; x++ ) {

            int houseTile = mHouseSubMapIDs[i];
            
            
            doublePair tilePos = getTilePos( i );
 
            
            if( houseTile == 0 ) {
                setDrawColor( 0.25, 0.25, 0.25, 1 );
                drawSquare( tilePos, mTileRadius );
                }
            else {
                setDrawColor( 1, 1, 1, 1 );
                
                SpriteHandle sprite = getObjectSprite( houseTile );
                
                drawSprite( sprite, tilePos, 1.0/16.0 );
                }
            
            

            
            
            if( mHighlightIndex == i ) {
                if( houseTile == '0' ) {
                    setDrawColor( 1, 0, 0, 0.35 );
                    }
                else {
                    setDrawColor( 0, 0, 0, 0.35 );
                    }
                drawSquare( tilePos, mTileRadius ); 
                }

            i++;
            }
        }

    int startSubIndex = fullToSub( mStartIndex );
    if( startSubIndex != -1 ) {
        setDrawColor( 0, 1, 0, 1 );
        drawSquare( getTilePos( startSubIndex ), 0.75 * mTileRadius );
        }

    int goalSubIndex = fullToSub( mGoalIndex );
    
    if( goalSubIndex != -1 ) {    
        setDrawColor( 1, 1, 0, 1 );
        drawSquare( getTilePos( goalSubIndex ), 0.75 * mTileRadius );
        }
    

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
    
    mHighlightIndex = index;

    if( index != -1 && index != mStartIndex && index != mGoalIndex ) {
    
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
    
    if( mHouseMapIDs[ newRobberIndex ] == 0 ) {
        // did not hit wall, can actually move here
        moveRobber( newRobberIndex );
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
    }
