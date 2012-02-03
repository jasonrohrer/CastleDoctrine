#ifndef ROB_HOUSE_GRID_DISPLAY_INCLUDED
#define ROB_HOUSE_GRID_DISPLAY_INCLUDED


#include "HouseGridDisplay.h"

#include "minorGems/util/SimpleVector.h"



// fires actionPerformed whenever robbery ends
class RobHouseGridDisplay : public HouseGridDisplay {
    
    public:

        RobHouseGridDisplay( double inX, double inY );
        
        virtual ~RobHouseGridDisplay();


        char getSuccess();

        // list as a whitespace-free string
        // destroyed by caller
        char *getMoveList();
        

        // override to update visibility when map changes
        virtual void setHouseMap( char *inHouseMap );
            

        virtual void draw();

        // override so that they do nothing
        virtual void pointerMove( float inX, float inY );
        virtual void pointerDown( float inX, float inY );
        virtual void pointerDrag( float inX, float inY );
        virtual void pointerUp( float inX, float inY );
        
        // arrow key movement
        virtual void specialKeyDown( int inKeyCode );
        virtual void specialKeyUp( int inKeyCode );


    protected:
        
        int mRobberIndex;
        
        char mSuccess;
        

        char mVisibleMap[ HOUSE_D * HOUSE_D ];

        

        SimpleVector<char *> mMoveList;

        void clearMoveList();



        void recomputeVisibility();
        
    };



#endif
