#include "GamePage.h"

#include "TextField.h"
#include "TextButton.h"


#include "minorGems/ui/event/ActionListener.h"


typedef struct RobberyLog {
        char *robberName;
        char *victimName;
        char *houseMap;
        char *loadout;
        char *moveList;
    } RobberyLog;



class FetchRobberyReplayPage : public GamePage, public ActionListener {
        
    public:
        
        FetchRobberyReplayPage();
        
        virtual ~FetchRobberyReplayPage();

        // must be called before makeActive
        void setLogID( int inID );
        

        virtual char getReturnToMenu();
        
        virtual char getRecordReady();

        // members destroyed internally
        virtual RobberyLog getLogRecord();
        
        virtual void actionPerformed( GUIComponent *inTarget );


        virtual void step();
        
        virtual void makeActive( char inFresh );

    protected:
        
        int mWebRequest;

        char mRecordReady;
        RobberyLog mLogRecord;

        TextButton mMenuButton;


        char mReturnToMenu;
        
        int mLogID;

        void clearRecord();
    };

