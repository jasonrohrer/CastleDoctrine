#include "GamePage.h"

#include "TextField.h"
#include "TextButton.h"
#include "RobPickList.h"

#include "minorGems/util/SimpleVector.h"


#include "minorGems/ui/event/ActionListener.h"




class RobberyReplayMenuPage : public GamePage, public ActionListener {
        
    public:
        
        RobberyReplayMenuPage();
        
        virtual ~RobberyReplayMenuPage();


        virtual char getReturnToMenu();
        virtual char getStartReplay();

        virtual int getLogID();
        

        virtual void actionPerformed( GUIComponent *inTarget );


        virtual void step();
        
        virtual void draw( doublePair inViewCenter, 
                           double inViewSize );
        
        virtual void makeActive( char inFresh );

    protected:
        

        RobPickList mPickList;

        TextButton mMenuButton;
        TextButton mReplayButton;
        
        

        char mReturnToMenu;
        char mStartReplay;
        
    };

