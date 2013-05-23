#include "GamePage.h"

#include "TextButton.h"

#include "BlueprintHouseGridDisplay.h"

#include "minorGems/ui/event/ActionListener.h"




class ViewBlueprintPage : public GamePage, public ActionListener {
        
    public:
        
        ViewBlueprintPage();
        
        virtual ~ViewBlueprintPage();
        
        
        
        // destroyed by caller
        void setHouseMap( const char *inHouseMap );

        void setDescription( const char *inDescription );

        void setViewOffset( int inXOffset, int inYOffset );
        

        char getDone() {
            return mDone;
            }
            

        virtual void actionPerformed( GUIComponent *inTarget );


        virtual void draw( doublePair inViewCenter, 
                           double inViewSize );
        
        
        virtual void makeActive( char inFresh );

    protected:


        BlueprintHouseGridDisplay mGridDisplay;
        TextButton mDoneButton;
        
        char mDone;
        
        char *mDescription;
    };

