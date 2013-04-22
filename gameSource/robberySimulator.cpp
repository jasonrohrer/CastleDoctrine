#include "ReplayRobHouseGridDisplay.h"

#include "minorGems/util/stringUtils.h"

ReplayRobHouseGridDisplay *replayCheckerDisplay;



void initSimulator() {
    replayCheckerDisplay = new ReplayRobHouseGridDisplay( 0, 0 );
    }

void freeSimulator() {
    delete replayCheckerDisplay;
    }




char *simulateRobbery( const char *inRobberyData ) {

    SimpleVector<char *> *tokens = tokenizeString( inRobberyData );
    
    if( tokens->size() != 9  ) {
        
        for( int i=0; i<tokens->size(); i++ ) {
            delete [] *( tokens->getElement( i ) );
            }
        
        return stringDuplicate( "FAILED" );
        }

    
    /*
      === Call:
simulate_robbery
houseMap
backpack_contents
moveList
wife_money
wife_name
son_name
daughter_name
    */
    
    replayCheckerDisplay->setHouseMap( *( tokens->getElement(1) ) );

    char *backpack = stringDuplicate( *( tokens->getElement(2) ) );
    
    
    replayCheckerDisplay->setMoveList( *( tokens->getElement(3) ) );

    int wifeMoney = 0;
    sscanf( *( tokens->getElement(4) ), "%d", &wifeMoney );
    
    replayCheckerDisplay->setWifeMoney( wifeMoney );
    
    replayCheckerDisplay->setWifeName( *( tokens->getElement(5) ) );
    replayCheckerDisplay->setSonName( *( tokens->getElement(6) ) );
    replayCheckerDisplay->setDaughterName( *( tokens->getElement(7) ) );
        
    // FIXME:  parse backpack contents
                          
    for( int i=0; i<tokens->size(); i++ ) {
        delete [] *( tokens->getElement( i ) );
        }

    replayCheckerDisplay->playAtFullSpeed();
    

    while( !replayCheckerDisplay->getDead() && 
           replayCheckerDisplay->getSuccess() == 0 ) {
        
        replayCheckerDisplay->step();
        }
    
    
    /*
Returns:
success
wife_killed
wife_robbed
any_family_killed
end_backpack_contents
end_house_map
    */
    
    char *endHouseMap = replayCheckerDisplay->getHouseMap();
    
    char *response = autoSprintf( "%d\n"
                                  "%d\n"
                                  "%d\n"
                                  "%d\n"
                                  "%s\n"
                                  "%s\n",
                                  replayCheckerDisplay->getSuccess(),
                                  replayCheckerDisplay->getWifeKilled(),
                                  replayCheckerDisplay->getWifeRobbed(),
                                  replayCheckerDisplay->getAnyFamilyKilled(),
                                  backpack,
                                  endHouseMap );


    delete [] backpack;
    delete [] endHouseMap;

    return response;
    }

