#ifndef HOUSE_GRID_DISPLAY_INCLUDED
#define HOUSE_GRID_DISPLAY_INCLUDED


#include "minorGems/game/doublePair.h"


#include "PageComponent.h"

#include "houseObjects.h"

#include "minorGems/ui/event/ActionListenerList.h"


#include "minorGems/game/gameGraphics.h"


#define HOUSE_D 13


// fires actionPerformed whenever grid contents changes
class HouseGridDisplay : public PageComponent, public ActionListenerList {
    
    public:

        HouseGridDisplay( double inX, double inY );
        
        virtual ~HouseGridDisplay();
        

        // destroyed by caller
        virtual void setHouseMap( char *inHouseMap );
        
        
        // destroyed by caller
        char *getHouseMap();

        char isGoalSet();
        
        
        virtual void setVisibleOffset( int inXOffset, int inYOffset );
        

        virtual void step();
        
        virtual void draw();

        virtual void pointerMove( float inX, float inY );
        virtual void pointerDown( float inX, float inY );
        virtual void pointerDrag( float inX, float inY );

        virtual void pointerUp( float inX, float inY );

        
        // arrow key movement
        virtual void specialKeyDown( int inKeyCode );
        virtual void specialKeyUp( int inKeyCode );
        

    protected:
        int mRobberIndex;

        char *mHouseMap;
        
        int mNumMapSpots;
        int *mHouseMapIDs;

        int *mHouseMapCellStates;
        
        int mFullMapD;

        int mSubMapOffsetX;
        int mSubMapOffsetY;

        // visible HOUSE_D x HOUSE_D sub-square of map
        int *mHouseSubMapIDs;
        
        int *mHouseSubMapCellStates;

        int subToFull( int inSubCellIndex );
        // can return -1 if out of sub-square bounds
        int fullToSub( int inFullCellIndex );

        // copy contents of sub cell back into full map
        void copySubCellBack( int inSubCellIndex );

        
        // can be overridded to do special processing after robber moves
        virtual void moveRobber( int inNewIndex );
        
        
        char isSubMapPropertySet( int inSubCellIndex, propertyID inProperty );

        
        int mHighlightIndex;
        
        
        // get tile at relative position inX, inY
        int getTileIndex( double inX, double inY );

        // get relative position of tile inIndex
        doublePair getTilePos( int inIndex );
        
        double mTileRadius;

        int mStartIndex;
        int mGoalIndex;

        char mGoalSet;


        // inIndex is tile's sub-map index
        // inNeighbor pick LBRT neighbor, in that order (L=0, T=3)
        int getTileNeighbor( int inIndex, int inNeighbor );
        

        SpriteHandle mWallShadowSprite;
        
        
        void drawTiles( char inFloorOnly );
        

        void recomputeWallShadows();
        
    };



#endif
