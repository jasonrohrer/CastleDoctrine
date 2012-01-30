#include "GamePage.h"

#include "TextField.h"
#include "TextButton.h"

#include "HouseGridDisplay.h"


#include "minorGems/ui/event/ActionListener.h"



class RobHousePage : public GamePage, public ActionListener {
        
    public:
        
        RobHousePage();
        
        virtual ~RobHousePage();
        
        
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
        TextButton mDoneButton;
        
        char mDone;
        

    };

