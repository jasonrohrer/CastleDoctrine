#include "HouseGridDisplay.h"


#include "minorGems/util/stringUtils.h"
#include "minorGems/game/gameGraphics.h"
#include "minorGems/game/drawUtils.h"

#include <math.h>




HouseGridDisplay::HouseGridDisplay( double inX, double inY )
        : PageComponent( inX, inY ),
          mHouseMap( NULL ), 
          mHighlightIndex( -1 ), mTileRadius( 0.375 ) {

    mStartIndex = HOUSE_D * ( HOUSE_D / 2 );
    mGoalIndex = HOUSE_D * ( HOUSE_D / 2 ) + HOUSE_D - 1;
    }



HouseGridDisplay::~HouseGridDisplay() {
    if( mHouseMap != NULL ) {
        delete [] mHouseMap;
        }
    if( mHouseMapIDs != NULL ) {
        delete [] mHouseMapIDs;
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
    
    mNumMapSpots = numTokens;

    mHouseMapIDs = new int[ mNumMapSpots ];

    for( int i=0; i<mNumMapSpots; i++ ) {
        sscanf( tokens[i], "%d", &( mHouseMapIDs[i] ) );
        
        delete [] tokens[i];
        }
    
    delete [] tokens;
    }



char *HouseGridDisplay::getHouseMap() {
    if( mHouseMap == NULL ) {
        return NULL;
        }
    else {
        
        char **parts = new char*[ mNumMapSpots ];
        

        for( int i=0; i<mNumMapSpots; i++ ) {
            parts[i] = autoSprintf( "%d", mHouseMapIDs[i] );
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

            int houseTile = mHouseMapIDs[i];
            
            
            if( houseTile == 0 ) {
                setDrawColor( 0.25, 0.25, 0.25, 1 );
                }
            else {
                setDrawColor( 1, 0, 0, 1 );
                }
            doublePair tilePos = getTilePos( i );

            drawSquare( tilePos, mTileRadius );
            
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

    setDrawColor( 0, 1, 0, 1 );
    drawSquare( getTilePos( mStartIndex ), 0.75 * mTileRadius );

    setDrawColor( 1, 1, 0, 1 );
    drawSquare( getTilePos( mGoalIndex ), 0.75 * mTileRadius );

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
    
        int old = mHouseMapIDs[ index ];
        
        if( old == 0 ) {
            mHouseMapIDs[ index ] = 1;
            fireActionPerformed( this );
            }
        else if( old == 1 ) {
            mHouseMapIDs[ index ] = 0;
            fireActionPerformed( this );
            }
        }
    }
