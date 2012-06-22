#include "InventorySlotButton.h"


#include "tools.h"


#include "minorGems/util/stringUtils.h"


InventorySlotButton::InventorySlotButton( Font *inDisplayFont,
                                          double inX, double inY,
                                          double inDrawScale )
        :
        // lie about sprite size so that button padding isn't so huge 
        SpriteButton( NULL, 9, 9, inX, inY, inDrawScale ),
        mFont( inDisplayFont ),
        mObjectID( -1 ),
        mQuantity( 0 ) {
    
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

