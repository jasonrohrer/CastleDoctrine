#include "GamePage.h"

#include "TextField.h"
#include "TextButton.h"

#include "ReplayRobHouseGridDisplay.h"


#include "minorGems/ui/event/ActionListener.h"



class ReplayRobHousePage : public GamePage, public ActionListener {
        
    public:
        
        ReplayRobHousePage();
        
        virtual ~ReplayRobHousePage();
        
        
        // destroyed by caller
        void setHouseMap( char *inHouseMap );

        // destroyed by caller, must be set AFTER house map
        void setMoveList( char *inMoveList );
        
        
        char getDone() {
            return mDone;
            }
            

        virtual void actionPerformed( GUIComponent *inTarget );


        virtual void step();
        
        
        virtual void makeActive( char inFresh );

    protected:
        
        int mWebRequest;


        ReplayRobHouseGridDisplay mGridDisplay;
        TextButton mDoneButton;
        
        char mDone;
        

    };

