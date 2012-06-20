

#include "SpriteButton.h"


class InventorySlotButton : public SpriteButton {
        

    public:

        InventorySlotButton( double inX, double inY, 
                             double inDrawScale = 1.0 );

        // if ID is -1, slot is cleared
        void setObject( int inID );
        
        int getObject();
        

    protected:
        
        int mObjectID;
        

    };

