#include "GamePage.h"

#include "TextField.h"
#include "TextButton.h"

#include "minorGems/ui/event/ActionListener.h"


class CheckinHousePage : public GamePage, public ActionListener {
        
    public:
        
        CheckinHousePage();
        
        virtual ~CheckinHousePage();


        virtual char getReturnToMenu();
        
        // destroyed by caller
        virtual void setHouseMap( char *inHouseMap );        
        virtual void setEditList( char *inEditList );
        virtual void setPriceList( char *inPriceList );
        

        virtual void actionPerformed( GUIComponent *inTarget );
        


        virtual void step();

        
        virtual void makeActive( char inFresh );


    protected:

        int mWebRequest;

        char *mHouseMap;
        char *mEditList;
        char *mPriceList;
        

        TextButton mMenuButton;


        char mReturnToMenu;

    };

