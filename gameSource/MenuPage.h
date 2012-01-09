#include "GamePage.h"

#include "TextField.h"
#include "Button.h"

#include "minorGems/util/SimpleVector.h"


#include "minorGems/ui/event/ActionListener.h"



typedef struct HouseRecord {
        int userID;
        char *characterName;
        int lootValue;
        int robAttempts;

        char selected;
        doublePair position;
    } HouseRecord;



class MenuPage : public GamePage, public ActionListener {
        
    public:
        
        MenuPage();
        
        virtual ~MenuPage();


        virtual char getStartEditHouse();


        virtual void actionPerformed( GUIComponent *inTarget );


        virtual void step();
        
        virtual void draw( doublePair inViewCenter, 
                           double inViewSize );
        
        virtual void makeActive( char inFresh );

    protected:

        int mWebRequest;

        char mStatusError;
        const char *mStatusMessageKey;
        

        Button mEditHouseButton;


        char mStartEditHouse;
        
        SimpleVector<HouseRecord> mHouseList;

        void clearHouseList();
        
    };

