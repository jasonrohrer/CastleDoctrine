#ifndef REPLAY_ROB_HOUSE_GRID_DISPLAY_INCLUDED
#define REPLAY_ROB_HOUSE_GRID_DISPLAY_INCLUDED


#include "RobHouseGridDisplay.h"

#include "TextButton.h"



// grid that supports replaying a move list
class ReplayRobHouseGridDisplay : public RobHouseGridDisplay, 
                                  public ActionListener {
    
    public:

        ReplayRobHouseGridDisplay( double inX, double inY,
                                   GamePage *inParentPage = NULL );
        
        virtual ~ReplayRobHouseGridDisplay();

        
        // must be called AFTER setting house map

        // list as a whitespace-free string
        // destroyed by caller
        void setMoveList( char *inMoveList );
        

        virtual void step();


        // override to ignore arrow key movement
        virtual void specialKeyDown( int inKeyCode );
        
        
        virtual void actionPerformed( GUIComponent *inTarget );
        

    protected:
        
        
        // override to make EVERYTHING visible if visibility toggled
        virtual void recomputeVisibility();

        
        TextButton mStepButton;
        TextButton mPlayButton;
        TextButton mStopButton;
        
        TextButton mVisibilityButton;

        char mPlaying;
        int mStepsUntilNextPlayStep;

        char mVisibilityToggle;

        void takeStep();
        
    };



#endif
