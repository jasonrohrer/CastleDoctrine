#include "GamePage.h"

#include "TextField.h"
#include "TextButton.h"

#include "minorGems/ui/event/ActionListener.h"


class CheckinHousePage : public GamePage, public ActionListener {
        
    public:
        
        CheckinHousePage();
        
        virtual ~CheckinHousePage();


        virtual char getReturnToMenu();
        virtual char getStartOver();
        
        // destroyed by caller
        virtual void setHouseMap( char *inHouseMap );        
        virtual void setVaultContents( char *inVaultContents );
        virtual void setBackpackContents( char *inBackpackContents );
        virtual void setEditList( char *inEditList );
        virtual void setPurchaseList( char *inPurchaseList );
        virtual void setPriceList( char *inPriceList );
        
        // 0 or 1
        virtual void setDied( int inDied );
        

        virtual void actionPerformed( GUIComponent *inTarget );
        


        virtual void step();

        
        virtual void makeActive( char inFresh );


    protected:

        int mWebRequest;

        char *mHouseMap;
        char *mVaultContents;
        char *mBackpackContents;
        char *mEditList;
        char *mPurchaseList;
        char *mPriceList;
        
        int mDied;
        

        TextButton mMenuButton;
        TextButton mStartOverButton;


        char mReturnToMenu;
        char mStartOver;

    };

