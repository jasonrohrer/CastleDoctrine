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

        // destroyed by caller if not NULL
        virtual char *getVaultContents();

        // destroyed by caller if not NULL
        virtual char *getBackpackContents();

        // destroyed by caller if not NULL
        virtual char *getPriceList();

        virtual int getLootValue();
        
        virtual void actionPerformed( GUIComponent *inTarget );


        virtual void step();
        
        virtual void makeActive( char inFresh );

    protected:

        int mWebRequest;

        char *mHouseMap;
        char *mVaultContents;
        char *mBackpackContents;
        char *mPriceList;
        
        int mLootValue;

        TextButton mMenuButton;


        char mReturnToMenu;

    };

