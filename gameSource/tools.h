#ifndef TOOLS_INCLUDED
#define TOOLS_INCLUDED


#include "minorGems/game/gameGraphics.h"



void initTools();

void freeTools();


SpriteHandle getToolSprite( int inObjectID );

int getToolReach( int inObjectID );


// gets the internal name of a tool, example:  "saw"
const char *getToolName( int inObjectID );


// gets the human-readable description, example:  "Carpentry Saw"
const char *getToolDescription( int inObjectID  );


// returns -1 on failure
int getToolID( const char *inName );


#endif
