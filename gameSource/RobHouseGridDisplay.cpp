#include "RobHouseGridDisplay.h"

#include "minorGems/game/gameGraphics.h"
#include "minorGems/game/drawUtils.h"

#include "minorGems/util/stringUtils.h"

#include "minorGems/graphics/openGL/KeyboardHandlerGL.h"


#include <math.h>



extern double frameRateFactor;



RobHouseGridDisplay::RobHouseGridDisplay( double inX, double inY )
        : HouseGridDisplay( inX, inY ),
          mRobberIndex( mStartIndex ),
          mSuccess( false ) {

    for( int i=0; i<HOUSE_D * HOUSE_D; i++ ) {
        mVisibleMap[i] = 0;
        }
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

    for( int i=0; i<HOUSE_D * HOUSE_D; i++ ) {
        mVisibleMap[i] = 0;
        }

    recomputeVisibility();
    }




/**
 * Blur convolution filter that uses a box for averaging.
 *
 * Faster accumulative implementation, as suggested by Gamasutra.
 *
 * For speed, does NOT handle edge pixels correctly
 *
 * For even more speed, does not support multiple radii (only radius=1)
 *
 *
 * Also, changed to process uchar channels (instead of doubles) for speed
 *
 * @author Jason Rohrer 
 */
class FastBoxBlurFilter { 
	
	public:
		
		/**
		 * Constructs a box filter.
		 */
		FastBoxBlurFilter();
		
		// implements the ChannelFilter interface 
        // (but for uchars and sub-regions, and a subset of pixels in that
        //  region)
		void applySubRegion( unsigned char *inChannel,
                             int *inTouchPixelIndices,
                             int inNumTouchPixels,
                             int inWidth, int inHeight );

	};
	
	
	
FastBoxBlurFilter::FastBoxBlurFilter() {	
	
	}





void FastBoxBlurFilter::applySubRegion( unsigned char *inChannel, 
                                        int *inTouchPixelIndices,
                                        int inNumTouchPixels,
                                        int inWidth, int inHeight ) {

    


    // use pointer tricks to walk through neighbor box of each pixel

    // four "corners" around box in accumulation table used to compute
    // box total
    // these are offsets to current accumulation pointer
    int cornerOffsetA = inWidth + 1;
    int cornerOffsetB = -inWidth + 1;
    int cornerOffsetC = inWidth - 1;
    int cornerOffsetD = -inWidth - 1;

    // sides around box
    int sideOffsetA = inWidth;
    int sideOffsetB = -inWidth;
    int sideOffsetC = 1;
    int sideOffsetD = -1;

    unsigned char *sourceData = new unsigned char[ inWidth * inHeight ];
    
    memcpy( sourceData, inChannel, inWidth * inHeight );
    
    
    
    // sum boxes right into passed-in channel

    for( int i=0; i<inNumTouchPixels; i++ ) {

        int pixelIndex = inTouchPixelIndices[ i ];
        

        unsigned char *sourcePointer = &( sourceData[ pixelIndex ] );

        inChannel[ pixelIndex ] =
            ( sourcePointer[ 0 ] +
              sourcePointer[ cornerOffsetA ] +
              sourcePointer[ cornerOffsetB ] +
              sourcePointer[ cornerOffsetC ] +
              sourcePointer[ cornerOffsetD ] +
              sourcePointer[ sideOffsetA ] +
              sourcePointer[ sideOffsetB ] +
              sourcePointer[ sideOffsetC ] +
              sourcePointer[ sideOffsetD ]
              ) / 9;
        }

    delete [] sourceData;
    

    return;
    }









    

