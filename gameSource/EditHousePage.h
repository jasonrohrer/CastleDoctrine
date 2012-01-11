#include "GamePage.h"

#include "TextField.h"
#include "Button.h"

#include "HouseGridDisplay.h"


#include "minorGems/ui/event/ActionListener.h"



class EditHousePage : public GamePage, public ActionListener {
        
    public:
        
        EditHousePage();
        
        virtual ~EditHousePage();
        
        
        // destroyed by caller
        void setHouseMap( char *inHouseMap );
        char *getHouseMap();


        char getDone() {
            return mDone;
            }
            

        virtual void actionPerformed( GUIComponent *inTarget );


        virtual void step();
        
        
        virtual void makeActive( char inFresh );

    protected:
        
        int mWebRequest;


        HouseGridDisplay mGridDisplay;
        Button mDoneButton;
        
        char mDone;
        

    };

