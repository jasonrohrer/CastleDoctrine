#ifndef GALLERY_SLOT_BUTTON_INCLUDED
#define GALLERY_SLOT_BUTTON_INCLUDED


#include "InventorySlotButton.h"


#include "minorGems/game/Font.h"



class GallerySlotButton : public InventorySlotButton {
        

    public:

        GallerySlotButton( Font *inDisplayFont,
                           SpriteHandle inFrameSprite,
                           double inX, double inY, 
                           double inDrawScale = 1.0 );

        // override from InventorySlotButton to handle galleryObjects
        // instead of tools
        virtual void setObject( int inID );
        
    protected:
        
        // override to draw a frame
        virtual void drawBorder();

        SpriteHandle mFrameSprite;
    };



#endif
