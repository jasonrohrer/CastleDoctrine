#include "balance.h"
#include "message.h"

#include "minorGems/game/doublePair.h"
#include "minorGems/game/Font.h"
#include "minorGems/game/game.h"

#include "minorGems/util/stringUtils.h"


extern Font *mainFont;
extern Font *numbersFontFixed;


void drawBalance( int inLoot, int inChangeCost ) {
    doublePair labelPos = { 9, 2 };
    
    

    //drawMessage( "editBalance", labelPos, false );
    
    mainFont->drawString( translate( "editBalance" ), 
                          labelPos, alignRight );

    labelPos.y = 1.25;
    labelPos.x -= 0.125;
    char *balanceMessage = autoSprintf( "$%d", inLoot );

    numbersFontFixed->drawString( balanceMessage, 
                                  labelPos, alignRight );

    //drawMessage( balanceMessage, labelPos, false );
    
    delete [] balanceMessage;



    if( inChangeCost >  0 ) {
        
        char costRed = ( inChangeCost > inLoot );

        //    labelPos.y = -6;
        
        //drawMessage( "editCost", labelPos, costRed );
        
        labelPos.y = 0.5;
        
        char *costMessage = autoSprintf( "-$%d", inChangeCost );
        
        if( !costRed ) {
            setDrawColor( 0, 0.75, 0, 1 );
            }
        
        numbersFontFixed->drawString( costMessage, 
                                      labelPos, alignRight );

        //drawMessage( costMessage, labelPos, costRed, 1, true );
        
        delete [] costMessage;
        }

    }

