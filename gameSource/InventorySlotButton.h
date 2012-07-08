#ifndef INVENTORY_SLOT_BUTTON_INCLUDED
#define INVENTORY_SLOT_BUTTON_INCLUDED


#include "SpriteButton.h"


#include "minorGems/game/Font.h"



class InventorySlotButton : public SpriteButton {
        

    public:

        InventorySlotButton( Font *inDisplayFont,
                             double inX, double inY, 
                             double inDrawScale = 1.0 );

        // if ID is -1, slot is cleared
        virtual void setObject( int inID );

        // set to 0 clears object ID
        void setQuantity( int inQuantity );
        
        int getQuantity();

        // -1 if empty
        int getObject();
        

        void setRingOn( char inRingOn );
        
        char getRingOn() {
            return mRingOn;
            }
        

    protected:
        Font *mFont;

        int mObjectID;
        
        int mQuantity;
        

        // override from SpriteButton to display quantity
        virtual void drawContents();

        char mRingOn;
        
        // override this from Button to draw ring
        virtual void draw();
    };



#endif
