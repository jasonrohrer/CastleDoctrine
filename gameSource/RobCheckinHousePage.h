#include "GamePage.h"

#include "TextField.h"
#include "TextButton.h"

#include "minorGems/ui/event/ActionListener.h"


class RobCheckinHousePage : public GamePage, public ActionListener {
        
    public:
        
        RobCheckinHousePage();
        
        virtual ~RobCheckinHousePage();


        virtual char getReturnToMenu();
        
        // destroyed by caller
        virtual void setHouseMap( char *inHouseMap );


        virtual void actionPerformed( GUIComponent *inTarget );
        


        virtual void step();

        
        virtual void makeActive( char inFresh );


    protected:

        int mWebRequest;

        char *mHouseMap;
        

        TextButton mMenuButton;


        char mReturnToMenu;

    };

