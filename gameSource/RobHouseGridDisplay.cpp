#include "RobHouseGridDisplay.h"

#include "minorGems/game/gameGraphics.h"
#include "minorGems/game/game.h"
#include "minorGems/game/drawUtils.h"

#include "minorGems/util/stringUtils.h"



#include "minorGems/game/Font.h"

extern Font *mainFont;



#include "FastBoxBlurFilter.h"
#include "houseObjects.h"
#include "houseTransitions.h"
#include "tools.h"

#include <math.h>
#include <limits.h>



extern double frameRateFactor;



RobHouseGridDisplay::RobHouseGridDisplay( double inX, double inY )
        : HouseGridDisplay( inX, inY ),
          mSuccess( 0 ),
          mDead( false ),
          mDeathSourceID( -1 ),
          mDeathSourceState( 1 ),
          mLeaveDisplayCanBeShown( true ),
          mLeaveSprite( loadSprite( "left.tga" ) ),
          mCurrentTool( -1 ),
          mToolJustUsed( false ),
          mSafeMoveMode( false ),
          mSafeMoveProposed( false ),
          mSafeMoveIndex( -1 ),
          mSafeMoveConfirmed( false ),
          mSafeUpSprite( loadSprite( "moveUp.tga" ) ),
          mSafeLeftSprite( loadSprite( "moveLeft.tga" ) ),
          mSafeDownSprite( loadSprite( "moveDown.tga" ) ),
          mSafeRightSprite( loadSprite( "moveRight.tga" ) ),
          mForceAllTileToolTips( false ),
          mRobberStoleFromWife( false ) {

    for( int i=0; i<HOUSE_D * HOUSE_D; i++ ) {
        mVisibleMap[i] = 0;
        }
    }



RobHouseGridDisplay::~RobHouseGridDisplay() {
    clearMoveList();

    freeSprite( mLeaveSprite );

    freeSprite( mSafeUpSprite );
    freeSprite( mSafeLeftSprite );
    freeSprite( mSafeDownSprite );
    freeSprite( mSafeRightSprite );
    }



void RobHouseGridDisplay::clearMoveList() {
    for( int i=0; i<mMoveList.size(); i++ ) {
        delete [] *( mMoveList.getElement( i ) );
        }
    mMoveList.deleteAll();
    }



int RobHouseGridDisplay::getSuccess() {
    return mSuccess;
    }



char RobHouseGridDisplay::getDead() {
    return mDead;
    }



int RobHouseGridDisplay::getDeathSourceID() {
    return mDeathSourceID;
    }


int RobHouseGridDisplay::getDeathSourceState() {
    return mDeathSourceState;
    }



char RobHouseGridDisplay::getWifeRobbed() {
    return mRobberStoleFromWife;
    }



char RobHouseGridDisplay::getWifeKilled() {
    int numFamily = mFamilyStatus.size();
    
    for( int i=0; i<numFamily; i++ ) {
        if( *( mFamilyStatus.getElement( i ) ) == 0 ) {
            // killed
            
            // is wife?
            int objectID = *( mFamilyObjects.getElement( i ) );
            
            if( isPropertySet( objectID, 0, wife ) ) {
                return true;
                }
            }
        }
    

    return false;
    }



char RobHouseGridDisplay::getAnyFamilyKilled() {
    int numFamily = mFamilyStatus.size();
    
    for( int i=0; i<numFamily; i++ ) {
        if( *( mFamilyStatus.getElement( i ) ) == 0 ) {
            // killed
            return true;
            }
        }
    return false;
    }




char *RobHouseGridDisplay::getMoveList() {
    if( mSuccess == 0 && ! mDead ) {
        // must have committed suicide
        mMoveList.push_back( stringDuplicate( "S" ) );
        }

    if( mMoveList.size() == 0 ) {
        return stringDuplicate( "#" );
        }
    


    char **moveArray = mMoveList.getElementArray();
    
    char *moveString = join( moveArray, mMoveList.size(), "#" );
    
    delete [] moveArray;

    return moveString;
    }



