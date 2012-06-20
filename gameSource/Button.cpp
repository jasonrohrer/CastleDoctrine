#include "Button.h"
#include "minorGems/game/drawUtils.h"
#include "minorGems/game/gameGraphics.h"

#include <math.h>


Button::Button( double inX, double inY,
                double inWide, double inHigh,
                double inPixelSize )
        : PageComponent( inX, inY ),
          mVisible( true ), 
          mHover( false ), mDragOver( false ),
          mWide( inWide ), mHigh( inHigh ), mPixWidth( inPixelSize ),
          mMouseOverTip( NULL ) {
    }


Button::~Button() {
    }



void Button::setMouseOverTip( const char *inTipMessage ) {
    mMouseOverTip = inTipMessage;
    }




void Button::setVisible( char inIsVible ) {
    mVisible = inIsVible;
    
    if( ! mVisible ) {
        clearState();
        }
    }



char Button::isVisible() {
    return mVisible;
    }



void Button::clearState() {
    mHover = false;
    mDragOver = false;
    }


        
void Button::step() {
    }
        

void Button::draw() {
    if( mHover && ! mDragOver ) {    
        setDrawColor( 0.75, 0.75, 0.75, 1 );
        }
    else if( mDragOver ) {
        setDrawColor( 0.25, 0.25, 0.25, 1 );
        }
    else {
        setDrawColor( 0.5, 0.5, 0.5, 1 );
        }
    

    drawRect( - mWide / 2, - mHigh / 2, 
              mWide / 2, mHigh / 2 );

    if( mDragOver ) {
        setDrawColor( 0.1, 0.1, 0.1, 1 );
        }
    else {
        setDrawColor( 0.25, 0.25, 0.25, 1 );
        }
    
    double rectStartX = - mWide / 2 + mPixWidth;
    double rectStartY = - mHigh / 2 + mPixWidth;

    double rectEndX = mWide / 2 - mPixWidth;
    double rectEndY = mHigh / 2 - mPixWidth;
    
    drawRect( rectStartX, rectStartY,
              rectEndX, rectEndY );

    if( mDragOver ) {    
        setDrawColor( 0.75, 0.75, 0, 1 );
        }
    else if( mHover ) {
        setDrawColor( 1, 1, 0, 1 );
        }
    else {
        setDrawColor( 1, 1, 1, 1 );
        }
    
    drawContents();
    }



void Button::drawContents() {
    }




char Button::isInside( float inX, float inY ) {
    return fabs( inX ) < mWide / 2 &&
        fabs( inY ) < mHigh / 2;
    }



void Button::pointerMove( float inX, float inY ) {
    if( isInside( inX, inY ) ) {
        mHover = true;
        if( mMouseOverTip != NULL ) {
            setToolTipDirect( (char*)mMouseOverTip );
            }
        }
    else {
        mHover = false;
        }
    }


void Button::pointerDown( float inX, float inY ) {
    pointerDrag( inX, inY );
    }



void Button::pointerDrag( float inX, float inY ) {
    if( isInside( inX, inY ) ) {
        mDragOver = true;
        if( mMouseOverTip != NULL ) {
            setToolTipDirect( (char*)mMouseOverTip );
            }
        }
    else {
        mDragOver = false;
        }
    mHover = false;
    }
        


void Button::pointerUp( float inX, float inY ) {
    mDragOver = false;
    if( isInside( inX, inY ) ) {
        mHover = true;
        fireActionPerformed( this );
        }
    }        

