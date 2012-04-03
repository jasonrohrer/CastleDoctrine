#include "minorGems/game/gameGraphics.h"



void initHouseObjects();


void freeHouseObjects();




// macro trick that allows us to define this list only once and use
// it in both the enum and to create the string array.
// Ensures consistency of both lists.

// found here:  
//    http://www.gamedev.net/community/forums/topic.asp?topic_id=260159

#ifdef F
  #error Macro "F" already defined
#endif

#define PROPERTY_NAMES \
    F(blocking), \
    F(deadly), \
    F(destroyed), \
    F(powered), \
    F(conductive), \
    F(playerSeeking) \

                



// first, let F simply resolve to the raw name
// thus, the above list expands into the body of the enum
#define F(inName) inName

enum propertyID {
	PROPERTY_NAMES,
    endPropertyID
    };



char isPropertySet( int inObjectID, int inState, propertyID inProperty );







#define MAX_ORIENTATIONS 16


int getNumOrientations( int inObjectID, int inState );



// orientations based on binary interpretation of LBRT "neighbor present"
// flags
SpriteHandle getObjectSprite( int inObjectID, int inOrientation, int inState );


