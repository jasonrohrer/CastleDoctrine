#include "GamePage.h"

#include "TextField.h"
#include "Button.h"

#include "HouseGridDisplay.h"



class EditHousePage : public GamePage {
        
    public:
        
        EditHousePage();
        
        virtual ~EditHousePage();
        
        
        // destroyed by caller
        void setHouseMap( char *inHouseMap );
        char *getHouseMap();


        char getDone() {
            return mDone;
            }
            


        virtual void step();
        
        virtual void draw( doublePair inViewCenter, 
                           double inViewSize );
        
        virtual void makeActive( char inFresh );
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

        HouseGridDisplay mGridDisplay;
        Button mDoneButton;
        
        char mDone;
        

    };

