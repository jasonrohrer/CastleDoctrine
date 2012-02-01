#include "RobHouseGridDisplay.h"

#include "minorGems/game/gameGraphics.h"
#include "minorGems/game/drawUtils.h"

#include "minorGems/graphics/openGL/KeyboardHandlerGL.h"


RobHouseGridDisplay::RobHouseGridDisplay( double inX, double inY )
        : HouseGridDisplay( inX, inY ),
          mRobberIndex( mStartIndex ) {

    memset( mVisibleMap, 0, HOUSE_D * HOUSE_D );
    }


RobHouseGridDisplay::~RobHouseGridDisplay() {
    }


    

void RobHouseGridDisplay::draw() {
    HouseGridDisplay::draw();

    setDrawColor( 0, 0, 1, 1 );
    drawSquare( getTilePos( mRobberIndex ), 0.5 * mTileRadius );

    
    // visibility overlay
    int i = 0;
    for( int y=0; y<HOUSE_D; y++ ) {
        for( int x=0; x<HOUSE_D; x++ ) {

            char visTile = mVisibleMap[i];
            
            
            if( visTile == 0 ) {
                setDrawColor( 0, 0, 0, 1 );
                doublePair tilePos = getTilePos( i );
                
                drawSquare( tilePos, mTileRadius );
                }
            else {
                }
            
            i++;
            }
        }


    }


// override so that they do nothing
void RobHouseGridDisplay::pointerMove( float inX, float inY ) {
    }

void RobHouseGridDisplay::pointerDown( float inX, float inY ) {
    }

void RobHouseGridDisplay::pointerDrag( float inX, float inY ) {
    }

void RobHouseGridDisplay::pointerUp( float inX, float inY ) {
    }


// arrow key movement
void RobHouseGridDisplay::specialKeyDown( int inKeyCode ) {
    
    int oldX = mRobberIndex % HOUSE_D;
    int oldY = mRobberIndex / HOUSE_D;
    
    int oldIndex = mRobberIndex;
    
    
    int newX = oldX;
    int newY = oldY;
    

    if( inKeyCode == MG_KEY_LEFT ) {
        if( newX > 0 ) {
            newX--;
            }
        }
    else if( inKeyCode == MG_KEY_RIGHT ) {
        if( newX < HOUSE_D - 1 ) {
            newX++;
            }
        }
    else if( inKeyCode == MG_KEY_DOWN ) {
        if( newY > 0 ) {
            newY--;
            }
        }
    else if( inKeyCode == MG_KEY_UP ) {
        if( newY < HOUSE_D - 1 ) {
            newY++;
            }
        }
    
    

    mRobberIndex = newY * HOUSE_D + newX;
    
    if( mHouseMap[ mRobberIndex ] != '0' ) {
        // hit wall, roll back to last position
        mRobberIndex = oldIndex;
        }
    
    if( mRobberIndex != oldIndex ) {
        recomputeVisibility();
        }
    }



void RobHouseGridDisplay::specialKeyUp( int inKeyCode ) {
    }



void RobHouseGridDisplay::recomputeVisibility() {

    doublePair robPos = getTilePos( mRobberIndex );


    int i = 0;
    for( int y=0; y<HOUSE_D; y++ ) {
        for( int x=0; x<HOUSE_D; x++ ) {
            
            
            doublePair pos = getTilePos( i );

            char hit = false;
            
            // steps
            for( int j=0; j<100 && !hit; j++ ) {
                double weight = j / 99.0;
                
                doublePair stepPos = add( mult( pos, weight ), 
                                          mult( robPos, 1 - weight ) );
                
                int stepIndex = getTileIndex( stepPos.x, stepPos.y );
                
                if( stepIndex != i && mHouseMap[stepIndex] != '0' ) {
                    hit = true;
                    }
                }

            if( hit ) {
                mVisibleMap[i] = 0;
                }
            else {
                mVisibleMap[i] = 1;
                }

            i++;
            }
        }
    
    }