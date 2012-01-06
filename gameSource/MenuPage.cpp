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
          mStatusError( false ),
          mStatusMessageKey( NULL ),
          mEditHouseButton( mainFont, 4, -4, translate( "startEditHouse" ) ),
          mStartEditHouse( false ) {
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




void MenuPage::step() {
    if( mWebRequest != -1 ) {
            
        int stepResult = stepWebRequest( mWebRequest );

        printf( "StepResult %d\n", stepResult );
                
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

    mEditHouseButton.draw();

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
    

    
    
    if( mStatusMessageKey != NULL ) {
        doublePair labelPos = { 0, -5 };

        drawMessage( mStatusMessageKey, labelPos, mStatusError );
        }

    }


        
void MenuPage::makeActive() {

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
    }



void MenuPage::makeNotActive() {
    }
        


void MenuPage::clearHouseList() {
    for( int i=0; i<mHouseList.size(); i++ ) {
        delete [] mHouseList.getElement(i)->characterName;
        }
    mHouseList.deleteAll();
    }


        
void MenuPage::pointerMove( float inX, float inY ) {
    mEditHouseButton.pointerMove( inX, inY );
    }



void MenuPage::pointerDown( float inX, float inY ) {
    mEditHouseButton.pointerDown( inX, inY );
    }



void MenuPage::pointerDrag( float inX, float inY ) {
    mEditHouseButton.pointerDrag( inX, inY );
    }



void MenuPage::pointerUp( float inX, float inY ) {
    if( mEditHouseButton.pointerUp( inX, inY ) ) {
        mStartEditHouse = true;
        }
    }