void RobHouseGridDisplay::startUsingTool( int inToolID ) {
    mSafeMoveConfirmed = false;
    mSafeMoveProposed = false;
    mSafeMoveIndex = -1;
    clearSpecialHighlights();
    
    if( ! getToolInRange( inToolID ) ) {
        mForbiddenMoveHappened = true;
        return;
        }

    mCurrentTool = inToolID;

    if( mCurrentTool != -1 ) {
        int reach = getToolReach( mCurrentTool );
        
        SimpleVector<int> hitSquares;
        hitSquares.push_back( mRobberIndex );
        
        int dX[] = { -1,  0,  1,  0 };
        int dY[] = {  0,  1,  0, -1 };
        

        for( int i=0; i<reach; i++ ) {
            // expand by one
            int existingSize = hitSquares.size();
            for( int j=0; j<existingSize; j++ ) {
                
                int index = *( hitSquares.getElement( j ) );
                
                int y = index / mFullMapD;
                int x = index % mFullMapD;
                
                for( int d=0; d<4; d++ ) {
                    int yNew = y + dY[d];
                    int xNew = x + dX[d];
                    
                    if( xNew >= 0 && xNew < mFullMapD &&
                        yNew >= 0 && yNew < mFullMapD ) {
                        
                        int indexNew = yNew * mFullMapD + xNew;

                        if( hitSquares.getElementIndex( indexNew ) == -1 ) {
                            // not already hit, add it?
                            
                            // but only if it's visible
                            int subIndexNew = fullToSub( indexNew );
                            
                            if( subIndexNew != -1 ) {
                                
                                if( mTileVisibleMap[ subIndexNew ] ) {
                                    hitSquares.push_back( indexNew );
                                    }
                                }
                            }
                        }
                    }
                }
            }

        // don't draw highlight over robber
        hitSquares.deleteElementEqualTo( mRobberIndex );
        
        // don't draw highlight over welcome mat
        hitSquares.deleteElementEqualTo( mStartIndex );
        
        // filter target squares based on which targets can actually
        // be changed by the current tool
        
        for( int i=0; i<hitSquares.size(); i++ ) {
            int index = *( hitSquares.getElement( i ) );
            
            if( checkTransition( mHouseMapIDs[ index ],
                                 mHouseMapCellStates[ index ],
                                 inToolID,
                                 -1 )
                != 
                mHouseMapCellStates[ index ]
                ||
                checkTransition( mHouseMapMobileIDs[ index ],
                                 mHouseMapMobileCellStates[ index ],
                                 inToolID,
                                 -1 )
                != 
                mHouseMapMobileCellStates[ index ] ) {
                // keep target
                }
            else {
                hitSquares.deleteElement( i );
                i--;
                }
            }

        
        // finally, filter out those which have target lines that cross
        // blocking objects
        doublePair robberPos = getTilePosFull( mRobberIndex );
        for( int i=0; i<hitSquares.size(); i++ ) {
            int targetIndex = fullToSub( *( hitSquares.getElement( i ) ) );
            doublePair targetPos = getTilePos( targetIndex );

            char crossesBlocked = false;
            
            doublePair vector = sub( targetPos, robberPos );
            
            doublePair step = mult( normalize( vector ), 0.5 );
            
            doublePair currentPosition = robberPos;
            
            int maxSteps = lrint( length( vector ) / length( step ) );

            int stepCount = 0;
            
            int currentIndex = 
                getTileIndex( currentPosition.x, currentPosition.y );
            
            while( currentIndex != targetIndex && 
                   stepCount < maxSteps ) {
                
                if( isSubMapPropertySet( currentIndex, blocking ) ) {
                    crossesBlocked = true;
                    break;
                    }
                currentPosition = add( currentPosition, step );
                currentIndex = 
                    getTileIndex( currentPosition.x, currentPosition.y );
                
                stepCount++;
                }
            

            if( crossesBlocked ) {
                hitSquares.deleteElement( i );
                i--;
                }
            }
        
        

        setTargetHighlights( &hitSquares );
        }
    }



void RobHouseGridDisplay::stopUsingTool() {
    mCurrentTool = -1;

    SimpleVector<int> emptyVector;
    setTargetHighlights( &emptyVector );
    }



char RobHouseGridDisplay::getToolJustUsed() {
    char temp = mToolJustUsed;
    mToolJustUsed = false;

    return temp;
    }




void RobHouseGridDisplay::setSafeMoveMode( char inOn ) {
    mSafeMoveMode = inOn;
    mSafeMoveProposed = false;
    mSafeMoveIndex = -1;
    mSafeMoveConfirmed = false;
    clearSpecialHighlights();
    }



