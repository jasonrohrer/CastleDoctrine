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
        void setHouseMap( char *inHouseMap );
        char *getHouseMap();


        // destroyed by caller
        void setVaultContents( char *inVaultContents );
        char *getVaultContents();


        // destroyed by caller
        void setBackpackContents( char *inBackpackContents );
        char *getBackpackContents();


        // destroyed by caller
        char *getEditList();
        
        // list of purchases added to vault/backpack
        char *getPurchaseList();
        
        
        // destroyed by caller
        void setPriceList( char *inPriceList );
        char *getPriceList();
        
        
        void setLootValue( int inLootValue );
        

        // true if map changed since last setHouseMap call
        char houseMapChanged();
        

        char getDone() {
            return mDone;
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
        
        int mLootValue;

        HouseObjectPicker mObjectPicker;
        HouseGridDisplay mGridDisplay;
        TextButton mDoneButton;
        KeyEquivalentTextButton mUndoButton;
        
        char mDone;


        void checkIfPlacementAllowed();

    };

