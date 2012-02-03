#include "GamePage.h"

#include "TextField.h"
#include "TextButton.h"

#include "minorGems/ui/event/ActionListener.h"


class RobCheckinHousePage : public GamePage, public ActionListener {
        
    public:
        
        RobCheckinHousePage();
        
        virtual ~RobCheckinHousePage();


        virtual char getReturnToMenu();
        
        // destroyed by caller
        virtual void setHouseMap( char *inHouseMap );
        
        virtual void setSuccess( char inSuccess );

        // destoryed by caller
        virtual void setMoveList( char *inMoveList );
        

        virtual void actionPerformed( GUIComponent *inTarget );
        


        virtual void step();

        
        virtual void makeActive( char inFresh );


    protected:

        int mWebRequest;

        char *mMoveList;
        char *mHouseMap;
        
        char mSuccess;

        TextButton mMenuButton;


        char mReturnToMenu;

    };

