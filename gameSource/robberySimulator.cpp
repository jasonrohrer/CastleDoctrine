#include "ReplayRobHouseGridDisplay.h"
#include "inventory.h"

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
    
    if( tokens->size() != 8 ) {
        
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

    SimpleVector<QuantityRecord> backpackSlots;    
    fromString( *( tokens->getElement(2) ), &backpackSlots );
    
    
    replayCheckerDisplay->setMoveList( *( tokens->getElement(3) ) );

    int wifeMoney = 0;
    sscanf( *( tokens->getElement(4) ), "%d", &wifeMoney );
    
    replayCheckerDisplay->setWifeMoney( wifeMoney );
    
    replayCheckerDisplay->setWifeName( *( tokens->getElement(5) ) );
    replayCheckerDisplay->setSonName( *( tokens->getElement(6) ) );
    replayCheckerDisplay->setDaughterName( *( tokens->getElement(7) ) );



                          
    for( int i=0; i<tokens->size(); i++ ) {
        delete [] *( tokens->getElement( i ) );
        }
    delete tokens;
    

    replayCheckerDisplay->playAtFullSpeed();
    

    // FIXME:  need to check for move list running out without conclusion

    char moveListIncorrect = false;
    
    while( !replayCheckerDisplay->getDead() && 
           replayCheckerDisplay->getSuccess() == 0 ) {
        
        replayCheckerDisplay->step();


        if( replayCheckerDisplay->getToolJustUsed() ) {
            
            int pickedID = replayCheckerDisplay->getToolIDJustPicked();
            
            if( pickedID != -1 ) {
                char toolAllowed = subtractFromQuantity( &backpackSlots,
                                                         pickedID );

                if( !toolAllowed ) {
                    moveListIncorrect = true;
                    break;
                    }
                }
            }
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
    
    char *response;

    if( !moveListIncorrect ) {
        
        char *endBackpack = toString( &backpackSlots );


        char *endHouseMap = replayCheckerDisplay->getHouseMap();
        
        response = 
            autoSprintf( "%d\n"
                         "%d\n"
                         "%d\n"
                         "%d\n"
                         "%s\n"
                         "%s",
                         replayCheckerDisplay->getSuccess(),
                         replayCheckerDisplay->getWifeKilled(),
                         replayCheckerDisplay->getWifeRobbed(),
                         replayCheckerDisplay->getAnyFamilyKilled(),
                         endBackpack,
                         endHouseMap );


        delete [] endBackpack;
        delete [] endHouseMap;
        }
    else {
        response = stringDuplicate( "FAILED" );
        }

    return response;
    }

