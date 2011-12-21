#include "TextField.h"

#include <string.h>

#include "minorGems/game/gameGraphics.h"
#include "minorGems/game/drawUtils.h"
#include "minorGems/util/stringUtils.h"
#include "minorGems/graphics/openGL/KeyboardHandlerGL.h"



// start:  none focused
TextField *TextField::sFocusedTextField = NULL;

int TextField::sDeleteFirstDelaySteps = 30;
int TextField::sDeleteNextDelaySteps = 2;




TextField::TextField( Font *inFixedFont,
                      Font *inDisplayFont, 
                      double inX, double inY, int inCharsWide )
        : mFont( inDisplayFont ), mX( inX ), mY( inY ), 
          mCharsWide( inCharsWide ),
          mFocused( false ), mText( new char[1] ),
          mCursorPosition( 0 ),
          mHoldDeleteSteps( -1 ), mFirstDeleteRepeatDone( false ) {
    
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



void TextField::step() {

    

    if( mHoldDeleteSteps > -1 ) {
        mHoldDeleteSteps ++;

        int stepsBetween = sDeleteFirstDelaySteps;
        
        if( mFirstDeleteRepeatDone ) {
            stepsBetween = sDeleteNextDelaySteps;
            }
        
        if( mHoldDeleteSteps > stepsBetween ) {
            // delete repeat
            mHoldDeleteSteps = 0;
            mFirstDeleteRepeatDone = true;
            
            deleteHit();
            }
        }
    
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


    char *textToDrawBase = stringDuplicate( mText );
    
    char *textToDraw = textToDrawBase;
    char tooLongFront = false;
    char tooLongBack = false;
    
    int cursorDrawPosition = mCursorPosition;

    int firstCharIndex = 0;
    

    while( mFont->measureString( textToDraw ) > mWide - 2 * mBorderWide ) {
        
        tooLongFront = true;
        
        textToDraw = &( textToDraw[1] );

        firstCharIndex++;

        cursorDrawPosition --;
        }

    if( tooLongFront ) {
        // as cursor backs up, don't ever let it back past the half-way
        // point in field
        while( firstCharIndex > 0 &&
               ( cursorDrawPosition < 0 
                 || mFont->measureString( 
                         &( textToDraw[ cursorDrawPosition ] ) )
                    > mWide / 2 + mCharWidth ) ) {
            
            tooLongBack = true;
        
            textToDraw = &( textToDraw[-1] );
            firstCharIndex --;
            
            textToDraw[ strlen( textToDraw ) - 1 ] = '\0';
            
            cursorDrawPosition ++;
            }
        }
    
    if( firstCharIndex == 0 ) {
        tooLongFront = false;
        }

    if( tooLongBack ) {
        // make sure it's not hanging off end again
        while( mFont->measureString( textToDraw ) > mWide - 2 * mBorderWide ) {
            textToDraw[ strlen( textToDraw ) - 1 ] = '\0';
            }
        }
    
        


    mFont->drawString( textToDraw, textPos, alignLeft );

    if( tooLongFront ) {
        // draw shaded overlay over left of string
        
        double verts[] = { rectStartX, rectStartY,
                           rectStartX, rectEndY,
                           rectStartX + 4 * mCharWidth, rectEndY,
                           rectStartX + 4 * mCharWidth, rectStartY };
        float vertColors[] = { 0.25, 0.25, 0.25, 1,
                               0.25, 0.25, 0.25, 1,
                               0.25, 0.25, 0.25, 0,
                               0.25, 0.25, 0.25, 0 };

        drawQuads( 1, verts , vertColors );
        }
    if( tooLongBack ) {
        // draw shaded overlay over right of string
        
        double verts[] = { rectEndX - 4 * mCharWidth, rectStartY,
                           rectEndX - 4 * mCharWidth, rectEndY,
                           rectEndX, rectEndY,
                           rectEndX, rectStartY };
        float vertColors[] = { 0.25, 0.25, 0.25, 0,
                               0.25, 0.25, 0.25, 0,
                               0.25, 0.25, 0.25, 1,
                               0.25, 0.25, 0.25, 1 };

        drawQuads( 1, verts , vertColors );
        }
    
    if( mFocused && cursorDrawPosition > -1 ) {            
        // make measurement to draw cursor

        char *beforeCursorText = stringDuplicate( textToDraw );
        
        beforeCursorText[ cursorDrawPosition ] = '\0';
        
        double cursorXOffset = mFont->measureString( beforeCursorText );
        
        if( cursorXOffset == 0 ) {
            cursorXOffset -= pixWidth;
            }

        delete [] beforeCursorText;
        
        setDrawColor( 0, 0, 0, 0.5 );
        
        drawRect( textPos.x + cursorXOffset, 
                  rectStartY - pixWidth,
                  textPos.x + cursorXOffset + pixWidth, 
                  rectEndY + pixWidth );
        }
    
    delete [] textToDrawBase;
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
    

    

    if( inASCII == 127 || inASCII == 8 ) {
        // delete
        deleteHit();
        
        mHoldDeleteSteps = 0;
        }
    else if( inASCII >= 32 ) {
        // add to it
        char *oldText = mText;
        
        char *preCursor = stringDuplicate( mText );
        preCursor[ mCursorPosition ] = '\0';
        char *postCursor = &( mText[ mCursorPosition ] );

        mText = autoSprintf( "%s%c%s", preCursor, inASCII, postCursor );
        
        delete [] preCursor;

        delete [] oldText;

        mCursorPosition++;
        
        mHoldDeleteSteps = -1;
        mFirstDeleteRepeatDone = false;
        }    
    }



void TextField::keyUp( unsigned char inASCII ) {
    if( inASCII == 127 || inASCII == 8 ) {
        // end delete hold down
        mHoldDeleteSteps = -1;
        mFirstDeleteRepeatDone = false;
        }
    }



void TextField::deleteHit() {
    if( mCursorPosition > 0 ) {

        char *oldText = mText;
        
        char *preCursor = stringDuplicate( mText );
        preCursor[ mCursorPosition - 1 ] = '\0';
        char *postCursor = &( mText[ mCursorPosition ] );

        mText = autoSprintf( "%s%s", preCursor, postCursor );
        
        delete [] preCursor;

        delete [] oldText;

        mCursorPosition--;
        }
    }



void TextField::specialKeyDown( int inKeyCode ) {
    if( !mFocused ) {
        return;
        }
    
    switch( inKeyCode ) {
        case MG_KEY_LEFT:
            mCursorPosition --;
            if( mCursorPosition < 0 ) {
                mCursorPosition = 0;
                }
            break;
        case MG_KEY_RIGHT:
            mCursorPosition ++;
            if( mCursorPosition > (int)strlen( mText ) ) {
                mCursorPosition = strlen( mText );
                }
            break;
        default:
            break;
        }
    
    }



void TextField::focus() {
    
    if( sFocusedTextField != NULL ) {
        // unfocus last focused
        sFocusedTextField->unfocus();
        }

    mFocused = true;
    sFocusedTextField = this;
    }



void TextField::unfocus() {
    mFocused = false;
 
    // hold-down broken if not focused
    mHoldDeleteSteps = -1;
    mFirstDeleteRepeatDone = false;

    if( sFocusedTextField == this ) {
        sFocusedTextField = NULL;
        }    
    }



char TextField::isFocused() {
    return mFocused;
    }



void TextField::setDeleteRepeatDelays( int inFirstDelaySteps,
                                       int inNextDelaySteps ) {
    sDeleteFirstDelaySteps = inFirstDelaySteps;
    sDeleteNextDelaySteps = inNextDelaySteps;
    }