void RobHouseGridDisplay::draw() {
    HouseGridDisplay::draw();

    setDrawColor( 0, 0, 1, 1 );
    drawSquare( getTilePos( mRobberIndex ), 0.5 * mTileRadius );

    
    // decay each frame
    for( int i=0; i<HOUSE_D * HOUSE_D; i++ ) {
        if( mVisibleMap[i] != 1 ) {
            
            if( mVisibleMap[i] != 0 ) {
                
                mVisibleMap[i] -= 0.02 * frameRateFactor;
                
                if( mVisibleMap[i] < 0 ) {
                    mVisibleMap[i] = 0;
                    }
                }
            }
        }


    unsigned char visPixels[ HOUSE_D * HOUSE_D ];
    

    int i=0;
    for( int y=0; y<HOUSE_D; y++ ) {
        // vertical flip

        int srcY = HOUSE_D - y - 1;
            
        for( int x=0; x<HOUSE_D; x++ ) {
            
            int srcI = srcY * HOUSE_D + x;

            visPixels[i] = 
                (unsigned char)( lrint( 255 - 255 * mVisibleMap[ srcI ] ) );
            i++;
            }
        }
    
    /*
    for( int i=0; i<HOUSE_D * HOUSE_D; i++ ) {
        visPixels[i] = (unsigned char)( lrint( 255 - 255 * mVisibleMap[i] ) );
        }
    */
    


    int blowUpFactor = 4;
    int blownUpSize = HOUSE_D * blowUpFactor;

    int numBlowupPixels = blownUpSize * blownUpSize;
    
    // opt:  do all this processing with uchars instead of doubles
    unsigned char *fullGridChannelsBlownUpAlpha =
        new unsigned char[ numBlowupPixels ];

    int *touchIndices = new int[ numBlowupPixels ];

    int numTouched = 0;

    memset( fullGridChannelsBlownUpAlpha, 0, numBlowupPixels );

    
    for( int y=0; y<HOUSE_D; y++ ) {
        for( int x=0; x<HOUSE_D; x++ ) {
    
            unsigned char alphaValue = visPixels[ y * HOUSE_D + x ];

            for( int blowUpY= y * blowUpFactor; 
                 blowUpY< y * blowUpFactor + blowUpFactor; 
                 blowUpY++ ) {

                for( int blowUpX= x * blowUpFactor; 
                     blowUpX< x * blowUpFactor + blowUpFactor; 
                     blowUpX++ ) {
                

                    int imageIndex = blowUpY * blownUpSize + blowUpX;
                    
                    fullGridChannelsBlownUpAlpha[ imageIndex ] = alphaValue;
                    
                    if( blowUpY > 0 && blowUpY < blownUpSize - 1
                        &&
                        blowUpX > 0 && blowUpX < blownUpSize - 1 ) {
                        
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

    
    for( int f=0; f<10; f++ ) {
        
        filter2.applySubRegion( fullGridChannelsBlownUpAlpha, 
                                touchIndices,
                                numTouched,
                                blownUpSize, blownUpSize );
        }
    


    SpriteHandle visSprite = 
        fillSpriteAlphaOnly( fullGridChannelsBlownUpAlpha, 
                             blownUpSize, blownUpSize );
    
    delete [] fullGridChannelsBlownUpAlpha;
    delete [] touchIndices;

    doublePair spritePos = { 0, 0 };
    
    setDrawColor( 0, 0, 0, 1 );

    toggleLinearMagFilter( true );
    drawSprite( visSprite, spritePos, 1.0 * 2 * mTileRadius / blowUpFactor );
    toggleLinearMagFilter( false );
    
    freeSprite( visSprite );

    /*
    // visibility overlay
    int i = 0;
    for( int y=0; y<HOUSE_D; y++ ) {
        for( int x=0; x<HOUSE_D; x++ ) {

            float visTile = mVisibleMap[i];
            
            
            if( visTile < 1 ) {
                setDrawColor( 0, 0, 0, 1 - visTile );
                doublePair tilePos = getTilePos( i );
                
                drawSquare( tilePos, mTileRadius );
                }
            else {
                }
            
            i++;
            }
        }
    */

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
                if( mVisibleMap[i] == 1 ) {
                    // start decay, since no longer visible
                    mVisibleMap[i] = 0.99;
                    }
                }
            else {
                mVisibleMap[i] = 1;
                }

            i++;
            }
        }
    
    }
