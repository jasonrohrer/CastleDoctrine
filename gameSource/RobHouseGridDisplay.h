#ifndef ROB_HOUSE_GRID_DISPLAY_INCLUDED
#define ROB_HOUSE_GRID_DISPLAY_INCLUDED


#include "HouseGridDisplay.h"


// fires actionPerformed whenever grid contents changes
class RobHouseGridDisplay : public HouseGridDisplay {
    
    public:

        RobHouseGridDisplay( double inX, double inY );
        
        virtual ~RobHouseGridDisplay();
        

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

        char mVisibleMap[ HOUSE_D * HOUSE_D ];


        void recomputeVisibility();
        
    };



#endif
