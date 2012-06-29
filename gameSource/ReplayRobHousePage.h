#include "GamePage.h"

#include "TextButton.h"

#include "ReplayRobHouseGridDisplay.h"
#include "FetchRobberyReplayPage.h"

#include "inventory.h"


#include "minorGems/ui/event/ActionListener.h"



class ReplayRobHousePage : public GamePage, public ActionListener {
        
    public:
        
        ReplayRobHousePage();
        
        virtual ~ReplayRobHousePage();
        
        
        // members destroyed by caller
        void setLog( RobberyLog inLog );
        
        
        char getDone() {
            return mDone;
            }
            

        virtual void actionPerformed( GUIComponent *inTarget );


        virtual void step();
        

        virtual void draw( doublePair inViewCenter, 
                           double inViewSize );
        
        
        virtual void makeActive( char inFresh );

    protected:
        
        int mWebRequest;


        ReplayRobHouseGridDisplay mGridDisplay;
        TextButton mDoneButton;
        
        InventorySlotButton *mPackSlots[ NUM_PACK_SLOTS ];

        
        char mDone;
        
        char *mDescription;

    };

