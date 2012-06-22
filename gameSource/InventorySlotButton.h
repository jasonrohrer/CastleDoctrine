

#include "SpriteButton.h"


#include "minorGems/game/Font.h"



class InventorySlotButton : public SpriteButton {
        

    public:

        InventorySlotButton( Font *inDisplayFont,
                             double inX, double inY, 
                             double inDrawScale = 1.0 );

        // if ID is -1, slot is cleared
        void setObject( int inID );

        // set to 0 clears object ID
        void setQuantity( int inQuantity );
        
        int getQuantity();

        // -1 if empty
        int getObject();
        

    protected:
        Font *mFont;

        int mObjectID;
        
        int mQuantity;
        

        // override from SpriteButton to display quantity
        virtual void drawContents();
    };

