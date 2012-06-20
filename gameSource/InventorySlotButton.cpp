#include "InventorySlotButton.h"


#include "tools.h"



InventorySlotButton::InventorySlotButton( double inX, double inY,
                                          double inDrawScale )
        :
        // lie about sprite size so that button padding isn't so huge 
        SpriteButton( NULL, 9, 9, inX, inY, inDrawScale ),
        mObjectID( -1 ) {
    
    mOverrideHighlightColor = true;
    }



void InventorySlotButton::setObject( int inID ) {
    mObjectID = inID;
    
    if( mObjectID == -1 ) {
        setMouseOverTip( NULL );
        mSprite = NULL;
        }
    else {
        setMouseOverTip( getToolDescription( mObjectID ) );
        mSprite = getToolSprite( mObjectID );
        }
    }

        
int InventorySlotButton::getObject() {
    return mObjectID;
    }

