

#include "GamePage.h"
#include "PageComponent.h"



typedef struct HouseRecord {
        int userID;
        char *characterName;
        int lootValue;
        int robAttempts;

        char selected;
        doublePair position;
    } HouseRecord;


class RobPickList : public PageComponent {
        
    public:
        
        
        RobPickList( double inX, double inY,
                     GamePage *inParentPage );
        

        virtual ~RobPickList();
        
        
        // fetch new results from server
        virtual void refreshList();
        

        virtual void step();
        
        virtual void draw();


        virtual void pointerUp( float inX, float inY );
        

    protected:
        
        double mX, mY;
        
        GamePage *mParentPage;

        int mWebRequest;


        SimpleVector<HouseRecord> mHouseList;

        void clearHouseList();
        
    };


        
