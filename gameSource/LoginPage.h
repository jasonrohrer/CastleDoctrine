#include "GamePage.h"

#include "TextField.h"
#include "Button.h"


class LoginPage : public GamePage {
        
    public:
        
        LoginPage();
        
        virtual ~LoginPage();


        virtual void step();
        
        virtual void draw( doublePair inViewCenter, 
                           double inViewSize );
        
        virtual void makeActive();
        virtual void makeNotActive();
        

        virtual void pointerMove( float inX, float inY );
        virtual void pointerDown( float inX, float inY );
        virtual void pointerDrag( float inX, float inY );
        virtual void pointerUp( float inX, float inY );

        virtual void keyDown( unsigned char inASCII );
        virtual void keyUp( unsigned char inASCII );
        
        virtual void specialKeyDown( int inKeyCode );
        virtual void specialKeyUp( int inKeyCode );

    protected:
        
        TextField mEmailField;
        TextField mTicketField;

        TextField *mFields[2];
        
        Button mLoginButton;


        void switchFields();

    };
