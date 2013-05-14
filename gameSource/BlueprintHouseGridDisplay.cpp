#include "BlueprintHouseGridDisplay.h"



BlueprintHouseGridDisplay::BlueprintHouseGridDisplay( double inX, 
                                                        double inY )
        : HouseGridDisplay( inX, inY ) {
    }


        
void BlueprintHouseGridDisplay::setHouseMap( const char *inHouseMap ) {
    HouseGridDisplay::setHouseMap( inHouseMap );
    
    // switch all to blueprint state
    resetToggledStates( 100 );
    }



void BlueprintHouseGridDisplay::specialKeyDown( int inKeyCode ) {
    // FIXME: pan around
    
    }


void BlueprintHouseGridDisplay::specialKeyUp( int inKeyCode ) {
    // FIXME: pan around

    }
