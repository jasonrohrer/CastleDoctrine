#include "GamePage.h"

#include "minorGems/ui/event/ActionListener.h"


class ServerShutdownPage : public GamePage {
        
    public:

        virtual void makeActive( char inFresh );

        virtual void draw( doublePair inViewCenter, 
                           double inViewSize );
    };

