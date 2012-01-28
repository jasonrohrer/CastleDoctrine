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
          mWide( inWide ), mHigh( inHigh ), mPixWidth( inPixelSize ) {
    }


Button::~Button() {
    }



void Button::setVisible( char inIsVible ) {
    mVisible = inIsVible;
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
    

    drawRect( mX - mWide / 2, mY - mHigh / 2, 
              mX + mWide / 2, mY + mHigh / 2 );

    if( mDragOver ) {
        setDrawColor( 0.1, 0.1, 0.1, 1 );
        }
    else {
        setDrawColor( 0.25, 0.25, 0.25, 1 );
        }
    
    double rectStartX = mX - mWide / 2 + mPixWidth;
    double rectStartY = mY - mHigh / 2 + mPixWidth;

    double rectEndX = mX + mWide / 2 - mPixWidth;
    double rectEndY = mY + mHigh / 2 - mPixWidth;
    
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
    return fabs( mX - inX ) < mWide / 2 &&
        fabs( mY - inY ) < mHigh / 2;
    }



void Button::pointerMove( float inX, float inY ) {
    if( isInside( inX, inY ) ) {
        mHover = true;
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