void RobHouseGridDisplay::setHouseMap( const char *inHouseMap ) {
    mSuccess = 0;
    mDead = false;
    mDeathSourceID = -1;
    mDeathSourceState = 1;

    stopUsingTool();
    mToolJustUsed = false;
    
    clearMoveList();

    HouseGridDisplay::setHouseMap( inHouseMap );    


    mFamilyExitPathProgress.deleteAll();
    mFamilyObjects.deleteAll();
    mFamilyStatus.deleteAll();
    mFamilySeenRobber.deleteAll();
    
    int numPaths = mFamilyExitPaths.size();
    
    for( int i=0; i<numPaths; i++ ) {
        mFamilyExitPathProgress.push_back( 0 );
        GridPos *path = *( mFamilyExitPaths.getElement( i ) );
        
        GridPos start = path[0];
        
        int startIndex = start.y * mFullMapD + start.x;
        
        mFamilyObjects.push_back( mHouseMapIDs[ startIndex ] );
        mFamilyStatus.push_back( 1 );
        mFamilySeenRobber.push_back( false );
        }
    

    mRobberStoleFromWife = false;

    // switch all unstuck
    // to "1" state for presentation to robber
    resetToggledStates( 1 );

    // robber outfit
    mRobberState = 1;
    
    for( int i=0; i<HOUSE_D * HOUSE_D * VIS_BLOWUP * VIS_BLOWUP; i++ ) {
        mVisibleMap[i] = 255;
        mTargetVisibleMap[i] = false;
        }

    // initial transitions (like for power that starts out on, etc)
    freezeMobileObjects( true );
    applyTransitionsAndProcess();
    freezeMobileObjects( false );
    }



void RobHouseGridDisplay::startWifeSearchForDeadChild( int inIndex ) {
    int numPaths = mFamilyExitPaths.size();
    for( int i=0; i<numPaths; i++ ) {
        int progress = *( mFamilyExitPathProgress.getElement( i ) );
        
        GridPos *path = *( mFamilyExitPaths.getElement( i ) );
        
        int status = *( mFamilyStatus.getElement( i ) );
        
        // still in house, alive
        if( status == 1 ) {
            
            int oldIndex = posToIndex( path[progress] );

            // wife and still alive
            if( isPropertySet( mHouseMapIDs[ oldIndex ],
                               mHouseMapCellStates[ oldIndex ], wife ) ) {

                char *blockedMap = getBlockedMap();
                
                // make dead child location unblocked
                // wife will walk to an adjacent square and stop
                blockedMap[ inIndex ] = false;
                
                GridPos startPos = 
                    { oldIndex % mFullMapD, oldIndex / mFullMapD };
                
                GridPos goalPos = 
                    { inIndex % mFullMapD, inIndex / mFullMapD };

                int numStepsToGoal;
                GridPos *fullPath;
                
                char found = pathFind( mFullMapD, mFullMapD,
                                       blockedMap, 
                                       startPos, goalPos, 
                                       &numStepsToGoal,
                                       &fullPath );
            
                if( found && numStepsToGoal != 0 ) {
                    
                    // splice with beginning of old path

                    int newPathLength = progress + numStepsToGoal;
                    
                    GridPos *newPath = 
                        new GridPos[ newPathLength ];
                    
                    memcpy( newPath, path, 
                            ( progress + 1 ) * sizeof( GridPos ) );
                    
                    memcpy( &( newPath[ progress + 1 ] ),
                            &( fullPath[ 1 ] ),
                            ( numStepsToGoal - 1 ) * sizeof( GridPos ) );
                    


                    delete [] *( mFamilyExitPaths.getElement( i ) );
                    delete [] fullPath;
                    
                    *( mFamilyExitPaths.getElement( i ) ) = newPath;
                    
                    *( mFamilyExitPathLengths.getElement( i ) ) = 
                        newPathLength;

                    // new path, same progress along it (wife at
                    // splice point)
                    *( mFamilyExitPathProgress.getElement( i ) ) = progress;

                    // force wife to "see robber" so that she starts moving
                    *( mFamilySeenRobber.getElement( i ) ) = true;
                    }
                else {
                    // no path found
                    // keep going to exit (original path)
                    }
                
                delete [] blockedMap;
                
                break;
                }
            }
        }       
    }



