#include "RobHouseGridDisplay.h"

#include "minorGems/game/gameGraphics.h"
#include "minorGems/game/drawUtils.h"

#include "minorGems/util/stringUtils.h"

#include "minorGems/graphics/openGL/KeyboardHandlerGL.h"


#include <math.h>



extern double frameRateFactor;



RobHouseGridDisplay::RobHouseGridDisplay( double inX, double inY )
        : HouseGridDisplay( inX, inY ),
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
    mSuccess = false;
    clearMoveList();

    HouseGridDisplay::setHouseMap( inHouseMap );    

    for( int i=0; i<HOUSE_D * HOUSE_D * VIS_BLOWUP * VIS_BLOWUP; i++ ) {
        mVisibleMap[i] = 255;
        mTargetVisibleMap[i] = false;
        }
    
    mRobberIndex = mStartIndex;
    
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

    int robSubIndex = fullToSub( mRobberIndex );
    if( robSubIndex != -1 ) {    
        setDrawColor( 0, 0, 1, 1 );
        drawSquare( getTilePos( robSubIndex ), 0.5 * mTileRadius );
        }
    
    
    // decay each frame
    for( int i=0; i<HOUSE_D * HOUSE_D * VIS_BLOWUP * VIS_BLOWUP; i++ ) {
        if( mTargetVisibleMap[i] ) {
            // wants to move toward visible
            if( mVisibleMap[i] != 0 ) {
                unsigned char oldValue = mVisibleMap[i];

                // revealing new areas happens faster than shrouding
                // no-longer-seen areas
                mVisibleMap[i] -= lrint( 10 * frameRateFactor );
                
                // watch for wrap-around!
                if( mVisibleMap[i] > oldValue ) {
                    mVisibleMap[i] = 0;
                    }

                }
            }
        else {
            // wants to move toward invisible
            
            if( mVisibleMap[i] != 255 ) {
                
                unsigned char oldValue = mVisibleMap[i];

                mVisibleMap[i] += lrint( 5 * frameRateFactor );
                
                // watch for wrap-around!
                if( mVisibleMap[i] < oldValue ) {
                    mVisibleMap[i] = 255;
                    }
                }
            }
        }


    int blowUpFactor = 2;
    int blownUpSize = HOUSE_D * VIS_BLOWUP * blowUpFactor;

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

    
    for( int y=0; y<HOUSE_D * VIS_BLOWUP; y++ ) {
        
        for( int x=0; x<HOUSE_D * VIS_BLOWUP; x++ ) {
            
            unsigned char alphaValue = 
                mVisibleMap[ y * HOUSE_D * VIS_BLOWUP + x ];

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

    for( int f=0; f<10; f++ ) {
        
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


    SpriteHandle visSprite = 
        fillSpriteAlphaOnly( fullGridChannelsBlownUpAlpha, 
                             paddedSize, paddedSize );
    
    delete [] fullGridChannelsBlownUpAlpha;
    delete [] touchIndices;

    doublePair spritePos = { 0, 0 };
    
    setDrawColor( 0, 0, 0, 1 );

    toggleLinearMagFilter( true );
    drawSprite( visSprite, spritePos, 
                1.0 * 2 * mTileRadius / ( blowUpFactor * VIS_BLOWUP ) );
    toggleLinearMagFilter( false );
    
    freeSprite( visSprite );
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
    
    int oldX = mRobberIndex % mFullMapD;
    int oldY = mRobberIndex / mFullMapD;
    
    int oldIndex = mRobberIndex;
    
    
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
    
    

    mRobberIndex = newY * mFullMapD + newX;
    
    if( mHouseMapIDs[ mRobberIndex ] != 0 ) {
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

    int robSubIndex = fullToSub( mRobberIndex );
    
    if( robSubIndex == -1 ) {
        // robber out of bounds
        // nothing visible
        for( int i=0; i<HOUSE_D * HOUSE_D * VIS_BLOWUP * VIS_BLOWUP; i++ ) {
            mTargetVisibleMap[i] = false;
            }
        return;
        }
    

    doublePair robPos = getTilePos( fullToSub( mRobberIndex ) );

    
    doublePair cornerPos = getTilePos( 0 );

    
    int i = 0;
    for( int y=0; y<HOUSE_D * VIS_BLOWUP; y++ ) {
        for( int x=0; x<HOUSE_D * VIS_BLOWUP; x++ ) {
            
            int flipY = HOUSE_D * VIS_BLOWUP - y - 1;

            doublePair visPos = 
                { cornerPos.x +  
                  ( x / (double)VIS_BLOWUP ) 
                  * 2 * mTileRadius - mTileRadius / 2,
                  
                  cornerPos.y +  
                  ( flipY / (double)VIS_BLOWUP ) 
                  * 2 * mTileRadius - mTileRadius / 2 };
            

            int visTileIndex = getTileIndex( visPos.x, visPos.y );

            // steps
            int numSteps = lrint( distance( visPos, robPos ) * 8 );
            char hit = false;
            
            // skip last few steps so that we show borders of visible walls
            for( int j=1; j<numSteps - 1 && !hit; j++ ) {
                double weight = j / (double)numSteps;
                
                doublePair stepPos = add( mult( visPos, weight ), 
                                          mult( robPos, 1 - weight ) );
                
                int stepIndex = getTileIndex( stepPos.x, stepPos.y );
                
                if( //stepIndex != visTileIndex && 
                    mHouseSubMapIDs[stepIndex] != 0 ) {
                    
                    hit = true;
                    }
                }
            
            if( hit ) {
                mTargetVisibleMap[i] = false;
                }
            else {
                mTargetVisibleMap[i] = true;
                }

            i++;
            }
        }
    
    }
