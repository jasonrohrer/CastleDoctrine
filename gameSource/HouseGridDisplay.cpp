#include "HouseGridDisplay.h"


#include "minorGems/util/stringUtils.h"
#include "minorGems/game/gameGraphics.h"
#include "minorGems/game/drawUtils.h"

#include <math.h>



HouseGridDisplay::HouseGridDisplay( double inX, double inY )
        : mX( inX ), mY( inY ), mHouseMap( NULL ), 
          mHighlightIndex( -1 ), mTileRadius( 0.5 ) {
    }



HouseGridDisplay::~HouseGridDisplay() {
    if( mHouseMap != NULL ) {
        delete [] mHouseMap;
        }
    }



void HouseGridDisplay::setHouseMap( char *inHouseMap ) {
    if( mHouseMap != NULL ) {
        delete [] mHouseMap;
        }
    mHouseMap = stringDuplicate( inHouseMap );
    }



char *HouseGridDisplay::getHouseMap() {
    if( mHouseMap == NULL ) {
        return NULL;
        }
    else {
        return stringDuplicate( mHouseMap );
        }
    }

    

void HouseGridDisplay::step() {
    }



int HouseGridDisplay::getTileIndex( double inX, double inY ) {

    double relX = inX - mX;
    double relY = inY - mY;
    
    int x = lrint(
        ( relX + ( 16 * mTileRadius - mTileRadius ) ) / ( 2 * mTileRadius )
        );
    
    int y = lrint( 
        ( relY + ( 16 * mTileRadius - mTileRadius ) ) / ( 2 * mTileRadius )
        );
    
    if( x >= 0 && x < 16 
        &&
        y >= 0 && y < 16 ) {
        
        return y * 16 + x;
        }
    else {
        return -1;
        }    
    }



doublePair HouseGridDisplay::getTilePos( int inIndex ) {
    
    int x = inIndex % 16;
    int y = inIndex / 16;
    


    doublePair tilePos = {  x * 2 * mTileRadius - 
                              ( 16 * mTileRadius - mTileRadius ), 
                            y * 2 * mTileRadius - 
                              ( 16 * mTileRadius - mTileRadius ) };

    tilePos.x += mX;
    tilePos.y += mY;
    
    return tilePos;
    }





void HouseGridDisplay::draw() {
    if( mHouseMap == NULL ) {
        return;
        }



    // draw house
    int i = 0;
    for( int y=0; y<16; y++ ) {
        for( int x=0; x<16; x++ ) {
    
            char houseTile = mHouseMap[i];
            
            
            if( houseTile == '0' ) {
                setDrawColor( 0.25, 0.25, 0.25, 1 );
                }
            else {
                setDrawColor( 1, 0, 0, 1 );
                }
            doublePair tilePos = getTilePos( i );

            drawSquare( tilePos, mTileRadius );
            
            if( mHighlightIndex == i ) {
                setDrawColor( 1, 1, 0, 0.5 );
                drawSquare( tilePos, mTileRadius ); 
                }

            i++;
            }
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

    mHighlightIndex = -1;
    
    int index = getTileIndex( inX, inY );
    
    if( index != -1 ) {
    
        char old = mHouseMap[ index ];
        
        if( old == '0' ) {
            mHouseMap[ index ] = '1';
            fireActionPerformed( this );
            }
        }
    }
