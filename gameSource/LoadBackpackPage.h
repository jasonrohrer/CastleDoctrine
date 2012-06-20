#include "LiveHousePage.h"

#include "TextField.h"
#include "TextButton.h"
#include "KeyEquivalentTextButton.h"

#include "HouseObjectPicker.h"


#include "minorGems/ui/event/ActionListener.h"


#define NUM_PACK_SLOTS 8



class LoadBackpackPage : public LiveHousePage, public ActionListener {
        
    public:
        
        LoadBackpackPage();
        
        virtual ~LoadBackpackPage();

        // destroyed by caller
        void setVaultContents( char *inVaultContents );
        char *getVaultContents();


        // destroyed by caller
        void setBackpackContents( char *inBackpackContents );
        char *getBackpackContents();
        
        // list of purchases added to vault/backpack
        void setPurchaseList( char *inPurchaseList );
        char *getPurchaseList();
        
        
        // destroyed by caller
        void setPriceList( char *inPriceList );
        
        
        void setLootValue( int inLootValue );


        char getDone() {
            return mDone;
            }
            

        virtual void actionPerformed( GUIComponent *inTarget );


        virtual void draw( doublePair inViewCenter, 
                           double inViewSize );
        
        
        virtual void makeActive( char inFresh );

    protected:

        
        char *mVaultContents;
        char *mBackpackContents;


        char *mPurchaseList;
        
        int mLootValue;

        HouseObjectPicker mToolPicker;
        TextButton mDoneButton;
        KeyEquivalentTextButton mUndoButton;
        
        char mDone;
        

        int mPackSlots[ NUM_PACK_SLOTS ];
    };

