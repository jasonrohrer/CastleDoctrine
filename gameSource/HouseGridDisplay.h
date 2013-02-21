#ifndef HOUSE_GRID_DISPLAY_INCLUDED
#define HOUSE_GRID_DISPLAY_INCLUDED


#include "minorGems/game/doublePair.h"


#include "PageComponent.h"

#include "houseObjects.h"

#include "HouseObjectPicker.h"

#include "pathFind.h"

#include "minorGems/ui/event/ActionListenerList.h"

#include "minorGems/util/SimpleVector.h"


#include "minorGems/game/gameGraphics.h"


#define HOUSE_D 13


#define NUM_NOISE_TILES 100


#define GOAL_ID  999
#define START_ID  997
#define PLAYER_ID 1000




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
                          HouseObjectPicker *inPicker = NULL );
        
        virtual ~HouseGridDisplay();
        
        // destroyed by caller
        void setWifeName( const char *inWifeName );
        void setSonName( const char *inSonName );
        void setDaughterName( const char *inDaughterName );

        // destroyed by caller
        virtual void setHouseMap( const char *inHouseMap );
        
        virtual void setWifeMoney( int inMoney );
        
        
        // destroyed by caller
        char *getWifeName();
        char *getSonName();
        char *getDaughterName();

        char *getHouseMap();
        char *getEditList();
        
        char *getFamilyExitPaths();

        char getWifeLiving();

        char areMandatoriesPlaced();
        
        char doAllFamilyObjectsHaveExitPath();
        

        // disable placement... for example, if out of money
        void allowPlacement( char inAllow );
        
        int getLastPlacedObject();
        

        char canUndo();
        
        // returns cost of change that was undone
        int undo();


        // resets all non-stuck objects to inTargetState
        // (example:  reset to 1 for presentation to robber, or 0 for 
        //    presentation to owner)
        void resetToggledStates( int inTargetState );

        
        
        virtual void setVisibleOffset( int inXOffset, int inYOffset );
        

        virtual void step();
        
        virtual void draw();

        virtual void pointerMove( float inX, float inY );
        virtual void pointerDown( float inX, float inY );
        virtual void pointerDrag( float inX, float inY );

        virtual void pointerUp( float inX, float inY );

        
        // WASD/IJKL key movement
        void keyDown( unsigned char inASCII );
        
        // arrow key movement
        virtual void specialKeyDown( int inKeyCode );
        virtual void specialKeyUp( int inKeyCode );
        

    protected:
        HouseObjectPicker *mPicker;

        char mHideRobber;

        int mLastRobberIndex;
        int mRobberIndex;
        int mRobberState;
        int mRobberOrientation;
        
        int mWifeMoney;

        char *mWifeName;
        char *mSonName;
        char *mDaughterName;
        

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
        
        // subclasses can override to receive notification of
        // robber leaving
        // base class does not allow robber to leave
        virtual void robberTriedToLeave() { 
            };
        

        // for when robber should not be drawn 
        virtual void hideRobber( char inHidden ) {
            mHideRobber = inHidden;
            }
        
        
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

        // get relative position of tile at full-map index inFullIndex
        doublePair getTilePosFull( int inFullIndex );

        
        double mTileRadius;

        int mStartIndex;
        int mGoalIndex;

        char mMandatoryNeedsPlacing;
        int mMandatoryToPlaceID;

        // places mMandatoryToPlaceID and handles logging edit, etc.
        void placeMandatory( int inFullIndex, int inIndex );
        
        char mAllFamilyObjectsHaveExitPath;

        // contains mNumMapSpots elements for family pathfinding
        // destroyed by caller
        char *getBlockedMap();
        
        void checkExitPaths();

        // paths as allocated arrays of GridPos 
        SimpleVector<GridPos *> mFamilyExitPaths;
        SimpleVector<int> mFamilyExitPathLengths;
        
        int posToIndex( GridPos inPos );


        // inFullIndex is tile's full-map index
        // inNeighbor pick LBRT neighbor, in that order (L=0, T=3)
        // returns neighbor full map index, or -1 if out of bounds
        int getTileNeighbor( int inFullIndex, int inNeighbor );
        // checks structural property of neighbor
        int getTileNeighborStructural( int inFullIndex, int inNeighbor );
        
        
        // inFullIndex is tile's full-map index
        // inTileID might be different from what's actually in the map
        //   at inIndex (so getOrientationIndex can be used for orienting
        //   a ghost tile before placement)
        int getOrientationIndex( int inFullIndex, 
                                 int inTileID, int inTileState );
        
        
        char mPointerInside;
        
        virtual void pointerOver( float inX, float inY );


        SpriteHandle mWallShadowSprite;

        
        void drawTiles( char inBeneathShadowsOnly );
        

        void recomputeWallShadows();

        
        char mAllowPlacement;
        int mLastPlacedObject;


        SimpleVector<GridChangeRecord> mEditHistory;

        // call this BEFORE setting edit in full grid
        void logEdit( int inFullIndex, int inNewID );
        

        // internal version of resetToggledStates (above)
        // does not perform sub-map copy-back or shadow recomputation
        void resetToggledStatesInternal( int inTargetState );



        // maintain one bank of noise tiles for all instances
        // (to prevent noise tiles jumping when we switch views)
        static char sInstanceCount;
        static char sNoiseTileBankPopulated;
        static SpriteHandle sNoiseTileBank[ NUM_NOISE_TILES ];

        // track one set of static cell-to-noise-tile mappings
        // as long as house map never changes, all instances will copy this
        static int *sHouseMapNoiseTileIndices;
                
        static SpriteHandle sDropShadowSprite;


        void drawDropShadow( doublePair inPosition );
        
        void drawDarkHaloBehind( int inObjectID, int inOrientation,
                                 int inState, 
                                 doublePair inPosition );
        
        void drawRobber( doublePair inPosition );

        
        
        // set locations of target highlights
        void setTargetHighlights( SimpleVector<int> *inToolTargetFullIndices );
        
        // sets one of the targets as picked (different color)
        // set to -1 to turn picked color off
        void setPickedTargetHighlight( int inPickedFullIndex );

        SpriteHandle mToolTargetSprite;
        SpriteHandle mToolTargetBorderSprite;
        
        SimpleVector<int> mToolTargetFullIndices;

        int mToolTargetPickedFullIndex;
        
    };



#endif
