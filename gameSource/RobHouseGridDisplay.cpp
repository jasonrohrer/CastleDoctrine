#include "RobHouseGridDisplay.h"

#include "minorGems/game/gameGraphics.h"
#include "minorGems/game/drawUtils.h"

#include "minorGems/util/stringUtils.h"

#include "minorGems/graphics/openGL/KeyboardHandlerGL.h"


RobHouseGridDisplay::RobHouseGridDisplay( double inX, double inY )
        : HouseGridDisplay( inX, inY ),
          mRobberIndex( mStartIndex ),
          mSuccess( false ) {

    memset( mVisibleMap, 0, HOUSE_D * HOUSE_D );
    }



RobHouseGridDisplay::~RobHouseGridDisplay() {
    clearMoveList();
    }



void RobHouseGridDisplay::clearMoveList() {
    for( int i=0; i<mMoveList.size(); i++ ) {
        delete [] *( mMoveList.getElement( i ) );
        }
    mMoveList.deleteAll();
    }



char RobHouseGridDisplay::getSuccess() {
    return mSuccess;
    }



char *RobHouseGridDisplay::getMoveList() {
    if( mMoveList.size() == 0 ) {
        return stringDuplicate( "" );
        }
    
    char **moveArray = mMoveList.getElementArray();
    
    char *moveString = join( moveArray, mMoveList.size(), "_" );
    
    delete [] moveArray;

    return moveString;
    }


void RobHouseGridDisplay::setHouseMap( char *inHouseMap ) {
    mRobberIndex = mStartIndex;
    mSuccess = false;
    clearMoveList();

    HouseGridDisplay::setHouseMap( inHouseMap );    
    recomputeVisibility();
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

        // a move!
        mMoveList.push_back( autoSprintf( "m%d", mRobberIndex ) );
        }

    if( mRobberIndex == mGoalIndex ) {
        mSuccess = true;
        fireActionPerformed( this );
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

            // corners, plus center
            // if ANY of these is visible, count
            // whole tile as visible
            doublePair vertOffsets[5] = 
                { { 0, 0 },
                  { -mTileRadius, -mTileRadius },
                  { -mTileRadius, +mTileRadius },
                  { +mTileRadius, +mTileRadius },
                  { +mTileRadius, -mTileRadius } };
            
            char hitForAll = true;
            
            for( int v=0; v<5 && hitForAll; v++ ) {
                
                doublePair vertPos = add( pos, vertOffsets[v] );

                char hit = false;
                
                // steps
                int numSteps = lrint( distance( vertPos, robPos ) * 2 );
                

                for( int j=1; j<numSteps && !hit; j++ ) {
                    double weight = j / (double)numSteps;
                    
                    doublePair stepPos = add( mult( vertPos, weight ), 
                                              mult( robPos, 1 - weight ) );
                    
                    int stepIndex = getTileIndex( stepPos.x, stepPos.y );
                    
                    if( stepIndex != i && mHouseMap[stepIndex] != '0' ) {
                        hit = true;
                        }
                    }

                if( !hit ) {
                    hitForAll = false;
                    }                
                }
            

            if( hitForAll ) {
                mVisibleMap[i] = 0;
                }
            else {
                mVisibleMap[i] = 1;
                }

            i++;
            }
        }
    
    }
