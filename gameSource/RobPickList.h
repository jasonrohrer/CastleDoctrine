#ifndef ROB_PICK_LIST_INCLUDED
#define ROB_PICK_LIST_INCLUDED



#include "GamePage.h"
#include "PageComponent.h"

#include "SpriteButton.h"



typedef struct HouseRecord {
        // might be userID (in case of houses-to-rob list)
        //       or logID (in the case of a list of past robberies to replay)
        int uniqueID;
        // full name with "_" for spaces as sent by server
        char *rawCharacterName;
        // trimmed and spaced name for human display
        char *characterName;
        char *lastRobberName;
        int lootValue;
        int robAttempts;
        int robberDeaths;
        
        char selected;
        doublePair position;

        char draw;
    } HouseRecord;


// fires actionPerformed when selection changes
class RobPickList : public PageComponent, public ActionListener, 
                    public ActionListenerList {
        
    public:
        
        
        RobPickList( double inX, double inY,
                     // if false, show houses-to-rob list
                     // if true, show list of successful robberies to view
                     char inRobberyLog,
                     GamePage *inParentPage );
        

        virtual ~RobPickList();
        
        
        virtual void actionPerformed( GUIComponent *inTarget );

        
        // fetch new results from server
        virtual void refreshList( char inPreservePosition = false );
        
        
        // NULL if nothing selected
        // destroyed internally
        virtual HouseRecord *getSelectedHouse();
        

        virtual void step();
        
        virtual void draw();


        virtual void pointerUp( float inX, float inY );
        


    protected:
        
        GamePage *mParentPage;


        int mCurrentSkip;
        
        int mWebRequest;
        
        int mProgressiveDrawSteps;
        
        char mRobberyLog;

        SimpleVector<HouseRecord> mHouseList;

        void clearHouseList();
        
        
        SpriteButton mUpButton;
        SpriteButton mDownButton;


    };



#endif

        
