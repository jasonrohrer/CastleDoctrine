#include "minorGems/game/gameGraphics.h"



void initHouseObjects();


void freeHouseObjects();



#define MAX_ORIENTATIONS 16


int getNumOrientations( int inID, int inState );



// orientations based on binary interpretation of LBRT "neighbor present"
// flags
SpriteHandle getObjectSprite( int inID, int inOrientation, int inState );


