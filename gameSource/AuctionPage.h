#include "LiveHousePage.h"

#include "TextButton.h"

#include "minorGems/util/SimpleVector.h"


#include "minorGems/ui/event/ActionListener.h"




class AuctionPage : public LiveHousePage, public ActionListener {
        
    public:
        
        AuctionPage();
        
        virtual ~AuctionPage();


        virtual char getDone();
        

        virtual void actionPerformed( GUIComponent *inTarget );


        virtual void step();
        
        virtual void draw( doublePair inViewCenter, 
                           double inViewSize );
        
        virtual void makeActive( char inFresh );

    protected:
        TextButton mDoneButton;
        TextButton mUpdateButton;

        int mWebRequest;

        int mSecondsUntilUpdate;
        int mBaseTimestamp;

        SimpleVector<char*> mAuctionItems;

        char mDone;

        void refreshPrices();
        
    };

