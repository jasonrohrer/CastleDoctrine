

#include "GamePage.h"
#include "PageComponent.h"

#include "SpriteButton.h"



typedef struct HouseRecord {
        int userID;
        char *characterName;
        int lootValue;
        int robAttempts;

        char selected;
        doublePair position;

        char draw;
    } HouseRecord;


class RobPickList : public PageComponent, public ActionListener {
        
    public:
        
        
        RobPickList( double inX, double inY,
                     GamePage *inParentPage );
        

        virtual ~RobPickList();
        
        
        virtual void actionPerformed( GUIComponent *inTarget );

        
        // fetch new results from server
        virtual void refreshList( char inPreservePosition = false );
        

        virtual void step();
        
        virtual void draw();


        virtual void pointerUp( float inX, float inY );
        

    protected:
        
        GamePage *mParentPage;


        int mCurrentSkip;
        
        int mWebRequest;
        
        int mProgressiveDrawSteps;
        

        SimpleVector<HouseRecord> mHouseList;

        void clearHouseList();
        
        
        SpriteButton mUpButton;
        SpriteButton mDownButton;


    };


        
