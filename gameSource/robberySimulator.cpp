

#include "ReplayRobHouseGridDisplay.h"

ReplayRobHouseGridDisplay *replayCheckerDisplay;



void initSimulator() {
    replayCheckerDisplay = new ReplayRobHouseGridDisplay( 0, 0 );
    }

void freeSimulator() {
    delete replayCheckerDisplay;
    }




char *simulateRobbery( const char *inRobberyData ) {


    for( int i=0; i<100; i++ ) {
        printf( "Stepping checker %d\n", i );
        replayCheckerDisplay->step();
        }

    }

