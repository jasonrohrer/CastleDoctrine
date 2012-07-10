#include "LiveHousePage.h"

#include "TextField.h"
#include "TextButton.h"
#include "KeyEquivalentTextButton.h"

#include "HouseGridDisplay.h"
#include "HouseObjectPicker.h"

#include "Gallery.h"

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
        void setGalleryContents( const char *inGalleryContents );
        char *getGalleryContents();


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
        
        void setMustSelfTest( char inMustSelfTest );
        


        // true if map changed since last setHouseMap call
        // or if self-map-test required for any other reason
        char houseMapChanged();
        

        char getDone() {
            return mDone;
            }

        char showLoadBackpack() {
            return mShowLoadBackpack;
            }
        
        char showAuctions() {
            return mShowAuctions;
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

        char mMustSelfTest;

        HouseObjectPicker mObjectPicker;
        HouseGridDisplay mGridDisplay;
        TextButton mDoneButton;
        TextButton mBackpackButton;
        TextButton mAuctionButton;
        KeyEquivalentTextButton mUndoButton;
        
        
        Gallery mGallery;
        
        char mDone;
        char mShowLoadBackpack;
        char mShowAuctions;

        void checkIfPlacementAllowed();

    };

