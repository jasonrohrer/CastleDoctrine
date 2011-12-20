#include "minorGems/game/Font.h"

class TextField {
        
    public:
        
        // Fixed width font is used for sizing of text field
        // centered on inX, inY
        TextField( Font *inFixedFont, 
                   Font *inDisplayFont, 
                   double inX, double inY, int inCharsWide );

        virtual ~TextField();
        
        
        virtual void draw();
        
        virtual void pointerUp( float inX, float inY );

        virtual void keyDown( unsigned char inASCII );
        
        virtual void specialKeyDown( int inKeyCode );
        

        // makes this text field the only focused field.
        // causes it to respond to keystrokes that are passed to it
        virtual void focus();
        
        virtual char isFocused();

        
    protected:
        Font *mFont;
        double mX, mY;
        int mCharsWide;
        double mWide, mHigh;
        
        double mBorderWide;
        
        // width of a single character
        double mCharWidth;
        
        

        char mFocused;

        char *mText;

        // clever (!) way of handling focus?

        // keep static pointer to focused field (there can be only one)
        static TextField *sFocusedTextField;

    };

