#include "GamePage.h"

#include "TextField.h"
#include "TextButton.h"


#include "minorGems/ui/event/ActionListener.h"


class RobCheckoutHousePage : public GamePage, public ActionListener {
        
    public:
        
        RobCheckoutHousePage();
        
        virtual ~RobCheckoutHousePage();

        // must be called before makeActive
        void setToRobUserID( int inID );
        void setToRobCharacterName( const char *inName );

        virtual char getReturnToMenu();
        
        // destroyed by caller if not NULL
        virtual char *getOwnerName();

        // destroyed by caller if not NULL
        virtual char *getHouseMap();
        
        // destroyed by caller if not NULL
        virtual char *getBackpackContents();

        // destroyed by caller if not NULL
        virtual char *getGalleryContents();

        virtual int getMusicSeed();

        virtual void actionPerformed( GUIComponent *inTarget );


        virtual void step();
        
        virtual void makeActive( char inFresh );

    protected:

        int mWebRequest;
        
        char *mOwnerName;
        char *mHouseMap;
        char *mBackpackContents;
        char *mGalleryContents;
        int mMusicSeed;
        
        TextButton mMenuButton;


        char mReturnToMenu;
        
        int mToRobUserID;
        char *mToRobCharacterName;
    };

