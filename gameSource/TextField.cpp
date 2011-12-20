#include "TextField.h"

#include <string.h>

#include "minorGems/game/gameGraphics.h"
#include "minorGems/game/drawUtils.h"
#include "minorGems/util/stringUtils.h"



// start:  none focused
TextField *TextField::sFocusedTextField = NULL;



TextField::TextField( Font *inFixedFont,
                      Font *inDisplayFont, 
                      double inX, double inY, int inCharsWide )
        : mFont( inDisplayFont ), mX( inX ), mY( inY ), 
          mCharsWide( inCharsWide ),
          mFocused( false ), mText( new char[1] ) {
    
    mCharWidth = inFixedFont->measureString( "W" );

    mBorderWide = mCharWidth * 0.25;

    mHigh = mCharWidth + 2 * mBorderWide;

    char *fullString = new char[ mCharsWide + 1 ];
    
    for( int i=0; i<mCharsWide; i++ ) {
        fullString[i] = 'W';
        }
    fullString[ mCharsWide ] = '\0';
    
    double fullStringWidth = inFixedFont->measureString( fullString );

    delete [] fullString;

    mWide = fullStringWidth + 2 * mBorderWide;
    
    

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
    double pixWidth = mCharWidth / 8;


    double rectStartX = mX - mWide / 2 + pixWidth;
    double rectStartY = mY - mHigh / 2 + pixWidth;

    double rectEndX = mX + mWide / 2 - pixWidth;
    double rectEndY = mY + mHigh / 2 - pixWidth;
    
    drawRect( rectStartX, rectStartY,
              rectEndX, rectEndY );
    
    setDrawColor( 1, 1, 1, 1 );

    doublePair textPos = { mX - mWide/2 + mBorderWide, mY };

    char *textToDraw = mText;
    char tooLong = false;

    while( mFont->measureString( textToDraw ) > mWide - 2 * mBorderWide ) {
        
        tooLong = true;
        
        textToDraw = &( textToDraw[1] );
        }

    mFont->drawString( textToDraw, textPos, alignLeft );

    if( tooLong ) {
        // draw shaded overlay over left of string
        
        double verts[] = { rectStartX, rectStartY,
                           rectStartX, rectEndY,
                           rectStartX + 2 * mCharWidth, rectEndY,
                           rectStartX + 2 * mCharWidth, rectStartY };
        float vertColors[] = { 0.25, 0.25, 0.25, 1,
                               0.25, 0.25, 0.25, 1,
                               0.25, 0.25, 0.25, 0,
                               0.25, 0.25, 0.25, 0 };

        drawQuads( 1, verts , vertColors );
        }
    
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
    
    char *oldText = mText;


    if( inASCII == 127 || inASCII == 8 ) {
        // delete
        if( oldLength > 0 ) {
            mText[ oldLength - 1 ] = '\0';
            }
        }
    else if( inASCII >= 32 ) {
        // add to it
        mText = autoSprintf( "%s%c", oldText, inASCII );
        
        delete [] oldText;
        }    
    }



void TextField::specialKeyDown( int inKeyCode ) {
    }



void TextField::focus() {
    
    if( sFocusedTextField != NULL ) {
        // unfocus last focused
        sFocusedTextField->mFocused = false;
        }

    mFocused = true;
    sFocusedTextField = this;
    }



char TextField::isFocused() {
    return mFocused;
    }