void RobHouseGridDisplay::applyTransitionsAndProcess() {

    // check for family deaths, regardless of whether mobiles frozen
    int numPaths = mFamilyExitPaths.size();
    for( int i=0; i<numPaths; i++ ) {
        int progress = *( mFamilyExitPathProgress.getElement( i ) );
        
        int pathLength = *( mFamilyExitPathLengths.getElement( i ) );
        
        GridPos *path = *( mFamilyExitPaths.getElement( i ) );
        
        if( pathLength == 1 || progress < pathLength - 1 ) {
            
            int oldIndex = posToIndex( path[progress] );

            if( mHouseMapCellStates[ oldIndex ] != 1 ) {
                // killed!
                // don't move
                    
                if( *( mFamilyStatus.getElement( i ) ) != 0 ) {
                    // just died
                    
                    if( isPropertySet( mHouseMapIDs[oldIndex],
                                       mHouseMapCellStates[oldIndex],
                                       son ) ||
                        isPropertySet( mHouseMapIDs[oldIndex],
                                       mHouseMapCellStates[oldIndex],
                                       daughter ) ) {

                        startWifeSearchForDeadChild( oldIndex );
                        }

                    // dead status
                    *( mFamilyStatus.getElement( i ) ) = 0;
                    }
                }
            }        
        }
    



    if( ! areMobilesFrozen() ) {
        
        // first, move each family member
        int numPaths = mFamilyExitPaths.size();
        for( int i=0; i<numPaths; i++ ) {
            int progress = *( mFamilyExitPathProgress.getElement( i ) );
        
            int pathLength = *( mFamilyExitPathLengths.getElement( i ) );
        
            GridPos *path = *( mFamilyExitPaths.getElement( i ) );

            int objectID = *( mFamilyObjects.getElement( i ) );

            char seenRobber = *( mFamilySeenRobber.getElement( i ) );
            
            if( !seenRobber ) {
                
                // check if seen this step
                int status = *( mFamilyStatus.getElement( i ) );

                if( status == 1 ) {
                    int subIndex = fullToSub( posToIndex( path[progress] ) );
                    
                    if( subIndex != -1 && mTileVisibleMap[ subIndex ] ) {
                        seenRobber = true;
                        *( mFamilySeenRobber.getElement( i ) ) = seenRobber;
                        }
                    }
                if( !seenRobber ) {
                    continue;
                    }
                }
            
            if( pathLength == 1 || progress < pathLength - 1 ) {
            
                int oldIndex = posToIndex( path[progress] );

                if( mHouseMapCellStates[ oldIndex ] != 1 ) {
                    // killed!
                    // don't move
                    }
                else if( progress < pathLength - 1 ) {
                    

                    progress ++;

                    int newIndex = posToIndex( path[progress] );
                    
                    // make sure it's clear
                    if( newIndex != mRobberIndex &&
                        mHouseMapIDs[newIndex] == 0 ) {
                        // move along
                
                        mHouseMapIDs[newIndex] = 
                            mHouseMapIDs[oldIndex];
                    
                        mHouseMapCellStates[newIndex] = 
                            mHouseMapCellStates[oldIndex];
                
                        // leave empty where we used to be
                        mHouseMapIDs[oldIndex] = 0;
                        mHouseMapCellStates[oldIndex] = 1;

                        *( mFamilyExitPathProgress.getElement( i ) ) = 
                            progress;
                        }    
                    }
                }
            else {    
                // else already done...
            
                // if standing on entrance, remove from map
                // (escape during this step)
                int oldIndex = posToIndex( path[progress] );

                if( oldIndex == mStartIndex &&
                    mHouseMapIDs[oldIndex] == objectID ) {
                    
                    mHouseMapIDs[oldIndex] = 0;
                    mHouseMapCellStates[oldIndex] = 1;
                    
                    // escaped status
                    *( mFamilyStatus.getElement( i ) ) = 2;
                    }
                }
            
            
            }
        }
    
 


    applyTransitions( mHouseMapIDs, mHouseMapCellStates, 
                      mHouseMapMobileIDs, mHouseMapMobileCellStates,
                      mFullMapD, mFullMapD,
                      mRobberIndex,
                      mLastRobberIndex,
                      mStartIndex );

    copyAllIntoSubCells();

    if( isPropertySet( mHouseMapIDs[ mRobberIndex ], 
                       mHouseMapCellStates[ mRobberIndex ], deadly ) ) {
        
        // robber killed by static object
        mDead = true;
        mDeathSourceID = mHouseMapIDs[ mRobberIndex ];
        mDeathSourceState = mHouseMapCellStates[ mRobberIndex ];
        
        mRobberState = checkTransition( PLAYER_ID, mRobberState,
                                        mDeathSourceID,
                                        mDeathSourceState );
        mSuccess = 0;
        processFamilyAtEnd();
        }
    else if( isPropertySet( mHouseMapMobileIDs[ mRobberIndex ], 
                            mHouseMapMobileCellStates[ mRobberIndex ], 
                            deadly ) ) {

        // robber killed by mobile
        mDead = true;
        mDeathSourceID = mHouseMapMobileIDs[ mRobberIndex ];
        mDeathSourceState = mHouseMapMobileCellStates[ mRobberIndex ];

        mRobberState = checkTransition( PLAYER_ID, mRobberState,
                                        mDeathSourceID,
                                        mDeathSourceState );
        mSuccess = 0;
        processFamilyAtEnd();
        }

    
    
    recomputeVisibility();



    // now trigger transitions based on the new visibility
    
    char *fullMapTilesVisible = new char[ mFullMapD * mFullMapD ];

    memset( fullMapTilesVisible, false, mFullMapD * mFullMapD );
    
    for( int y=0; y<HOUSE_D; y++ ) {
        int bigY = y + mSubMapOffsetY;

        for( int x=0; x<HOUSE_D; x++ ) {
            int bigX = x + mSubMapOffsetX;

            int subIndex = y * HOUSE_D + x;
            
            int bigIndex = bigY * mFullMapD + bigX;
            
            fullMapTilesVisible[ bigIndex ] = mTileVisibleMap[ subIndex ];
            }
        }

    
    applyVisibilityTransitions( mHouseMapIDs, mHouseMapCellStates, 
                                mHouseMapMobileIDs, mHouseMapMobileCellStates,
                                mFullMapD, mFullMapD,
                                fullMapTilesVisible );

    delete [] fullMapTilesVisible;
    }













    

