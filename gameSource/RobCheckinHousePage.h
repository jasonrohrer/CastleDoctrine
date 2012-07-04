#include "GamePage.h"

#include "TextField.h"
#include "TextButton.h"
#include "inventory.h"

#include "minorGems/ui/event/ActionListener.h"


class RobCheckinHousePage : public GamePage, public ActionListener {
        
    public:
        
        RobCheckinHousePage();
        
        virtual ~RobCheckinHousePage();


        virtual char getReturnToHome();
        
        // did robber die?
        virtual char getStartOver();
        

        // destroyed by caller
        virtual void setHouseMap( char *inHouseMap );
        
        virtual void setSuccess( char inSuccess );

        // destoryed by caller
        virtual void setBackpackContents( char *inBackpackContents );

        // destoryed by caller
        virtual void setMoveList( char *inMoveList );
        

        virtual void actionPerformed( GUIComponent *inTarget );
        


        virtual void step();

        
        virtual void makeActive( char inFresh );


    protected:

        int mWebRequest;

        char *mBackpackContents;
        char *mMoveList;
        char *mHouseMap;
        
        char mSuccess;

        TextButton mHomeButton;
        TextButton mStartOverButton;
        
        InventorySlotButton *mVaultSlots[ NUM_VAULT_SLOTS ];

        char mReturnToHome;
        char mStartOver;

    };

