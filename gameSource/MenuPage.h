#include "GamePage.h"

#include "TextField.h"
#include "Button.h"

#include "minorGems/util/SimpleVector.h"



typedef struct HouseRecord {
        int userID;
        char *characterName;
        int lootValue;
        int robAttempts;

        char selected;
        doublePair position;
    } HouseRecord;



class MenuPage : public GamePage {
        
    public:
        
        MenuPage();
        
        virtual ~MenuPage();


        virtual char getStartEditHouse();

        virtual void step();
        
        virtual void draw( doublePair inViewCenter, 
                           double inViewSize );
        
        virtual void makeActive();
        virtual void makeNotActive();
        
        
        virtual void pointerMove( float inX, float inY );
        virtual void pointerDown( float inX, float inY );
        virtual void pointerDrag( float inX, float inY );
        virtual void pointerUp( float inX, float inY );

        /*
        virtual void keyDown( unsigned char inASCII );
        virtual void keyUp( unsigned char inASCII );
        
        virtual void specialKeyDown( int inKeyCode );
        virtual void specialKeyUp( int inKeyCode );
        */

    protected:

        int mWebRequest;

        char mStatusError;
        const char *mStatusMessageKey;
        

        Button mEditHouseButton;


        char mStartEditHouse;
        
        SimpleVector<HouseRecord> mHouseList;

        void clearHouseList();
        
    };