void RobHouseGridDisplay::draw() {
    HouseGridDisplay::draw();

    
    // apply same scissor region to visibility overlay
    enableScissor( -HOUSE_D * mTileRadius,
                   -HOUSE_D * mTileRadius,
                   2 * ( HOUSE_D * mTileRadius ),
                   2 * ( HOUSE_D * mTileRadius ) );
    

    


    
    // decay each frame
    for( int i=0; i<HOUSE_D * HOUSE_D * VIS_BLOWUP * VIS_BLOWUP; i++ ) {
        if( mRobberIndex == mGoalIndex ) {
            // robber hit vault
            // instantly black out, because robber on vault looks weird
            mVisibleMap[i] = 255;
            }
        else if( mTargetVisibleMap[i] ) {
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


    disableScissor();


    if( mLeaveDisplayCanBeShown && mRobberIndex == mStartIndex ) {
        doublePair startPos = getTilePosFull( mStartIndex );
        
        startPos.x -= 1;

        setDrawColor( 1, 1, 1, 1 );
        
        drawSprite( mLeaveSprite, startPos, 
                    1.0 / 16.0 );
        
        startPos.x -= 0.5;

        mainFont->drawString( translate( "leave" ), startPos, alignRight );;

        }
    
    }



char RobHouseGridDisplay::getAboutToLeave() {
    return ( mRobberIndex == mStartIndex );
    }




void RobHouseGridDisplay::applyCurrentTool( int inTargetFullIndex ) {
    
    if( mToolTargetFullIndices.getElementIndex( inTargetFullIndex ) != -1 ) {
        // this is an allowed target spot for this tool
        
        applyToolTransition( mHouseMapIDs, mHouseMapCellStates, 
                             mHouseMapMobileIDs, mHouseMapMobileCellStates,
                             mFullMapD, mFullMapD,
                             mCurrentTool, inTargetFullIndex );
        }
    else {
        mForbiddenMoveHappened = true;
        }
    
    // tool use triggers a step, but not for mobiles
    freezeMobileObjects( true );
    applyTransitionsAndProcess();
    freezeMobileObjects( false );
    
    stopUsingTool();
    mToolJustUsed = true;
    }




void RobHouseGridDisplay::pointerOver( float inX, float inY ) {
    
    int overIndex = getTileIndex( inX, inY );
    

    // don't show tool tips for invisible tiles (shrouded)
    // or during blackout after robber hits goal
    if( overIndex != -1 && 
        ( mTileVisibleMap[ overIndex ] || mForceAllTileToolTips ) 
        &&
        mRobberIndex != mGoalIndex ) {

        // base behavior (display tool tip)
        HouseGridDisplay::pointerOver( inX, inY );
        
        // but override so that highlight is never shown
        mHighlightIndex = -1;
        }
    else if( mPointerInside ) {
        // pointer just left visible region
        setToolTip( NULL );
        
        mPointerInside = false;
        }
    
        
        
    }



// override so that they do nothing but display tool tips
void RobHouseGridDisplay::pointerMove( float inX, float inY ) {
    pointerOver( inX, inY );
    }


void RobHouseGridDisplay::pointerDown( float inX, float inY ) {
    pointerDrag( inX, inY );
    }



void RobHouseGridDisplay::pointerDrag( float inX, float inY ) {
    pointerOver( inX, inY );
    
    setPickedTargetHighlight( -1 );
    
    if( mCurrentTool != -1 ) {
        // see if this is in our target region
        int index = subToFull( getTileIndex( inX, inY ) );

        if( mToolTargetFullIndices.getElementIndex( index ) != -1 ) {
            // hit
            setPickedTargetHighlight( index );
            }
        }
    }


void RobHouseGridDisplay::pointerUp( float inX, float inY ) {
    pointerOver( inX, inY );

    if( mCurrentTool != -1 ) {
        // see if this is in our target region
        int index = subToFull( getTileIndex( inX, inY ) );

        if( mToolTargetFullIndices.getElementIndex( index ) != -1 ) {
            // hit

            mMoveList.push_back( 
                autoSprintf( "t%d@%d", mCurrentTool, index ) );
            
            
            applyCurrentTool( index );
            
            fireActionPerformed( this );
            }
        }
    

    }



void RobHouseGridDisplay::keyDown( unsigned char inASCII ) {    
    if( mSafeMoveMode && mSafeMoveProposed ) {
        
        if( inASCII == 13 ) {
            mSafeMoveConfirmed = true;
            // apply move again, now that we have confirmation
            moveRobber( mSafeMoveIndex );
            }
        }
    
    HouseGridDisplay::keyDown( inASCII );
    }




void RobHouseGridDisplay::moveRobber( int inNewIndex ) {
    if( mDead ) {
        // can't move anymore
        return;
        }

    if( mCurrentTool != -1 ) {
        // turn tool off when robber moves
        stopUsingTool();
        mToolJustUsed = false;
        }


    if( mSafeMoveMode ) {
        if( mSafeMoveProposed && mSafeMoveConfirmed &&
            inNewIndex == mSafeMoveIndex ) {
            // this call is a confirmation

            // go ahead and move, below
            mSafeMoveConfirmed = false;
            mSafeMoveProposed = false;
            mSafeMoveIndex = -1;

            clearSpecialHighlights();
            }
        else {
            // this is a new proposal

            mSafeMoveConfirmed = false;
            mSafeMoveProposed = true;
            mSafeMoveIndex = inNewIndex;
            clearSpecialHighlights();
            
            // add highlight for this proposal
            doublePair robberPos = getTilePosFull( mRobberIndex );
            doublePair movePos = getTilePosFull( mSafeMoveIndex );
        
            double dX = movePos.x - robberPos.x;
            double dY = movePos.y - robberPos.y;
            
            SpriteHandle moveSprite;
            
            if( dX < 0 ) {
                moveSprite = mSafeLeftSprite;
                }
            else if( dX > 0 ) {
                moveSprite = mSafeRightSprite;
                }
            else if( dY < 0 ) {
                moveSprite = mSafeDownSprite;
                }
            else if( dY > 0 ) {
                moveSprite = mSafeUpSprite;
                }

            
            addSpecialHighlight( mSafeMoveIndex, moveSprite );
            setToolTip( translate( "safeMoveTip" ) );
            
            // wait for confirmation
            return;
            }
        }
    
    

    HouseGridDisplay::moveRobber( inNewIndex );

    if( mSuccess == 2 ) {
        // left, don't apply transitions
        return;
        }

    // did robber step onto killed wife?
    if( getWifeKilled() ) {
        if( isPropertySet( mHouseMapIDs[ mRobberIndex ], 0, wife ) ) {
            mRobberStoleFromWife = true;
            setWifeMoney( 0 );
            }
        }

    applyTransitionsAndProcess();
    
    // a move!
    mMoveList.push_back( autoSprintf( "m%d", mRobberIndex ) );
    
    if( mRobberIndex == mGoalIndex ) {
        mSuccess = 1;
        processFamilyAtEnd();

        fireActionPerformed( this );
        }
    }



void RobHouseGridDisplay::robberTriedToLeave() {
    mSuccess = 2;
    processFamilyAtEnd();

    mMoveList.push_back( stringDuplicate( "L" ) );

    fireActionPerformed( this );
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


    if( xExtra != 0 || yExtra != 0 ) {                
        // view on screen has moved
        // must recompute visibility here so that family 
        // start-moving-when-visible triggers work correctly
        recomputeVisibility();
        }    

    }




void RobHouseGridDisplay::recomputeVisibility() {

    // clear
    memset( mTileVisibleMap, 0, HOUSE_D * HOUSE_D );


    int robSubIndex = fullToSub( mRobberIndex );
    
    if( robSubIndex == -1 ) {
        // robber out of bounds
        // nothing visible
        for( int i=0; i<HOUSE_D * HOUSE_D * VIS_BLOWUP * VIS_BLOWUP; i++ ) {
            mTargetVisibleMap[i] = false;
            }
        return;
        }
    

    char *blockingMap = new char[ HOUSE_D * HOUSE_D ];    

    memset( blockingMap, 0, HOUSE_D * HOUSE_D );

    for( int i=0; i<HOUSE_D * HOUSE_D; i++ ) {
        if( isSubMapPropertySet( i, visionBlocking ) ) {
            
            blockingMap[ i ] = 1;
            }
        }
    
    int robberX = robSubIndex % HOUSE_D;
    int robberY = robSubIndex / HOUSE_D;
    
    // in middle of blown up tile
    int robberBlowupX = robberX * VIS_BLOWUP + VIS_BLOWUP / 2;
    int robberBlowupY = robberY * VIS_BLOWUP + VIS_BLOWUP / 2;
    

    // make sure there is room for our high res int coordinates, for
    // the farthest subspace tile, in 32-bitint space
    // also room for taking the square of coordinates (for measuring
    // distance)
    // 46340 is smaller than the sqrt of INT_MAX
    int highResFactor = 46340 / (VIS_BLOWUP * HOUSE_D );
    


    // seven steps per tile when checking for line-of-sight obstruction
    int numStepsFactor = ( highResFactor * VIS_BLOWUP ) / 7;
    

    for( int y=0; y<HOUSE_D * VIS_BLOWUP; y++ ) {
        for( int x=0; x<HOUSE_D * VIS_BLOWUP; x++ ) {

            int highY = y * highResFactor + highResFactor / 2;
            int highX = x * highResFactor + highResFactor / 2;
             
            int robberHighY = 
                robberBlowupY * highResFactor + highResFactor / 2;
            int robberHighX = 
                robberBlowupX * highResFactor + highResFactor / 2;

            int distHigh = 
                (int)( sqrt( ( highY - robberHighY ) *
                             ( highY - robberHighY ) ) )
                +
                (int)( sqrt( ( highX - robberHighX ) *
                             ( highX - robberHighX ) ) );
                
            int numSteps = distHigh / numStepsFactor;

            
            // next:  take this number of steps along high-res
            // line between robber and the point that we're checking
            }
        }
    

    delete [] blockingMap;
        


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
            
            int selfBlockingSteps = 0;

            // steps
            int numSteps = lrint( distance( visPos, robPos ) * 8 );
            char hit = false;
            
            // skip last few steps so that we show borders of visible walls


            doublePair stepVector = mult( sub( visPos, robPos ), 
                                          1.0 / numSteps );
            double stepVectorX = stepVector.x;
            double stepVectorY = stepVector.y;
                            

            // start at step 1, one stepVector length in
            doublePair stepPos = add( robPos, stepVector );

            for( int j=1; j<numSteps - 1 && !hit; j++ ) {
                int stepIndex = getTileIndex( stepPos.x, stepPos.y );
                
                // avoid self-blocking on first steps into self

                if( selfBlockingSteps < 2 && stepIndex == visTileIndex ) {
                    // blocked by self for few steps
                    // don't count this as vision blocking
                    selfBlockingSteps ++;
                    }
                else if( isSubMapPropertySet( stepIndex, visionBlocking ) ) {
                    hit = true;
                    }

                // profiler found:
                // don't call add() in inner loop here
                stepPos.x += stepVectorX;
                stepPos.y += stepVectorY;
                }
            
            if( hit ) {
                mTargetVisibleMap[i] = false;
                }
            else {
                mTargetVisibleMap[i] = true;
                
                // at least one sub-area of tile is visible
                int visTileIndex = getTileIndex( visPos.x, visPos.y );
                
                mTileVisibleMap[ visTileIndex ] = true;
                }

            i++;
            }
        }
    
    for( int y=HOUSE_D; y>=0; y-- ) {
        for( int x=0; x<HOUSE_D; x++ ) {
            if( mTileVisibleMap[ y * HOUSE_D + x ] ) {
                printf( "1" );
                }
            else {
                printf( "0" );
                }
            }
        printf( "\n" );
        }
    printf( "\n" );

    }




