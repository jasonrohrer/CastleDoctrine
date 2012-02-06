#ifndef SPRITE_BUTTON_INCLUDED
#define SPRITE_BUTTON_INCLUDED


#include "Button.h"

#include "minorGems/game/gameGraphics.h"


class SpriteButton : public Button {
        
    public:
        

        SpriteButton( SpriteHandle inSprite, 
                      // size of sprite in pixels
                      int inWide, int inHigh,
                      double inX, double inY,
                      double inDrawScale = 1.0 );
        
        SpriteButton( const char *inTGAFileName, double inX, double inY,
                      double inDrawScale = 1.0 );

        virtual ~SpriteButton();
        


    protected:
        
        SpriteHandle mSprite;

        double mDrawScale;

        
        // override
        virtual void drawContents();

        
    };



#endif
        
