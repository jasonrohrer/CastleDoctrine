#include "LiveHousePage.h"

#include "TextButton.h"
#include "GallerySlotButton.h"

#include "minorGems/util/SimpleVector.h"


#include "minorGems/ui/event/ActionListener.h"



#define NUM_AUCTION_SLOTS  15

#define NUM_AUCTION_SLOTS_PER_ROW  5


class AuctionPage : public LiveHousePage, public ActionListener {
        
    public:
        
        AuctionPage();
        
        virtual ~AuctionPage();


        virtual char getDone();

        void setLootValue( int inLootValue );

        virtual void actionPerformed( GUIComponent *inTarget );


        virtual void step();
        
        virtual void draw( doublePair inViewCenter, 
                           double inViewSize );
        
        virtual void makeActive( char inFresh );

    protected:
        TextButton mDoneButton;
        TextButton mUpdateButton;
        TextButton mBuyButton;
        
        GallerySlotButton *mAuctionSlots[ NUM_AUCTION_SLOTS ];

        int mAuctionPrices[ NUM_AUCTION_SLOTS ];

        int mLootValue;
        
        int mWebRequest;

        int mSecondsUntilUpdate;
        int mBaseTimestamp;

        SimpleVector<char*> mAuctionItems;

        char mDone;

        void refreshPrices();

        void turnAllRingsOff();

        char getPricesStale();
        
    };