void RobHouseGridDisplay::processFamilyAtEnd() {
    int numFamily = mFamilyObjects.size();
    
    for( int i=0; i<numFamily; i++ ) {
        int status = *( mFamilyStatus.getElement( i ) );
        
        int objectID = *( mFamilyObjects.getElement( i ) );
        
        int pathLength = *( mFamilyExitPathLengths.getElement( i ) );

        int progress = *( mFamilyExitPathProgress.getElement( i ) );

        GridPos *path = *( mFamilyExitPaths.getElement( i ) );


        if( status != 0 && pathLength > 1 ) {
            // still alive

            // move back to their starting positions

            if( status == 1 ) {
                // alive and in house
                // remove them from where they are in house
                
                int oldIndex = posToIndex( path[progress] );
                mHouseMapIDs[ oldIndex ] = 0;
                mHouseMapCellStates[ oldIndex ] = 1;
                }
            // otherwise, left house
            
            
            // start at their starting positions
            int pathSpot = 0;

            // look for spot that's unblocked for them to stand, as
            // close to starting position along path as possible
            
            char found = false;
            
            // don't consider door mat as possible spot for them
            while( pathSpot < pathLength - 1 ) {
                int newIndex = posToIndex( path[pathSpot] );
                
                if( mHouseMapIDs[ newIndex ] == 0 ) {
                    
                    found = true;
                    
                    mHouseMapIDs[ newIndex ] = objectID;
                    mHouseMapCellStates[ newIndex ] = 1;
                    break;
                    }
                pathSpot++;
                }
            
            if( !found ) {
                
                // search columns backward away from door
                
                for( int x=1; x<mFullMapD; x++ ) {
                    
                    // above door
                    for( int y=mFullMapD/2; y<mFullMapD; y++ ) {
                        int index = y * mFullMapD + x;
                        
                        if( mHouseMapIDs[ index ] == 0 ) {
                            
                            found = true;
                    
                            mHouseMapIDs[ index ] = objectID;
                            mHouseMapCellStates[ index ] = 1;
                            break;
                            }
                        }
                    if( found ) {
                        break;
                        }

                    // below door
                    for( int y=mFullMapD/2-1; y>=0; y-- ) {
                        int index = y * mFullMapD + x;
                        
                        if( mHouseMapIDs[ index ] == 0 ) {
                            
                            found = true;
                    
                            mHouseMapIDs[ index ] = objectID;
                            mHouseMapCellStates[ index ] = 1;
                            break;
                            }
                        }

                    if( found ) {
                        break;
                        }
                    }
                }
            
            // still possible that not found here.
            // in that case (house is so full that there's no room
            // for this family member), well... family member leaves for good!
            }

        }
    
    }

