#include "RobPickList.h"


#include "ticketHash.h"

#include "minorGems/game/Font.h"
#include "minorGems/game/game.h"
#include "minorGems/game/drawUtils.h"

#include "minorGems/util/stringUtils.h"


extern Font *mainFont;

extern char *serverURL;

extern int userID;


static const int linesPerPage = 8;



RobPickList::RobPickList( double inX, double inY,
                          GamePage *inParentPage )
        : PageComponent( inX, inY ),
          mParentPage( inParentPage ),
          mCurrentSkip( 0 ),
          mWebRequest( -1 ),
          mUpButton( "up.tga", 5, 1, 1/16.0 ),
          mDownButton( "down.tga", 5, -1, 1/16.0 ) {

    mUpButton.setVisible( false );
    
    addComponent( &mUpButton );
    addComponent( &mDownButton );

    mUpButton.addActionListener( this );
    mDownButton.addActionListener( this );
    }



RobPickList::~RobPickList() {
    if( mWebRequest != -1 ) {
        clearWebRequest( mWebRequest );
        }
    clearHouseList();
    }



void RobPickList::actionPerformed( GUIComponent *inTarget ) {
    if( inTarget == &mUpButton ) {
        mCurrentSkip -= linesPerPage;
        if( mCurrentSkip < 0 ) {
            mCurrentSkip = 0;
            }

        if( mCurrentSkip == 0 ) {
            mUpButton.setVisible( false );
            }
        refreshList();
        }
    else if( inTarget == &mDownButton ) {
        mCurrentSkip += linesPerPage;
        
        if( mCurrentSkip > 0 ) {
            mUpButton.setVisible( true );
            }
        refreshList();
        }
    }



// fetch new results from server
void RobPickList::refreshList() {
    if( mWebRequest != -1 ) {
        clearWebRequest( mWebRequest );
        }
    clearHouseList();
    

    // request house list from server
    char *ticketHash = getTicketHash();
        
            
    
    char *actionString = autoSprintf( 
        "action=list_houses&skip=%d&limit=%d&user_id=%d"
        "&%s",
        mCurrentSkip, linesPerPage, userID, ticketHash );
    delete [] ticketHash;
            
    
    mWebRequest = startWebRequest( "POST", 
                                   serverURL, 
                                   actionString );
    
    delete [] actionString;
    }



void RobPickList::step() {
    if( mWebRequest != -1 ) {
            
        int stepResult = stepWebRequest( mWebRequest );
                
        switch( stepResult ) {
            case 0:
                break;
            case -1:
                mParentPage->setStatus( "err_webRequest", true );
                clearWebRequest( mWebRequest );
                mWebRequest = -1;
                break;
            case 1: {
                char *result = getWebResult( mWebRequest );
                clearWebRequest( mWebRequest );
                mWebRequest = -1;
                     
                printf( "Web result = %s\n", result );
   
                if( strstr( result, "DENIED" ) != NULL ) {
                    mParentPage->setStatus( "listFetchFailed", true );
                    }
                else {
                    // got house list!

                    mParentPage->setStatus( NULL, false );

                    // parse result
                    SimpleVector<char *> *lines = 
                        tokenizeString( result );

                    double lineHeight = 0.5;

                    double topOffset = ( linesPerPage * lineHeight ) / 2 
                        - lineHeight / 2;

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
                            r.position.y = topOffset - i * lineHeight;
                            
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

                    if( lines->size() < linesPerPage ) {
                        // on last page of list
                        mDownButton.setVisible( false );
                        }
                    else {
                        mDownButton.setVisible( true );
                        }                    

                    delete lines;
                    }
                        
                        
                delete [] result;
                }
                break;
            }
        }
    }



void RobPickList::draw() {
    

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



void RobPickList::pointerUp( float inX, float inY ) {
    }



void RobPickList::clearHouseList() {
    for( int i=0; i<mHouseList.size(); i++ ) {
        delete [] mHouseList.getElement(i)->characterName;
        }
    mHouseList.deleteAll();
    }

