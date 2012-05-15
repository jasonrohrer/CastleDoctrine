#ifndef HOUSE_OBJECTS_INCLUDED
#define HOUSE_OBJECTS_INCLUDED


#include "minorGems/game/gameGraphics.h"



void initHouseObjects();


void freeHouseObjects();



// gets full list of IDs that have been loaded
// result destroyed by caller
int *getFullObjectIDList( int *outNumIDs );


// gets the internal name of an object, example:  "wall_wood"
const char *getObjectName( int inObjectID );


// gets the human-readable description, example:  "Wooden Wall"
const char *getObjectDescription( int inObjectID );


// returns -1 on failure
int getObjectID( const char *inName );




// macro trick that allows us to define this list only once and use
// it in both the enum and to create the string array.
// Ensures consistency of both lists.

// found here:  
//    http://www.gamedev.net/community/forums/topic.asp?topic_id=260159

#ifdef F
  #error Macro "F" already defined
#endif

#define PROPERTY_NAMES \
    F(permanent), \
    F(blocking), \
    F(visionBlocking), \
    F(shadowMaking), \
    F(structural), \
    F(deadly), \
    F(destroyed), \
    F(stuck), \
    F(powered), \
    F(conductive), \
    F(conductiveLeftToRight), \
    F(conductiveTopToBottom), \
    F(mobile), \
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

char isBehindSpritePresent( int inObjectID, int inState );


// orientations based on binary interpretation of LBRT "neighbor present"
// flags
SpriteHandle getObjectSprite( int inObjectID, int inOrientation, int inState );

SpriteHandle getObjectSpriteBehind( int inObjectID, int inOrientation, 
                                    int inState );



#endif

