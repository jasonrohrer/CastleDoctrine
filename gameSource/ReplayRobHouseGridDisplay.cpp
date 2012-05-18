#include "ReplayRobHouseGridDisplay.h"


#include "minorGems/game/Font.h"
#include "minorGems/game/game.h"

#include "minorGems/util/stringUtils.h"


extern Font *mainFont;

extern double frameRateFactor;


#define STEP_DELAY (int)( 30 * frameRateFactor )

#define BUTTON_Y ( - mTileRadius * HOUSE_D  - 0.75 )



ReplayRobHouseGridDisplay::ReplayRobHouseGridDisplay( double inX, double inY,
                                                      GamePage *inParentPage )
        : RobHouseGridDisplay( inX, inY, inParentPage ),
          mStepButton( mainFont, 0, BUTTON_Y, 
                       translate( "step" ) ),
          mPlayButton( mainFont, 2, BUTTON_Y, 
                       translate( "play" ) ),
          mStopButton( mainFont, 4, BUTTON_Y, 
                       translate( "stop" ) ),
          mVisibilityButton( mainFont, -4, BUTTON_Y, 
                             translate( "toggleVisibility" ) ),
          mPlaying( false ),
          mStepsUntilNextPlayStep( 0 ),
          mVisibilityToggle( false ) {

    mStopButton.setVisible( false );
    
    addComponent( &mStepButton );
    addComponent( &mPlayButton );
    addComponent( &mStopButton );
    addComponent( &mVisibilityButton );
    
    mStepButton.addActionListener( this );
    mPlayButton.addActionListener( this );
    mStopButton.addActionListener( this );
    mVisibilityButton.addActionListener( this );
    }


        
ReplayRobHouseGridDisplay::~ReplayRobHouseGridDisplay() {

    }



void ReplayRobHouseGridDisplay::setMoveList( char *inMoveList ) {
    clearMoveList();
    
    int numMoves;
    char **moves = split( inMoveList, "_", &numMoves );

    mMoveList.appendArray( moves, numMoves );
    
    delete [] moves;

    // prepare for new playback
    mPlaying =  false;
        
    mStepButton.setVisible( true );
    mPlayButton.setVisible( true );
    mStopButton.setVisible( false );
    }



void ReplayRobHouseGridDisplay::step() {
    RobHouseGridDisplay::step();
    
    if( mPlaying ) {
        mStepsUntilNextPlayStep --;
        
        if( mStepsUntilNextPlayStep == 0 ) {
            takeStep();
            
            mStepsUntilNextPlayStep = STEP_DELAY;
            }
        }
    
    }



void ReplayRobHouseGridDisplay::takeStep() {
    if( mMoveList.size() > 0 ) {
        
        char *move = *( mMoveList.getElement( 0 ) );

        mMoveList.deleteElement( 0 );
        

        if( strlen( move ) > 0 ) {
            if( move[0] == 'm' ) {
                // player movement to new index
                int newIndex;
                sscanf( move, "m%d", &newIndex );

                // DON'T call RobHouseGridDisplay's moveRobber, 
                // because it detects success condition and fires an event,
                // which we don't want to do during replay
                HouseGridDisplay::moveRobber( newIndex );
                
                applyTransitionsAndProcess();
                }
            }
        delete [] move;
        }

    if( mMoveList.size() == 0 ) {
        // auto stop, no more steps allowed
        mPlaying =  false;
        
        mStepButton.setVisible( false );
        mPlayButton.setVisible( false );
        mStopButton.setVisible( false );
        }
    }




void ReplayRobHouseGridDisplay::specialKeyDown( int inKeyCode ) {
    // do nothing (ignore keyboard input)
    }

        
        
void ReplayRobHouseGridDisplay::actionPerformed( GUIComponent *inTarget ) {
    if( inTarget == &mPlayButton ) {
        mPlaying = true;
        mStepsUntilNextPlayStep = STEP_DELAY;     

        mStopButton.setVisible( true );
        mPlayButton.setVisible( false );
        mStepButton.setVisible( false );
       }
    else if( inTarget == &mStopButton ) {
        mPlaying = false;
        mStopButton.setVisible( false );
        mPlayButton.setVisible( true );
        mStepButton.setVisible( true );
        }
    else if( inTarget == &mStepButton ) {
        takeStep();
        }
    else if( inTarget == &mVisibilityButton ) {
        mVisibilityToggle = ! mVisibilityToggle;
        recomputeVisibility();
        }
    }



void ReplayRobHouseGridDisplay::recomputeVisibility() {
    
    if( mVisibilityToggle ) {    
        // all visible during playback

        int i = 0;
        for( int y=0; y<HOUSE_D * VIS_BLOWUP; y++ ) {
            for( int x=0; x<HOUSE_D * VIS_BLOWUP; x++ ) {

                mTargetVisibleMap[i] = true;

                i++;
                }
            }
        }
    else {
        RobHouseGridDisplay::recomputeVisibility();
        }
    
    }

