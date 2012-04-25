#include "RobHouseGridDisplay.h"

#include "minorGems/game/gameGraphics.h"
#include "minorGems/game/drawUtils.h"

#include "minorGems/util/stringUtils.h"


#include "FastBoxBlurFilter.h"
#include "houseObjects.h"
#include "houseTransitions.h"

#include <math.h>



extern double frameRateFactor;



RobHouseGridDisplay::RobHouseGridDisplay( double inX, double inY )
        : HouseGridDisplay( inX, inY ),
          mSuccess( false ),
          mDead( false ),
          mDeathSourceID( -1 ) {

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



char RobHouseGridDisplay::getDead() {
    return mDead;
    }



int RobHouseGridDisplay::getDeathSourceID() {
    return mDeathSourceID;
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
    mDead = false;

    clearMoveList();

    HouseGridDisplay::setHouseMap( inHouseMap );    

    for( int i=0; i<mNumMapSpots; i++ ) {
        
        if( ! isPropertySet( mHouseMapIDs[i], mHouseMapCellStates[i],
                             destroyed )
            &&
            ! isPropertySet( mHouseMapIDs[i], mHouseMapCellStates[i],
                             stuck ) ) {
            
            // switch all unstuck, not-destroyed objects
            // to "1" state for presentation to robber
    
            mHouseMapCellStates[i] = 1;
            }

        // same for mobile objects
        if( mHouseMapMobileIDs[i] != 0 
            &&
            ! isPropertySet( mHouseMapMobileIDs[i], 
                             mHouseMapMobileCellStates[i],
                             destroyed )
            &&
            ! isPropertySet( mHouseMapMobileIDs[i], 
                             mHouseMapMobileCellStates[i],
                             stuck ) ) {
            
            mHouseMapMobileCellStates[i] = 1;
            }
        
        // leave other states alone
        // example:  walls that were burned down during a previous, successful
        // robbery, but not repaired by owner yet
        }
    
    for( int i=0; i<HOUSE_D * HOUSE_D * VIS_BLOWUP * VIS_BLOWUP; i++ ) {
        mVisibleMap[i] = 255;
        mTargetVisibleMap[i] = false;
        }

    // initial transitions (like for power that starts out on, etc)
    applyTransitionsAndProcess();
    }



void RobHouseGridDisplay::applyTransitionsAndProcess() {
    applyTransitions( mHouseMapIDs, mHouseMapCellStates, 
                      mHouseMapMobileIDs, mHouseMapMobileCellStates,
                      mFullMapD, mFullMapD,
                      mRobberIndex );

    copyAllIntoSubCells();

    if( isPropertySet( mHouseMapIDs[ mRobberIndex ], 
                       mHouseMapCellStates[ mRobberIndex ], deadly ) ) {
        
        // robber killed by static object
        mDead = true;
        mDeathSourceID = mHouseMapIDs[ mRobberIndex ];
        mSuccess = false;
        }
    else if( isPropertySet( mHouseMapMobileIDs[ mRobberIndex ], 
                            mHouseMapMobileCellStates[ mRobberIndex ], 
                            deadly ) ) {

        // robber killed by mobile
        mDead = true;
        mDeathSourceID = mHouseMapMobileIDs[ mRobberIndex ];
        mSuccess = false;
        }

    
    
    recomputeVisibility();
    }













    

void RobHouseGridDisplay::draw() {
    HouseGridDisplay::draw();

    
    
    
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
    

    // clear border again, but leave one extra pixel for map-edge shrouding
    for( int y=0; y<paddedSize; y++ ) {
        for( int x=0; x<blowUpBorder-1; x++ ) {
            fullGridChannelsBlownUpAlpha[ y * paddedSize + x ] = 0;
            }
        for( int x=paddedSize - blowUpBorder + 1; x<paddedSize; x++ ) {
            fullGridChannelsBlownUpAlpha[ y * paddedSize + x ] = 0;
            }
        }
    for( int x=0; x<paddedSize; x++ ) {
        for( int y=0; y<blowUpBorder-1; y++ ) {
            fullGridChannelsBlownUpAlpha[ y * paddedSize + x ] = 0;
            }
        for( int y=paddedSize - blowUpBorder + 1; y<paddedSize; y++ ) {
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



void RobHouseGridDisplay::moveRobber( int inNewIndex ) {
    if( mDead ) {
        // can't move anymore
        return;
        }

    HouseGridDisplay::moveRobber( inNewIndex );

    applyTransitionsAndProcess();
    
    // a move!
    mMoveList.push_back( autoSprintf( "m%d", mRobberIndex ) );
    
    if( mRobberIndex == mGoalIndex ) {
        mSuccess = true;
        fireActionPerformed( this );
        }
    }



void RobHouseGridDisplay::setVisibleOffset( int inXOffset, int inYOffset ) {
    int xExtra = inXOffset - mSubMapOffsetX;
    int yExtra = inYOffset - mSubMapOffsetY;
    
    if( xExtra != 0 || yExtra != 0 ) {        
        // slide values in visibility map
        unsigned char *mNewVisibleMap = new unsigned char[ 
            VIS_BLOWUP * HOUSE_D * VIS_BLOWUP * HOUSE_D ];

        // leave black in excess area
        memset( mNewVisibleMap, 255, 
                VIS_BLOWUP * HOUSE_D * VIS_BLOWUP * HOUSE_D );

        int visXDelta = -xExtra * VIS_BLOWUP;
        int visYDelta = yExtra * VIS_BLOWUP;
        
        for( int y=0; y<VIS_BLOWUP * HOUSE_D; y++ ) {
            int destY = y + visYDelta;
            
            if( destY >= 0 && destY < VIS_BLOWUP * HOUSE_D ) {
                
                for( int x=0; x<VIS_BLOWUP * HOUSE_D; x++ ) {
                    
                    int destX = x + visXDelta;
                    
                    if( destX >= 0 && destX < VIS_BLOWUP * HOUSE_D ) {
                        
                        mNewVisibleMap[ destY * VIS_BLOWUP * HOUSE_D + destX ]
                            = mVisibleMap[ y * VIS_BLOWUP * HOUSE_D + x ];
                        }
                    }
                }
            }
        
        // now replace old with new
        memcpy( mVisibleMap, mNewVisibleMap, 
                VIS_BLOWUP * HOUSE_D * VIS_BLOWUP * HOUSE_D );
        delete [] mNewVisibleMap;
        }
    
    HouseGridDisplay::setVisibleOffset( inXOffset, inYOffset );
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
            

            // steps
            int numSteps = lrint( distance( visPos, robPos ) * 8 );
            char hit = false;
            
            // skip last few steps so that we show borders of visible walls
            for( int j=1; j<numSteps - 1 && !hit; j++ ) {
                double weight = j / (double)numSteps;
                
                doublePair stepPos = add( mult( visPos, weight ), 
                                          mult( robPos, 1 - weight ) );
                
                int stepIndex = getTileIndex( stepPos.x, stepPos.y );
                
                if( isSubMapPropertySet( stepIndex, visionBlocking ) ) {
                    
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
