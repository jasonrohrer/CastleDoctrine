#ifndef HOUSE_GRID_DISPLAY_INCLUDED
#define HOUSE_GRID_DISPLAY_INCLUDED


#include "minorGems/game/doublePair.h"


#include "PageComponent.h"

#include "houseObjects.h"

#include "HouseObjectPicker.h"


#include "minorGems/ui/event/ActionListenerList.h"

#include "minorGems/util/SimpleVector.h"


#include "minorGems/game/gameGraphics.h"


#define HOUSE_D 13


#define NUM_NOISE_TILES 100


// for undo history
typedef struct GridChangeRecord {
        // index of change
        int fullIndex;
        
        int oldID;
        int oldState;

        int oldMobileID;
        int oldMobileState;
        
        int newID;

        int robberIndex;
        int subMapOffsetX, subMapOffsetY;
        
    } ObjectPlacementRecord;



// fires actionPerformed whenever grid contents changes
class HouseGridDisplay : public PageComponent, public ActionListenerList {
    
    public:

        HouseGridDisplay( double inX, double inY,
                          GamePage *inParentPage = NULL,
                          HouseObjectPicker *inPicker = NULL );
        
        virtual ~HouseGridDisplay();
        

        // destroyed by caller
        virtual void setHouseMap( char *inHouseMap );
        
        
        // destroyed by caller
        char *getHouseMap();
        char *getEditList();
        
        char isGoalSet();
        

        // disable placement... for example, if out of money
        void allowPlacement( char inAllow );
        
        int getLastPlacedObject();
        

        char canUndo();
        
        // returns cost of change that was undone
        int undo();
        
        
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
        GamePage *mParentPage;
        HouseObjectPicker *mPicker;

        int mRobberIndex;

        char *mHouseMap;
        
        int mNumMapSpots;
        int *mHouseMapIDs;

        int *mHouseMapCellStates;

        int *mHouseMapMobileIDs;
        int *mHouseMapMobileCellStates;
        
        int mFullMapD;

        // maps each cell to a noise overlay sprite
        int *mHouseMapNoiseTileIndices;
        

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
        
        // copy contents of all cells back into sub map
        void copyAllIntoSubCells();
        
        
        // can be overridded to do special processing after robber moves
        virtual void moveRobber( int inNewIndex );
        
        
        char isSubMapPropertySet( int inSubCellIndex, propertyID inProperty );

        
        int mHighlightIndex;
        

        int mPointerDownIndex;
        char mDraggedAway;
        
        int mPointerDownObjectID;
        // false if erase on drag
        char mPlaceOnDrag;
        
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
        // returns object ID
        int getTileNeighbor( int inIndex, int inNeighbor );
        // checks structural property of neighbor
        int getTileNeighborStructural( int inIndex, int inNeighbor );
        
        
        // inIndex is tile's sub-map index
        // inTileID might be different from what's actually in the map
        //   at inIndex (so getOrientationIndex can be used for orienting
        //   a ghost tile before placement)
        int getOrientationIndex( int inIndex, int inTileID, int inTileState );
        
        
        void pointerOver( float inX, float inY );


        SpriteHandle mWallShadowSprite;
        
        
        void drawTiles( char inNonStructuralOnly );
        

        void recomputeWallShadows();

        
        char mAllowPlacement;
        int mLastPlacedObject;


        SimpleVector<GridChangeRecord> mEditHistory;

        // call this BEFORE setting edit in full grid
        void logEdit( int inFullIndex, int inNewID );
        

        // maintain one bank of noise tiles for all instances
        // (to prevent noise tiles jumping when we switch views)
        static char sInstanceCount;
        static char sNoiseTileBankPopulated;
        static SpriteHandle sNoiseTileBank[ NUM_NOISE_TILES ];

        // track one set of static cell-to-noise-tile mappings
        // as long as house map never changes, all instances will copy this
        static int *sHouseMapNoiseTileIndices;
    };



#endif
