#include "LiveHousePage.h"

#include "TextButton.h"
#include "KeyEquivalentTextButton.h"
#include "InventorySlotButton.h"

#include "HouseObjectPicker.h"

#include "inventory.h"



#include "minorGems/ui/event/ActionListener.h"
#include "minorGems/util/SimpleVector.h"




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
        
        int getLootValue() {
            return mLootValue;
            }
        

        char getDone() {
            return mDone;
            }
            

        virtual void actionPerformed( GUIComponent *inTarget );


        virtual void draw( doublePair inViewCenter, 
                           double inViewSize );
        
        
        virtual void makeActive( char inFresh );



    protected:
        
        int mLootValue;

        HouseObjectPicker mToolPicker;
        TextButton mDoneButton;
        KeyEquivalentTextButton mUndoButton;
        
        SpriteButton mBuyButton;

        char mDone;
        

        InventorySlotButton *mPackSlots[ NUM_PACK_SLOTS ];

        InventorySlotButton *mVaultSlots[ NUM_VAULT_SLOTS ];

        SimpleVector<QuantityRecord> mPurchaseRecords;
        
        // for undo
        SimpleVector<int> mPurchaseHistory;
        

        void checkBuyButtonStatus();
        void checkUndoStatus();
        


    };

