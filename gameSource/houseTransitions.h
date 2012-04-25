#ifndef HOUSE_TRANSITIONS_INCLUDED
#define HOUSE_TRANSITIONS_INCLUDED



void initHouseTransitions();


void freeHouseTransitions();


// applies transition rules to transform inMapIDs and inMapStates
void applyTransitions( int *inMapIDs, int *inMapStates, 
                       int *inMapMobileIDs, int *inMapMobileStates,
                       int inMapW, int inMapH,
                       int inRobberIndex );



#endif
