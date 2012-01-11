#include "MenuPage.h"
#include "ticketHash.h"
#include "message.h"


#include "minorGems/game/Font.h"
#include "minorGems/game/game.h"
#include "minorGems/game/drawUtils.h"

#include "minorGems/util/stringUtils.h"


extern Font *mainFont;


extern char *serverURL;

extern int userID;



MenuPage::MenuPage() 
        : mWebRequest( -1 ),
          mEditHouseButton( mainFont, 4, -4, translate( "startEditHouse" ) ),
          mStartEditHouse( false ) {

    addComponent( &mEditHouseButton );
    mEditHouseButton.addActionListener( this );
    }


        
MenuPage::~MenuPage() {
    if( mWebRequest != -1 ) {
        clearWebRequest( mWebRequest );
        }

    clearHouseList();
    }



char MenuPage::getStartEditHouse() {
    return mStartEditHouse;
    }



void MenuPage::actionPerformed( GUIComponent *inTarget ) {
    if( inTarget == &mEditHouseButton ) {
        mStartEditHouse = true;
        }
    }



void MenuPage::step() {
    if( mWebRequest != -1 ) {
            
        int stepResult = stepWebRequest( mWebRequest );
                
        switch( stepResult ) {
            case 0:
                break;
            case -1:
                mStatusError = true;
                mStatusMessageKey = "err_webRequest";
                clearWebRequest( mWebRequest );
                mWebRequest = -1;
                break;
            case 1: {
                char *result = getWebResult( mWebRequest );
                clearWebRequest( mWebRequest );
                mWebRequest = -1;
                     
                printf( "Web result = %s\n", result );
   
                if( strstr( result, "DENIED" ) != NULL ) {
                    mStatusError = true;
                    mStatusMessageKey = "listFetchFailed";
                    }
                else {
                    // got house list!

                    mStatusError = false;
                    mStatusMessageKey = NULL;

                    // parse result
                    SimpleVector<char *> *lines = 
                        tokenizeString( result );
                    
                    for( int i=0; i<lines->size(); i++ ) {
                        char *line = *( lines->getElement( i ) );
                        
                        int numParts;
                        char **parts = split( line, "#", &numParts );
                        
                        if( numParts != 4 ) {
                            printf( "Unexpected number of parts on house "
                                    "list line: %d\n", numParts );
                            }
                        else {
                            HouseRecord r;
                            
                            r.selected = false;
                            r.position.x = 0;
                            r.position.y = i * 0.5;
                            
                            sscanf( parts[0], "%d", &( r.userID ) );
                            
                            
                            char found;
                            r.characterName = 
                                replaceAll( parts[1], "_", " ", &found );
                            
                            sscanf( parts[2], "%d", &( r.lootValue ) );
                            sscanf( parts[3], "%d", &( r.robAttempts ) );

                            mHouseList.push_back( r );
                            }
                        
                        for( int j=0; j<numParts; j++ ) {
                            delete [] parts[j];
                            }
                        delete [] parts;

                        
                        delete [] line;
                        }
                    delete lines;
                    }
                        
                        
                delete [] result;
                }
                break;
            }
        }
    }


        
void MenuPage::draw( doublePair inViewCenter, 
                          double inViewSize ) {


    for( int i=0; i<mHouseList.size(); i++ ) {
        HouseRecord *r = mHouseList.getElement( i );
    

        if( r->selected ) {
            setDrawColor( 0.25, 0.25, 0.25, 1 );
            drawSquare( r->position, 1 );
            }
        
        setDrawColor( 1, 1, 1, 1 );
        
        mainFont->drawString( r->characterName, r->position, alignRight );

        char *lootString = autoSprintf( "$%d", r->lootValue );
        
        doublePair drawPos = r->position;
        drawPos.x += 1;
        
        mainFont->drawString( lootString, drawPos, alignLeft );

        delete [] lootString;
        }
    }


        
void MenuPage::makeActive( char inFresh ) {
    if( !inFresh ) {
        return;
        }

    // request house list from server
    char *ticketHash = getTicketHash();
        
            
    
    char *actionString = autoSprintf( 
        "action=list_houses&user_id=%d"
        "&%s",
        userID, ticketHash );
    delete [] ticketHash;
            
    
    mWebRequest = startWebRequest( "POST", 
                                   serverURL, 
                                   actionString );
    
    delete [] actionString;

    mStartEditHouse = false;
    clearHouseList();


    mStatusMessageKey = NULL;
    mStatusError = false;
    }

        


void MenuPage::clearHouseList() {
    for( int i=0; i<mHouseList.size(); i++ ) {
        delete [] mHouseList.getElement(i)->characterName;
        }
    mHouseList.deleteAll();
    }


        

