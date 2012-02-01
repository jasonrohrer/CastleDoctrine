#include "RobHouseGridDisplay.h"

#include "minorGems/game/gameGraphics.h"
#include "minorGems/game/drawUtils.h"

#include "minorGems/graphics/openGL/KeyboardHandlerGL.h"


RobHouseGridDisplay::RobHouseGridDisplay( double inX, double inY )
        : HouseGridDisplay( inX, inY ),
          mRobberIndex( mStartIndex ) {

    }


RobHouseGridDisplay::~RobHouseGridDisplay() {
    }


    

void RobHouseGridDisplay::draw() {
    HouseGridDisplay::draw();

    setDrawColor( 0, 0, 1, 1 );
    drawSquare( getTilePos( mRobberIndex ), 0.5 * mTileRadius );
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
    }

void RobHouseGridDisplay::specialKeyUp( int inKeyCode ) {
    }

