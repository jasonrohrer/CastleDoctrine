#include "nameProcessing.h"

#include "minorGems/game/game.h"
#include "minorGems/util/stringUtils.h"




char *nameParse( char *inNameString ) {
    
    char found;
    
    char *name = replaceAll( inNameString, "_", " ", &found );
    
    delete [] inNameString;

    if( strcmp( name, "You" ) == 0 ) {
        delete [] name;
        
        name = stringDuplicate( translate( "nameYou" ) );
        }

    return name;
    }
