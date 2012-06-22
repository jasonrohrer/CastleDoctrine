#include "LiveHousePage.h"

#include "TextField.h"
#include "TextButton.h"
#include "KeyEquivalentTextButton.h"
#include "InventorySlotButton.h"

#include "HouseObjectPicker.h"


#include "minorGems/ui/event/ActionListener.h"
#include "minorGems/util/SimpleVector.h"


#define NUM_PACK_SLOTS 8

// must be a whole multiple of NUM_PACK_SLOTS
#define NUM_VAULT_SLOTS 48


typedef struct QuantityRecord {
        int objectID;
        int quantity;
    } QuantityRecord;





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
        

        // parses a string in the following form:
        // id:quanity#id:quanity#id:quantity (or # for an empty list)
        //
        // and adds the pairs as recors in a empty record list
        void fromString( char *inListString, 
                         SimpleVector<QuantityRecord> *inEmptyList );
        
        // converts a list of QuantityRecords to a string like
        //  id:quanity#id:quanity#id:quantity (or # for an empty list)
        char *toString( SimpleVector<QuantityRecord> *inQuantities );
        

        void addToQuantity( 
            SimpleVector<QuantityRecord> *inOldQuanties, int inAddObjectID );

        void subtractFromQuantity( SimpleVector<QuantityRecord> *inOldQuanties,
                                   int inSubtractObjectID );
        


    };

