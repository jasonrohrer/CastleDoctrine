#include "RobPickList.h"


#include "ticketHash.h"

#include "minorGems/game/Font.h"
#include "minorGems/game/game.h"
#include "minorGems/game/drawUtils.h"

#include "minorGems/util/stringUtils.h"


extern Font *mainFont;

extern char *serverURL;

extern int userID;

extern double frameRateFactor;


static const int linesPerPage = 8;



RobPickList::RobPickList( double inX, double inY,
                          GamePage *inParentPage )
        : PageComponent( inX, inY ),
          mParentPage( inParentPage ),
          mCurrentSkip( 0 ),
          mWebRequest( -1 ),
          mProgressiveDrawSteps( 0 ),
          mUpButton( "up.tga", 5, 1, 1/16.0 ),
          mDownButton( "down.tga", 5, -1, 1/16.0 ) {

    mUpButton.setVisible( false );
    mDownButton.setVisible( false );
    
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

        mUpButton.setVisible( false );
        mDownButton.setVisible( false );

        refreshList();
        }
    else if( inTarget == &mDownButton ) {
        mCurrentSkip += linesPerPage;
        
        if( mCurrentSkip > 0 ) {
            mUpButton.setVisible( true );
            }

        mUpButton.setVisible( false );
        mDownButton.setVisible( false );

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



static double lineHeight = 0.75;
static double lineWidthLeft = 7;
static double lineWidthRight = 4;



void RobPickList::step() {
    if( mWebRequest != -1 ) {
            
        int stepResult = stepWebRequest( mWebRequest );
              
        mProgressiveDrawSteps = 0;
        
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
                            r.draw = false;
                            r.position.x = 0;
                            r.position.y = topOffset - i * lineHeight;
                            
                            sscanf( parts[0], "%d", &( r.userID ) );
                            
                            
                            char found;
                            r.characterName = 
                                replaceAll( parts[1], "_", " ", &found );
                            
                            // make sure name isn't too long for display
                            if( mainFont->measureString( r.characterName ) 
                                > lineWidthLeft ) {
                            
                                // try trimming middle name
                                int namePartCount;
                                char **nameParts = split( r.characterName, " ",
                                                          &namePartCount );
                                
                                // first letter only, followed by period
                                nameParts[1][1] = '.';
                                nameParts[1][2] = '\0';
                                
                                delete [] r.characterName;
                                
                                r.characterName = 
                                    join( nameParts, namePartCount, " " );
                                
                                if( mainFont->measureString( r.characterName ) 
                                    > lineWidthLeft ) {
                                    // still too long!
                                    
                                    // condense first name too.

                                    // first letter only, followed by period
                                    nameParts[0][1] = '.';
                                    nameParts[0][2] = '\0';
                                
                                    delete [] r.characterName;
                                    
                                    r.characterName = 
                                        join( nameParts, namePartCount, " " );
                                    }                                        

                                for( int j=0; j<namePartCount; j++ ) {
                                    delete [] nameParts[j];
                                    }
                                delete [] nameParts;
                                }
                            

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

    
    mProgressiveDrawSteps++;
    
    if( mProgressiveDrawSteps > 2 * frameRateFactor ) {
        mProgressiveDrawSteps = 0;

        // turn on one at a time
        char foundNew = false;
        for( int i=0; i<mHouseList.size() && !foundNew; i++ ) {
            HouseRecord *r = mHouseList.getElement( i );
            if( !r->draw ) {
                r->draw = true;
                foundNew = true;
                }
            }
        if( !foundNew ) {
            // all have been drawn now
            
            if( mHouseList.size() < linesPerPage ) {
                // on last page of list
                mDownButton.setVisible( false );
                }
            else {
                mDownButton.setVisible( true );
                }
            
            if( mCurrentSkip > 0 ) {
                mUpButton.setVisible( true );
                }
            else {
                mUpButton.setVisible( false );
                }
            }
        
        }
    
    }



void RobPickList::draw() {
    
    // alternate every other line
    char altColor = false;
    

    for( int i=0; i<mHouseList.size(); i++ ) {
        HouseRecord *r = mHouseList.getElement( i );
    

        if( r->draw ) {
            
            if( r->selected ) {
                setDrawColor( 0.35, 0.35, 0.15, 1 );
                }
            else {
                if( altColor ) {
                    setDrawColor( 0.2, 0.2, 0.2, 1 );
                    }
                else {
                    setDrawColor( 0.15, 0.15, 0.15, 1 );
                    }
                }

            drawRect( r->position.x - lineWidthLeft, 
                      r->position.y - lineHeight / 2, 
                      r->position.x + lineWidthRight, 
                      r->position.y + lineHeight / 2 );

            if( r->selected ) {
                setDrawColor( 1, 1, 0, 1 );
                }
            else {
                setDrawColor( 1, 1, 1, 1 );
                }
            
        
            mainFont->drawString( r->characterName, r->position, alignRight );

            char *lootString = autoSprintf( "$%d", r->lootValue );
        
            doublePair drawPos = r->position;
            drawPos.x += 1;
        
            mainFont->drawString( lootString, drawPos, alignLeft );

            delete [] lootString;

            altColor = ! altColor;
            }
        }
    }



void RobPickList::pointerUp( float inX, float inY ) {
    for( int i=0; i<mHouseList.size(); i++ ) {
        HouseRecord *r = mHouseList.getElement( i );
    
        if( inX > - lineWidthLeft && inX < lineWidthRight &&
            fabs( inY - r->position.y ) < lineHeight / 2 ) {
            // hit
            
            // unselect all
            for( int j=0; j<mHouseList.size(); j++ ) {
                HouseRecord *rB = mHouseList.getElement( j );
                rB->selected = false;
                }
            
            r->selected = true;
            return;
            }
        }
    }



void RobPickList::clearHouseList() {
    for( int i=0; i<mHouseList.size(); i++ ) {
        delete [] mHouseList.getElement(i)->characterName;
        }
    mHouseList.deleteAll();
    }

