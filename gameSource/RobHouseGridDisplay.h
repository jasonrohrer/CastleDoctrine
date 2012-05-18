#ifndef ROB_HOUSE_GRID_DISPLAY_INCLUDED
#define ROB_HOUSE_GRID_DISPLAY_INCLUDED


#include "HouseGridDisplay.h"

#include "minorGems/util/SimpleVector.h"


#define VIS_BLOWUP 2


// fires actionPerformed whenever robbery ends
class RobHouseGridDisplay : public HouseGridDisplay {
    
    public:

        RobHouseGridDisplay( double inX, double inY,
                             GamePage *inParentPage = NULL );
        
        virtual ~RobHouseGridDisplay();


        char getSuccess();
        char getDead();
        int getDeathSourceID();
        int getDeathSourceState();

        // list as a whitespace-free string
        // destroyed by caller
        char *getMoveList();
        

        // override to update visibility when map changes
        virtual void setHouseMap( char *inHouseMap );

        // override to shift visibility map too
        virtual void setVisibleOffset( int inXOffset, int inYOffset );


        virtual void draw();

        // override so that they do nothing
        virtual void pointerMove( float inX, float inY );
        virtual void pointerDown( float inX, float inY );
        virtual void pointerDrag( float inX, float inY );
        virtual void pointerUp( float inX, float inY );


    protected:
        
        char mSuccess;
        char mDead;
        char mDeathSourceID;
        char mDeathSourceState;

        // 0 = visible (shroud transparent)
        // 255 = invisible (shroud opaque)
        unsigned char mVisibleMap[ 
            VIS_BLOWUP * HOUSE_D * VIS_BLOWUP * HOUSE_D ];

        // true/false for whether each spot wants to move towards visibility
        // (for smooth transitions)
        char mTargetVisibleMap[ VIS_BLOWUP * HOUSE_D * VIS_BLOWUP * HOUSE_D ];


        SimpleVector<char *> mMoveList;

        void clearMoveList();

        // override to recompute visibility and check for goal reached
        virtual void moveRobber( int inNewIndex );
        
        virtual void recomputeVisibility();
        

        void applyTransitionsAndProcess();

        
        // override from HouseGridDisplay
        void pointerOver( float inX, float inY );

    };



#endif
