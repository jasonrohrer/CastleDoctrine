#include "LiveHousePage.h"

#include "TextField.h"
#include "TextButton.h"
#include "KeyEquivalentTextButton.h"

#include "HouseGridDisplay.h"
#include "HouseObjectPicker.h"


#include "minorGems/ui/event/ActionListener.h"



class EditHousePage : public LiveHousePage, public ActionListener {
        
    public:
        
        EditHousePage();
        
        virtual ~EditHousePage();
        
        
        // destroyed by caller
        void setHouseMap( const char *inHouseMap );
        char *getHouseMap();


        // destroyed by caller
        void setVaultContents( const char *inVaultContents );
        char *getVaultContents();


        // destroyed by caller
        void setBackpackContents( const char *inBackpackContents );
        char *getBackpackContents();


        // destroyed by caller
        char *getEditList();
        
        // list of purchases added to vault/backpack
        void setPurchaseList( const char *inPurchaseList );
        char *getPurchaseList();
        
        
        // destroyed by caller
        void setPriceList( const char *inPriceList );
        char *getPriceList();
        
        
        void setLootValue( int inLootValue );
        
        int getLootValue() {
            return mLootValue;
            }
        

        // true if map changed since last setHouseMap call
        char houseMapChanged();
        

        char getDone() {
            return mDone;
            }

        char showLoadBackpack() {
            return mShowLoadBackpack;
            }
            

        virtual void actionPerformed( GUIComponent *inTarget );


        virtual void draw( doublePair inViewCenter, 
                           double inViewSize );
        
        
        virtual void makeActive( char inFresh );

    protected:

        // for change detection
        char *mStartHouseMap;
        
        char *mVaultContents;
        char *mBackpackContents;

        char *mPriceList;
        
        char *mPurchaseList;

        int mLootValue;

        HouseObjectPicker mObjectPicker;
        HouseGridDisplay mGridDisplay;
        TextButton mDoneButton;
        TextButton mBackpackButton;
        KeyEquivalentTextButton mUndoButton;
        
        char mDone;
        char mShowLoadBackpack;

        void checkIfPlacementAllowed();

    };

