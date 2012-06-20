#include "SpriteButton.h"

#include "minorGems/util/log/AppLog.h"



SpriteButton::SpriteButton( SpriteHandle inSprite,
                            int inWide, int inHigh,
                            double inX, double inY,
                            double inDrawScale )
        : Button( inX, inY, 
                  inWide * 2 * inDrawScale,
                  inHigh * 2 * inDrawScale, 
                  inDrawScale ), 
          mSprite( inSprite ), 
          mDrawScale( inDrawScale ) {

    }


SpriteButton::SpriteButton( const char *inTGAFileName, double inX, double inY,
                            double inDrawScale )
        : // placeholder until we load file below 
        Button( inX, inY, 
                1,
                1, 
                inDrawScale ), 
        mSprite( NULL ), 
        mDrawScale( inDrawScale ) {
    
    Image *image = readTGAFile( inTGAFileName );

    if( image != NULL ) {
        // fill Button's values here
        mWide = image->getWidth() * 2 * inDrawScale;
        mHigh = image->getHeight() * 2 * inDrawScale;
        
        mSprite = fillSprite( image );

        delete image;
        }
    else {
        AppLog::errorF( "Failed to read file for SpriteButton: %s",
                        inTGAFileName );
        }
    }




SpriteButton::~SpriteButton() {
    if( mSprite != NULL ) {
        freeSprite( mSprite );
        }
    }



void SpriteButton::drawContents() {
    if( mSprite != NULL ) {
        
        // leave draw color as set by Button's draw function
        
        doublePair center = { 0, 0 };
        
        drawSprite( mSprite, center, mDrawScale );
        }
    }
