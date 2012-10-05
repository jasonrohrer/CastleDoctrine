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
const char *getObjectDescription( int inObjectID,
                                  int inState );


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
    F(stuck), \
    F(powered), \
    F(conductive), \
    F(conductiveLeftToRight), \
    F(conductiveTopToBottom), \
    F(mobile), \
    F(mobileBlocking), \
    F(playerSeeking), \
    F(onTopOfPlayer) \

                



// first, let F simply resolve to the raw name
// thus, the above list expands into the body of the enum
#define F(inName) inName

enum propertyID {
	PROPERTY_NAMES,
    endPropertyID
    };



char isPropertySet( int inObjectID, int inState, propertyID inProperty );



// tests if inOtherObjectID is in inObjectID's group
char isInGroup( int inObjectID, int inOtherObjectID );





#define MAX_ORIENTATIONS 16


int getNumOrientations( int inObjectID, int inState );

char isBehindSpritePresent( int inObjectID, int inState );
char isUnderSpritePresent( int inObjectID, int inState );


// orientations based on binary interpretation of LBRT "neighbor present"
// flags
SpriteHandle getObjectSprite( int inObjectID, int inOrientation, int inState );

// drawn behind mobile objects
SpriteHandle getObjectSpriteBehind( int inObjectID, int inOrientation, 
                                    int inState );
// drawn under shadow layer
SpriteHandle getObjectSpriteUnder( int inObjectID, int inOrientation, 
                                   int inState );




// utility function that other systems can use

// reads a TGA sprite and shade map, applies the shade map, and
// creates a sprite for each orientation square in the file

// inTgaPath and inShadeMapTgaPath are deleted if not NULL
// returns number of orientaitons
int readShadeMappedSprites( char *inTgaPath, char *inShadeMapTgaPath,
                            SpriteHandle *inSpriteOrientationArray );



#endif

