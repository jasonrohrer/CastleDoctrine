#include "GamePage.h"

#include "TextField.h"
#include "TextButton.h"

#include "RobHouseGridDisplay.h"


#include "minorGems/ui/event/ActionListener.h"



class RobHousePage : public GamePage, public ActionListener {
        
    public:
        
        RobHousePage( const char *inDoneButtonKey = "doneRob" );
        
        virtual ~RobHousePage();
        
        
        // destroyed by caller
        void setHouseMap( char *inHouseMap );
        char *getHouseMap();

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


        virtual void step();

        virtual void draw( doublePair inViewCenter, 
                   double inViewSize );

        
        virtual void makeActive( char inFresh );

    protected:
        
        int mWebRequest;


        RobHouseGridDisplay mGridDisplay;
        TextButton mDoneButton;
        
        char mDone;

        char *mDescription;

        int mLastPingTime;

    };

