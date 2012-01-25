#include "TextButton.h"

#include "minorGems/util/stringUtils.h"



TextButton::TextButton( Font *inDisplayFont, 
                        double inX, double inY,
                        const char *inLabelText ) 
        : Button( inX, inY, 
                  // button width
                  inDisplayFont->measureString( inLabelText ) +
                    inDisplayFont->getFontHeight(),
                  // button height
                  2 * inDisplayFont->getFontHeight(),
                  // pix width
                  inDisplayFont->getFontHeight() / 8 ), 
          mFont( inDisplayFont ),
          mLabelText( stringDuplicate( inLabelText ) ) {
    }



TextButton::~TextButton() {
    delete [] mLabelText;
    }

        
void TextButton::drawContents() {
    // leave draw color set by Button
    
    doublePair textPos = { mX, mY };
    
    mFont->drawString( mLabelText, textPos, alignCenter );
    }

