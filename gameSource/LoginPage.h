#include "GamePage.h"

#include "TextField.h"
#include "TextButton.h"


#include "minorGems/ui/event/ActionListener.h"


class LoginPage : public GamePage, public ActionListener {
        
    public:
        
        LoginPage();
        
        virtual ~LoginPage();

        
        char getLoginDone();
        

        virtual void actionPerformed( GUIComponent *inTarget );
        


        virtual void step();
        
        virtual void draw( doublePair inViewCenter, 
                           double inViewSize );
        
        virtual void makeActive( char inFresh );
        virtual void makeNotActive();
        

        // for TAB and ENTER (switch fields and start login)
        virtual void keyDown( unsigned char inASCII );
        
        // for arrow keys (switch fields)
        virtual void specialKeyDown( int inKeyCode );
        
    protected:
        
        TextField mEmailField;
        TextField mTicketField;

        TextField *mFields[2];
        
        TextButton mLoginButton;

        char mLoggedIn;
        

        char *mServerURL;

        int mRequestSteps;
        int mWebRequest;


        void switchFields();
        
        void startLogin();
        

        void acceptInput();

    };

