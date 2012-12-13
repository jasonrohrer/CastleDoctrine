#ifndef REPLAY_ROB_HOUSE_GRID_DISPLAY_INCLUDED
#define REPLAY_ROB_HOUSE_GRID_DISPLAY_INCLUDED


#include "RobHouseGridDisplay.h"

#include "TextButton.h"



// grid that supports replaying a move list
class ReplayRobHouseGridDisplay : public RobHouseGridDisplay, 
                                  public ActionListener {
    
    public:

        ReplayRobHouseGridDisplay( double inX, double inY );
        
        virtual ~ReplayRobHouseGridDisplay();

        // override
        virtual void setWifeMoney( int inMoney );

        // must be called AFTER setting house map

        // list as a whitespace-free string
        // destroyed by caller
        void setMoveList( char *inMoveList );


        // can check whether a tool has been picked since the last
        // call to getToolIDJustPicked
        // returns ID of pickedtool, or -1
        int getToolIDJustPicked();
        

        virtual void step();


        // override to ignore arrow key movement
        virtual void specialKeyDown( int inKeyCode );
        
        
        virtual void actionPerformed( GUIComponent *inTarget );
        

        // override from RobHouseGridDisplay so that tool targeting not
        // triggered by mouse
        virtual void pointerDrag( float inX, float inY );

        virtual void pointerUp( float inX, float inY );


    protected:
        
        
        // override to make EVERYTHING visible if visibility toggled
        virtual void recomputeVisibility();

        
        TextButton mStepButton;
        TextButton mPlayButton;
        TextButton mStopButton;
        TextButton mRestartButton;
        
        TextButton mVisibilityButton;

        char mPlaying;
        int mStepsUntilNextPlayStep;

        char mVisibilityToggle;
        
        int mToolIDJustPicked;

        void takeStep();
        
        // for restart
        char mOriginalWifeMoney;
        char *mOriginalMoveList;
    };



#endif
