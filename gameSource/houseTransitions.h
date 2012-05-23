#ifndef HOUSE_TRANSITIONS_INCLUDED
#define HOUSE_TRANSITIONS_INCLUDED



void initHouseTransitions();


void freeHouseTransitions();


// when mobile objects are frozen, they don't move during applyTransitions
// and their presence on a tile does not trigger transitions for that tile
void freezeMobileObjects( char inFreeze );



// applies transition rules to transform inMapIDs and inMapStates
void applyTransitions( int *inMapIDs, int *inMapStates, 
                       int *inMapMobileIDs, int *inMapMobileStates,
                       int inMapW, int inMapH,
                       int inRobberIndex );



// returns new state for inTarget
int checkTransition( int inTargetID, int inTargetState,
                     int inTriggerID, int inTriggerState );



#endif
