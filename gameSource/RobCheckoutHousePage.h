#include "GamePage.h"

#include "TextField.h"
#include "TextButton.h"


#include "minorGems/ui/event/ActionListener.h"


class RobCheckoutHousePage : public GamePage, public ActionListener {
        
    public:
        
        RobCheckoutHousePage();
        
        virtual ~RobCheckoutHousePage();

        // must be called before makeActive
        void setToRobUserID( int inID );
        

        virtual char getReturnToMenu();
        
        // destroyed by caller if not NULL
        virtual char *getOwnerName();

        // destroyed by caller if not NULL
        virtual char *getHouseMap();
        
        virtual void actionPerformed( GUIComponent *inTarget );


        virtual void step();
        
        virtual void makeActive( char inFresh );

    protected:

        int mWebRequest;
        
        char *mOwnerName;
        char *mHouseMap;


        TextButton mMenuButton;


        char mReturnToMenu;
        
        int mToRobUserID;
    };

