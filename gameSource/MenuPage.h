#include "GamePage.h"

#include "TextField.h"
#include "Button.h"
#include "RobPickList.h"

#include "minorGems/util/SimpleVector.h"


#include "minorGems/ui/event/ActionListener.h"




class MenuPage : public GamePage, public ActionListener {
        
    public:
        
        MenuPage();
        
        virtual ~MenuPage();


        virtual char getStartEditHouse();


        virtual void actionPerformed( GUIComponent *inTarget );


        virtual void step();
        
        virtual void draw( doublePair inViewCenter, 
                           double inViewSize );
        
        virtual void makeActive( char inFresh );

    protected:
        

        RobPickList mPickList;

        Button mEditHouseButton;


        char mStartEditHouse;
        
        
        
    };

