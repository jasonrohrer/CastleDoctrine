#ifndef HOUSE_GRID_DISPLAY_INCLUDED
#define HOUSE_GRID_DISPLAY_INCLUDED


#include "minorGems/game/doublePair.h"


#include "PageComponent.h"

#include "minorGems/ui/event/ActionListenerList.h"


#define HOUSE_D 16


// fires actionPerformed whenever grid contents changes
class HouseGridDisplay : public PageComponent, public ActionListenerList {
    
    public:

        HouseGridDisplay( double inX, double inY );
        
        virtual ~HouseGridDisplay();
        

        // destroyed by caller
        void setHouseMap( char *inHouseMap );
        
        
        // destroyed by caller
        char *getHouseMap();
        
            

        virtual void step();
        
        virtual void draw();

        virtual void pointerMove( float inX, float inY );
        virtual void pointerDown( float inX, float inY );
        virtual void pointerDrag( float inX, float inY );

        virtual void pointerUp( float inX, float inY );
        

    protected:
        
        char *mHouseMap;
        
        
        int mHighlightIndex;
        
        
        // get tile at relative position inX, inY
        int getTileIndex( double inX, double inY );

        // get relative position of tile inIndex
        doublePair getTilePos( int inIndex );
        
        double mTileRadius;

        int mStartIndex;
        int mGoalIndex;
    };



#endif
