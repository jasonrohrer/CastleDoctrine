#include "GamePage.h"

#include "TextField.h"
#include "TextButton.h"

#include "minorGems/ui/event/ActionListener.h"


class CheckinHousePage : public GamePage, public ActionListener {
        
    public:
        
        CheckinHousePage();
        
        virtual ~CheckinHousePage();


        virtual char getReturnToMenu();
        virtual char getStartOver();
        
        // destroyed by caller
        virtual void setHouseMap( char *inHouseMap );        
        virtual void setEditList( char *inEditList );
        virtual void setPriceList( char *inPriceList );
        
        // 0 or 1
        virtual void setDied( int inDied );
        

        virtual void actionPerformed( GUIComponent *inTarget );
        


        virtual void step();

        
        virtual void makeActive( char inFresh );


    protected:

        int mWebRequest;

        char *mHouseMap;
        char *mEditList;
        char *mPriceList;
        
        int mDied;
        

        TextButton mMenuButton;
        TextButton mStartOverButton;


        char mReturnToMenu;
        char mStartOver;

    };

