#ifndef BUTTON_INCLUDED
#define BUTTON_INCLUDED



#include "minorGems/game/Font.h"

class Button {
        
    public:
        
        // centered on inX, inY

        // label text copied internally
        Button( Font *inDisplayFont, 
                double inX, double inY,
                const char *inLabelText );

        virtual ~Button();
        
        
        virtual void step();
        
        virtual void draw();

        virtual void pointerMove( float inX, float inY );
        virtual void pointerDown( float inX, float inY );
        virtual void pointerDrag( float inX, float inY );
        
        // returns true if button pressed as a result of
        // this event
        virtual char pointerUp( float inX, float inY );        

        
    protected:
        Font *mFont;
        double mX, mY;
        char *mLabelText;

        char mHover;
        char mDragOver;

        double mWide, mHigh, mPixWidth;
        
        char isInside( float inX, float inY );

        
    };



#endif
