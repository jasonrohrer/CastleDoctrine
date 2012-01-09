#ifndef BUTTON_INCLUDED
#define BUTTON_INCLUDED


#include "PageComponent.h"

#include "minorGems/game/Font.h"
#include "minorGems/ui/event/ActionListenerList.h"


// fires actionPerformed whenever button pressed
class Button : public PageComponent, public ActionListenerList {
        
    public:
        
        // centered on inX, inY

        // label text copied internally
        Button( Font *inDisplayFont, 
                double inX, double inY,
                const char *inLabelText );

        virtual ~Button();
        

        virtual void setVisible( char inIsVible );
        
        virtual char isVisible();

        
        virtual void clearState();
        
        virtual void step();
        
        virtual void draw();

        virtual void pointerMove( float inX, float inY );
        virtual void pointerDown( float inX, float inY );
        virtual void pointerDrag( float inX, float inY );
        
        // fires action performed to listener list
        virtual void pointerUp( float inX, float inY );        

        
    protected:
        char mVisible;
        
        Font *mFont;
        double mX, mY;
        char *mLabelText;

        char mHover;
        char mDragOver;

        double mWide, mHigh, mPixWidth;
        
        char isInside( float inX, float inY );

        
    };



#endif
