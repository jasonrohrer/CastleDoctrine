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
        
        // list of sold items removed from vault/backpack
        void setSellList( char *inSellList );
        char *getSellList();

        
        // destroyed by caller
        void setPriceList( char *inPriceList );
        
        
        void setLootValue( int inLootValue );
        
        int getLootValue() {
            return mLootValue;
            }
        

        char getDone() {
            return mDone;
            }
        

        char showGridToolPicker() {
            return mShowGridToolPicker;
            }
        
        HouseObjectPicker *getToolPicker() {
            return &mToolPicker;
            }


        virtual void actionPerformed( GUIComponent *inTarget );


        virtual void draw( doublePair inViewCenter, 
                           double inViewSize );
        
        
        virtual void makeActive( char inFresh );



    protected:
        
        int mLootValue;
        
        char mSellMode;
        char mSellHalfMode;
        
        HouseObjectPicker mToolPicker;
        TextButton mDoneButton;
        TextButton mSellModeButton;
        TextButton mSellHalfButton;
        TextButton mSellOneButton;

        TextButton mBuyModeButton;
        KeyEquivalentTextButton mUndoButton;
        
        SpriteButton mBuyButton;

        char mDone;
        char mShowGridToolPicker;
        

        InventorySlotButton *mPackSlots[ NUM_PACK_SLOTS ];

        InventorySlotButton *mVaultSlots[ NUM_VAULT_SLOTS ];

        SimpleVector<QuantityRecord> mPurchaseRecords;
        SimpleVector<QuantityRecord> mSellRecords;
        
        // for undo
        // positive quanties here represent purchases
        // negative quantities represent sales
        SimpleVector<QuantityRecord> mPurchaseHistory;
        

        void checkBuyButtonStatus();
        void checkUndoStatus();
        
        void checkSellModeStatus();


    };

