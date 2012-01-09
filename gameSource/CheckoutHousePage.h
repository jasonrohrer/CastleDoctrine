#include "GamePage.h"

#include "TextField.h"
#include "Button.h"


class CheckoutHousePage : public GamePage {
        
    public:
        
        CheckoutHousePage();
        
        virtual ~CheckoutHousePage();


        virtual char getReturnToMenu();
        
        // destroyed by caller if not NULL
        virtual char *getHouseMap();
        


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

        char *mHouseMap;

        char mStatusError;
        const char *mStatusMessageKey;
        

        Button mMenuButton;


        char mReturnToMenu;

    };
