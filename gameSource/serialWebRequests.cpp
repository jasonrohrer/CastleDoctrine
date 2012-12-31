
#include "serialWebRequests.h"

#include "minorGems/game/game.h"




int startWebRequestSerial( const char *inMethod, const char *inURL,
                           const char *inBody ) {
    
    return startWebRequest( inMethod, inURL, inBody );
    }


int stepWebRequestSerial( int inHandle ) {
    return stepWebRequest( inHandle );
    }


char *getWebResultSerial( int inHandle ) {
    return getWebResult( inHandle );
    }



void clearWebRequestSerial( int inHandle ) {
    clearWebRequest( inHandle );
    }
