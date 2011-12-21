#include "GamePage.h"

#include "TextField.h"


class LoginPage : public GamePage {
        
    public:
        
        LoginPage();
        
        virtual ~LoginPage();


        virtual void step();
        
        virtual void draw( doublePair inViewCenter, 
                           double inViewSize );
        
        virtual void makeActive();


        virtual void pointerUp( float inX, float inY );

        virtual void keyDown( unsigned char inASCII );
        virtual void keyUp( unsigned char inASCII );
        
        virtual void specialKeyDown( int inKeyCode );

    protected:
        
        TextField mEmailField;
        TextField mTicketField;

        TextField *mFields[2];
        

        void switchFields();

    };

