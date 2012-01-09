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
        
        virtual void draw( doublePair inViewCenter, 
                           double inViewSize );
        
        virtual void makeActive( char inFresh );

    protected:
        
        int mWebRequest;

        char mStatusError;
        const char *mStatusMessageKey;

        HouseGridDisplay mGridDisplay;
        Button mDoneButton;
        
        char mDone;
        

    };

