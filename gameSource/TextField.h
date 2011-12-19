#include "minorGems/game/Font.h"

class TextField {
        
    public:
        
        // should use fixed-width font
        // centered on inX, inY
        TextField( Font *inFont, double inX, double inY, int inCharsWide );

        virtual ~TextField();
        
        
        virtual void draw();
        
        virtual void pointerUp( float inX, float inY );

        virtual void keyDown( unsigned char inASCII );
        
        virtual void specialKeyDown( int inKeyCode );
        

        // makes this text field the only focused field.
        // causes it to respond to keystrokes that are passed to it
        virtual void focus();
        

        
    protected:
        Font *mFont;
        double mX, mY;
        int mCharsWide;
        double mWide, mHigh;
        
        char mFocused;

        char *mText;

        // clever (!) way of handling focus?

        // keep static pointer to focused field (there can be only one)
        static TextField *sFocusedTextField;

    };

