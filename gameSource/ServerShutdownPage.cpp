#include "ServerShutdownPage.h"

#include "minorGems/game/Font.h"
#include "minorGems/game/game.h"

#include "minorGems/util/stringUtils.h"

#include "message.h"


extern Font *mainFont;



void ServerShutdownPage::makeActive( char inFresh ) {
    if( !inFresh ) {
        return;
        }
    
    setWaiting( false );
    }


void ServerShutdownPage::draw( doublePair inViewCenter, 
                              double inViewSize ) {

    doublePair labelPos = { 0, 1 };

    
    drawMessage( translate( "serverShutdownMessage" ), labelPos, false ); 
    }


