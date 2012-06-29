#include "LiveHousePage.h"

#include "TextField.h"
#include "TextButton.h"

#include "RobHouseGridDisplay.h"

#include "inventory.h"



#include "minorGems/ui/event/ActionListener.h"



class RobHousePage : public LiveHousePage, public ActionListener {
        
    public:
        
        RobHousePage( const char *inDoneButtonKey = "doneRob" );
        
        virtual ~RobHousePage();
        

        // set to false to hide backpack buttons
        // defaults to showing them
        void showBackpack( char inShow );
        
        
        // destroyed by caller
        void setHouseMap( char *inHouseMap );
        char *getHouseMap();

        // destroyed by caller
        void setBackpackContents( char *inBackpackContents );
        char *getBackpackContents();


        char getSuccess() {
            return mGridDisplay.getSuccess();
            }

        char *getMoveList() {
            return mGridDisplay.getMoveList();
            }
        
        char getDone() {
            return mDone;
            }
            
        void setDescription( const char *inDescription );
        


        virtual void actionPerformed( GUIComponent *inTarget );


        virtual void draw( doublePair inViewCenter, 
                   double inViewSize );

        
        virtual void makeActive( char inFresh );

    protected:
        char mShowBackpack;

        RobHouseGridDisplay mGridDisplay;
        TextButton mDoneButton;
        
        InventorySlotButton *mPackSlots[ NUM_PACK_SLOTS ];

        const char *mDoneButtonKey;
        
        char mDone;

        char *mDescription;

    };

