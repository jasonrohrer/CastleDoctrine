#include "GallerySlotButton.h"

#include "galleryObjects.h"


GallerySlotButton::GallerySlotButton( Font *inDisplayFont,
                                      SpriteHandle inFrameSprite,
                                      double inX, double inY, 
                                      double inDrawScale ) 
        : InventorySlotButton( inDisplayFont, inX, inY, inDrawScale  ),
          mFrameSprite( inFrameSprite ) {
    
    // room for border around gallery object sprite (they're all 32x32)
    mWide = inDrawScale * 34;
    mHigh = inDrawScale * 34;
    }



void GallerySlotButton::setObject( int inID ) {
    if( inID == -1 ) {
        InventorySlotButton::setObject( inID );
        }
    else {
        mObjectID = inID;
        
        setMouseOverTip( getGalleryObjectDescription( mObjectID ) );
        mSprite = getGalleryObjectSprite( mObjectID );
        mQuantity = 1;
        }
    }


void GallerySlotButton::drawBorder() {

    doublePair center = { 0, 0 };
        
    drawSprite( mFrameSprite, center, mDrawScale * 2 );
    }


