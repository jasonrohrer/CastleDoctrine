#include "GamePage.h"

#include "TextField.h"
#include "TextButton.h"


#include "minorGems/ui/event/ActionListener.h"


class CheckoutHousePage : public GamePage, public ActionListener {
        
    public:
        
        CheckoutHousePage();
        
        virtual ~CheckoutHousePage();


        virtual char getReturnToMenu();
        
        // destroyed by caller if not NULL
        virtual char *getHouseMap();
        
        virtual void actionPerformed( GUIComponent *inTarget );


        virtual void step();
        
        virtual void makeActive( char inFresh );

    protected:

        int mWebRequest;

        char *mHouseMap;


        TextButton mMenuButton;


        char mReturnToMenu;

    };

