#include "TextField.h"

#include <string.h>

#include "minorGems/game/gameGraphics.h"
#include "minorGems/game/drawUtils.h"



// start:  none focused
TextField *TextField::sFocusedTextField = NULL;



TextField::TextField( Font *inFont, double inX, double inY, int inCharsWide )
        : mFont( inFont ), mX( inX ), mY( inY ), mCharsWide( inCharsWide ),
          mFocused( false ), mText( new char[ inCharsWide + 1 ] ) {
    
    double charWidth = mFont->measureString( "W" );

    double borderWidth = charWidth * 0.25;

    mHigh = charWidth + 2 * borderWidth;

    char *fullString = new char[ mCharsWide + 1 ];
    
    for( int i=0; i<mCharsWide; i++ ) {
        fullString[i] = 'W';
        }
    fullString[ mCharsWide ] = '\0';
    
    double fullStringWidth = mFont->measureString( fullString );

    delete [] fullString;

    mWide = fullStringWidth + 2 * borderWidth;
    
    

    mText[0] = '\0';
    }



TextField::~TextField() {
    if( this == sFocusedTextField ) {
        // we're focused, now nothing is focused
        sFocusedTextField = NULL;
        }

    delete [] mText;
    }

        
        
void TextField::draw() {
    
    if( mFocused ) {    
        setDrawColor( 1, 1, 1, 1 );
        }
    else {
        setDrawColor( 0.5, 0.5, 0.5, 1 );
        }
    

    drawRect( mX - mWide / 2, mY - mHigh / 2, 
              mX + mWide / 2, mY + mHigh / 2 );
    
    setDrawColor( 0.25, 0.25, 0.25, 1 );
    

    double charWidth = mFont->measureString( "W" );

    double pixWidth = charWidth / 8;

    drawRect( mX - mWide / 2 + pixWidth, mY - mHigh / 2 + pixWidth, 
              mX + mWide / 2 - pixWidth, mY + mHigh / 2 - pixWidth );

    
    setDrawColor( 1, 1, 1, 1 );

    doublePair textPos = { mX - mWide/2, mY };

    mFont->drawString( mText, textPos, alignLeft );
    }


void TextField::pointerUp( float inX, float inY ) {
    if( inX > mX - mWide / 2 &&
        inX < mX + mWide / 2 &&
        inY > mY - mHigh / 2 &&
        inY < mY + mHigh / 2 ) {

        focus();
        }
    }


void TextField::keyDown( unsigned char inASCII ) {
    if( !mFocused ) {
        return;
        }
    

    int oldLength = strlen( mText );
    

    if( inASCII == 127 || inASCII == 8 ) {
        // delete
        if( oldLength > 0 ) {
            mText[ oldLength - 1 ] = '\0';
            }
        }
    else if( inASCII >= 32 ) {
        // add to it
        
        if( oldLength < mCharsWide ) {
            
            mText[ oldLength ] = inASCII;
            
            mText[ oldLength + 1 ] = '\0';
            }
        }    
    }



void TextField::specialKeyDown( int inKeyCode ) {
    }



// makes this text field the only focused field.
// causes it to respond to keystrokes that are passed to it
void TextField::focus() {
    
    if( sFocusedTextField != NULL ) {
        // unfocus last focused
        sFocusedTextField->mFocused = false;
        }

    mFocused = true;
    sFocusedTextField = this;
    }

