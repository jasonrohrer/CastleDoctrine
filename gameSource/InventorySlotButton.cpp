#include "InventorySlotButton.h"


#include "tools.h"


#include "minorGems/util/stringUtils.h"

#include "minorGems/game/drawUtils.h"
#include "minorGems/game/gameGraphics.h"



InventorySlotButton::InventorySlotButton( Font *inDisplayFont,
                                          double inX, double inY,
                                          double inDrawScale )
        :
        // lie about sprite size so that button padding isn't so huge 
        SpriteButton( NULL, 9, 9, inX, inY, inDrawScale ),
        mFont( inDisplayFont ),
        mObjectID( -1 ),
        mQuantity( 0 ),
        mRingOn( false ) {
    
    mOverrideHighlightColor = true;
    }



void InventorySlotButton::setObject( int inID ) {
    mObjectID = inID;
    
    if( mObjectID == -1 ) {
        setMouseOverTip( NULL );
        mSprite = NULL;
        mQuantity = 0;
        }
    else {
        setMouseOverTip( getToolDescription( mObjectID ) );
        mSprite = getToolSprite( mObjectID );
        mQuantity = 1;
        }
    }


        
int InventorySlotButton::getObject() {
    return mObjectID;
    }



void InventorySlotButton::setQuantity( int inQuantity ) {
    mQuantity = inQuantity;
    
    if( mQuantity == 0 ) {
        setObject( -1 );
        }
    }


        
int InventorySlotButton::getQuantity() {
    return mQuantity;
    }




void InventorySlotButton::drawContents() {
    SpriteButton::drawContents();
    
    if( mQuantity > 1 ) {
        
        doublePair textPos = { 0.5 - mDrawScale, -0.5 };
        
        char *text = autoSprintf( "%d", mQuantity );
        
        setDrawColor( 1, 1, 0, 1 );

        mFont->drawString( text, textPos, alignRight );
    
        delete [] text;
        }
    
    }




void InventorySlotButton::setRingOn( char inRingOn ) {
    mRingOn = inRingOn;
    }





void InventorySlotButton::draw() {

    if( mRingOn ) {

        // ring
        setDrawColor( 0.75, 0.75, 0, 1 );
        
        double fullWide = mWide + 4 * mPixWidth;
        double fullHigh = mHigh + 4 * mPixWidth;

        drawRect( - fullWide / 2, - fullHigh / 2, 
                  fullWide / 2, fullHigh / 2 );


        // background inside ring
        setDrawColor( 0, 0, 0, 1 );
        
        fullWide -= 2 * mPixWidth;
        fullHigh -= 2 * mPixWidth;
        
        drawRect( - fullWide / 2, - fullHigh / 2, 
                  fullWide / 2, fullHigh / 2 );
        }
    

    // draw button's contents on top
    SpriteButton::draw();
    }
