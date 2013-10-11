#ifndef ROB_HOUSE_GRID_DISPLAY_INCLUDED
#define ROB_HOUSE_GRID_DISPLAY_INCLUDED


#include "HouseGridDisplay.h"

#include "minorGems/util/SimpleVector.h"


#define VIS_BLOWUP 2


// fires actionPerformed whenever robbery ends
class RobHouseGridDisplay : public HouseGridDisplay {
    
    public:

        RobHouseGridDisplay( double inX, double inY );
        
        virtual ~RobHouseGridDisplay();

        
        // 0 death
        // 1 reached vault
        // 2 chickened out
        int getSuccess();

        char getDead();
        int getDeathSourceID();
        int getDeathSourceState();

        char getWifeKilled();
        // true if robber killed wife and reached her body
        char getWifeRobbed();
        char getAnyFamilyKilled();

        // list as a whitespace-free string
        // destroyed by caller
        char *getMoveList();
        
        void startUsingTool( int inToolID );
        void stopUsingTool();
        
        // can check whether a tool has been used since the last
        // call to getToolJustUsed
        char getToolJustUsed();
        
        
        void setSafeMoveMode( char inOn );


        // true if robber standing in the entrance
        char getAboutToLeave();
        

        // override to update visibility when map changes
        virtual void setHouseMap( const char *inHouseMap );

        // override to copy in mobile final jump-back positions
        virtual char *getHouseMap();


        // override to shift visibility map too
        virtual void setVisibleOffset( int inXOffset, int inYOffset );
        
        // toggles whether leave display can be shown
        virtual void setLeaveCanBeShown( char inCanBeShown ) {
            mLeaveDisplayCanBeShown = inCanBeShown;
            }
        

        virtual void draw();

        // override so that they do nothing
        virtual void pointerMove( float inX, float inY );
        virtual void pointerDown( float inX, float inY );
        virtual void pointerDrag( float inX, float inY );
        virtual void pointerUp( float inX, float inY );

        
        // override to catch ENTER for safe move mode
        virtual void keyDown( unsigned char inASCII );


        // at end of robbery, deal with family/mobile repositioning
        // called automatically in cases where this grid display catches
        // the end condition (but not for externally-detected end conditions,
        // like suicide)
        void processFamilyAndMobilesAtEnd();


    protected:
        
        int mSuccess;
        char mDead;
        int mDeathSourceID;
        int mDeathSourceState;

        char mLeaveDisplayCanBeShown;
        SpriteHandle mLeaveSprite;

        int mCurrentTool;
        char mToolJustUsed;

        char mSafeMoveMode;
        char mSafeMoveProposed;
        int mSafeMoveIndex;
        char mSafeMoveConfirmed;

        SpriteHandle mSafeUpSprite;
        SpriteHandle mSafeLeftSprite;
        SpriteHandle mSafeDownSprite;
        SpriteHandle mSafeRightSprite;
        


        // 0 = visible (shroud transparent)
        // 255 = invisible (shroud opaque)
        unsigned char mVisibleMap[ 
            VIS_BLOWUP * HOUSE_D * VIS_BLOWUP * HOUSE_D ];

        // true/false for whether each spot wants to move towards visibility
        // (for smooth transitions)
        char mTargetVisibleMap[ VIS_BLOWUP * HOUSE_D * VIS_BLOWUP * HOUSE_D ];

        // for each tile, true if (even partly) visible
        // false if completely invisible
        char mTileVisibleMap[ HOUSE_D * HOUSE_D ];
        

        // set to true to force tool tips for all tiles, even if shrouded
        char mForceAllTileToolTips;
        

        SimpleVector<char *> mMoveList;

        void clearMoveList();

        // override to recompute visibility and check for goal reached
        virtual void moveRobber( int inNewIndex );
        
        virtual void robberTriedToLeave();


        virtual void recomputeVisibility();

        // two different sub-implementations
        virtual void recomputeVisibilityInt();
        virtual void recomputeVisibilityFloat();
        

        void applyTransitionsAndProcess();

        void applyCurrentTool( int inTargetFullIndex );

        // instead of running to exit, wife will run to inIndex
        // replaces her current exit path
        void startWifeSearchForDeadChild( int inIndex );

        
        // override from HouseGridDisplay
        void pointerOver( float inX, float inY );


        // so that living mobiles can return to where they started
        // at the beginning of the robbery, if starting position is not blocked
        int *mHouseMapMobileStartingPositions;
        
        // the states of mobiles after they jump-back at the end
        // (we don't draw this, ever, but we return it when getHouseMap is 
        //  called)
        int *mHouseMapMobileFinalIDs;
        int *mHouseMapMobileFinalCellStates;
        

        // step index for each family member
        SimpleVector<int> mFamilyExitPathProgress;
        SimpleVector<int> mFamilyObjects;
        // for each member:
        // 1 = alive in house
        // 2 = escaped
        // 0 = killed in house
        SimpleVector<int> mFamilyStatus;

        // starts false for all family members
        // they don't move while false
        // becomes true if they see robber
        SimpleVector<char> mFamilySeenRobber;
        
        
        

        // true if robber reaches wife's cell after killing her
        char mRobberStoleFromWife;
        
    };



#endif
