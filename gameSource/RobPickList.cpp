#include "RobPickList.h"


#include "ticketHash.h"

#include "minorGems/game/Font.h"
#include "minorGems/game/game.h"
#include "minorGems/game/drawUtils.h"

#include "minorGems/util/stringUtils.h"


extern Font *mainFont;
extern Font *mainFontFixed;

extern char *serverURL;

extern int userID;

extern double frameRateFactor;


static const int linesPerPage = 8;
static double lineHeight = 0.75;
static double lineWidthLeft = 7;
static double lineWidthRight = 7;


static double topOffset = ( linesPerPage * lineHeight ) / 2 - lineHeight / 2;



RobPickList::RobPickList( double inX, double inY,
                          char inRobberyLog,
                          GamePage *inParentPage )
        : PageComponent( inX, inY ),
          mParentPage( inParentPage ),
          mCurrentSkip( 0 ),
          mWebRequest( -1 ),
          mProgressiveDrawSteps( 0 ),
          mRobberyLog( inRobberyLog ),
          mUpButton( "up.tga", 8, 1, 1/16.0 ),
          mDownButton( "down.tga", 8, -1, 1/16.0 ),
          mSearchField( mainFontFixed, mainFont, 
                        0, 4,
                        8,
                        false,
                        translate( "nameSearch" ),
                        "abcdefghijklmnopqrstuvwxyz"
                        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"),
          mFilterButton( mainFont, 4, 4,
                         translate( "filter" ) ),
          mAppliedSearchWord( stringDuplicate( "" ) ) {

    mUpButton.setVisible( false );
    mDownButton.setVisible( false );
    
    addComponent( &mUpButton );
    addComponent( &mDownButton );
    addComponent( &mSearchField );
    addComponent( &mFilterButton );
    
    mUpButton.addActionListener( this );
    mDownButton.addActionListener( this );

    mSearchField.addActionListener( this );
    mFilterButton.addActionListener( this );
    }



RobPickList::~RobPickList() {
    if( mWebRequest != -1 ) {
        clearWebRequest( mWebRequest );
        }
    clearHouseList();

    delete [] mAppliedSearchWord;
    }



void RobPickList::actionPerformed( GUIComponent *inTarget ) {
    if( inTarget == &mUpButton ) {
        mCurrentSkip -= linesPerPage;
        if( mCurrentSkip < 0 ) {
            mCurrentSkip = 0;
            }

        refreshList( true, true );
        }
    else if( inTarget == &mDownButton ) {
        mCurrentSkip += linesPerPage;

        refreshList( true, true );
        }
    else if( inTarget == &mFilterButton || inTarget == &mSearchField ) {
        delete [] mAppliedSearchWord;

        mAppliedSearchWord = mSearchField.getText();

        refreshList( true, false );
        }
    }



// fetch new results from server
void RobPickList::refreshList( char inPreserveSearch,
                               char inPreservePosition ) {
    if( mWebRequest != -1 ) {
        clearWebRequest( mWebRequest );
        }
    clearHouseList();

    mUpButton.setVisible( false );
    mDownButton.setVisible( false );

    mSearchField.focus();

    if( ! inPreserveSearch ) {
        delete [] mAppliedSearchWord;
        mAppliedSearchWord = stringDuplicate( "" );
        mSearchField.setText( mAppliedSearchWord );
        }
    
    if( ! inPreservePosition ) {
        mCurrentSkip = 0;
        }


    // request house list from server
    char *ticketHash = getTicketHash();
        
    const char *action = "list_houses";
    
    if( mRobberyLog ) {
        action = "list_logged_robberies";
        }
        
    char *actionString = autoSprintf( 
        "action=%s&skip=%d&limit=%d&name_search=%s&user_id=%d"
        "&%s",
        action, mCurrentSkip, linesPerPage, mAppliedSearchWord, 
        userID, ticketHash );
    delete [] ticketHash;
            
    
    mWebRequest = startWebRequest( "POST", 
                                   serverURL, 
                                   actionString );
    
    printf( "Request = %s\n", actionString );
    
    delete [] actionString;

    // nothing selected now (so selection might have changed)
    fireActionPerformed( this );
    }



HouseRecord *RobPickList::getSelectedHouse() {
    for( int i=0; i<mHouseList.size(); i++ ) {
        HouseRecord *r = mHouseList.getElement( i );
        
        if( r->selected ) {
            return r;
            }
        }

    return NULL;
    }







// inName destroyed internally
// result destroyed by caller
static char *trimName( char *inName, double inMaxLength ) {
    
    // make sure name isn't too long for display
    if( mainFont->measureString( inName ) 
        > inMaxLength ) {
                            
        // try trimming middle name
        int namePartCount;
        char **nameParts = split( inName, " ",
                                  &namePartCount );
                                
        // first letter only, followed by period
        nameParts[1][1] = '.';
        nameParts[1][2] = '\0';
                                
        delete [] inName;
                                
        inName = 
            join( nameParts, namePartCount, " " );
                                
        if( mainFont->measureString( inName ) 
            > inMaxLength ) {
            // still too long!
                                    
            // condense first name too.

            // first letter only, followed by period
            nameParts[0][1] = '.';
            nameParts[0][2] = '\0';
                                
            delete [] inName;
                                    
            inName = 
                join( nameParts, namePartCount, " " );
            }                                        

        for( int j=0; j<namePartCount; j++ ) {
            delete [] nameParts[j];
            }
        delete [] nameParts;
        }

    if( strcmp( inName, "You" ) == 0 ) {
        delete [] inName;
        
        inName = stringDuplicate( translate( "nameYou" ) );
        }
    

    return inName;
    }



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



                    char badParse = false;

                    // last line should be OK
                    for( int i=0; i<lines->size() - 1; i++ ) {
                        char *line = *( lines->getElement( i ) );
                        
                        int numParts;
                        char **parts = split( line, "#", &numParts );
                        
                        if( numParts != 6 ) {
                            printf( "Unexpected number of parts (%d) on house "
                                    "list line: %d\n", numParts, i );
                            badParse = true;
                            }
                        else {
                            HouseRecord r;
                            
                            r.selected = false;
                            r.draw = false;
                            r.position.x = 0;
                            r.position.y = topOffset - i * lineHeight;
                            
                            sscanf( parts[0], "%d", &( r.uniqueID ) );
                            
                            
                            char found;
                            
                            r.rawCharacterName = stringDuplicate( parts[1] );
                            
                            r.characterName = 
                                trimName(
                                    replaceAll( parts[1], "_", " ", &found ),
                                    lineWidthLeft - 0.125 );
                            
                            r.lastRobberName = 
                                trimName (
                                    replaceAll( parts[2], "_", " ", &found ),
                                    lineWidthLeft - 0.125 );
                            

                            sscanf( parts[3], "%d", &( r.lootValue ) );
                            sscanf( parts[4], "%d", &( r.robAttempts ) );
                            sscanf( parts[5], "%d", &( r.robberDeaths ) );

                            mHouseList.push_back( r );
                            }
                        
                        for( int j=0; j<numParts; j++ ) {
                            delete [] parts[j];
                            }
                        delete [] parts;

                        
                        delete [] line;
                        }
                    
                    if( lines->size() > 0 ) {
                        
                        char *line = 
                            *( lines->getElement( lines->size() -1  ) );
                    
                        if( strcmp( line, "OK" ) != 0 ) {
                            badParse = true;
                            }
                        
                        delete [] line;
                        }
                    else {
                        // list could be empty, but it still must have OK
                        // at the end
                        badParse = true;
                        }
                    
                    

                    delete lines;

                    
                    if( badParse ) {
                        clearHouseList();
                        mParentPage->setStatus( "listFetchFailed", true );
                        }
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
            
            char *nameToDraw = r->characterName;
            
        
            if( mRobberyLog ) {
                nameToDraw = r->lastRobberName;
                }
            
            mainFont->drawString( nameToDraw, r->position, alignRight );
            

            char *lootString = autoSprintf( "$%d", r->lootValue );
        
            doublePair drawPos = r->position;
            drawPos.x += 1;
        
            mainFont->drawString( lootString, drawPos, alignLeft );

            delete [] lootString;


            char *attemptString = autoSprintf( "%d", r->robAttempts );
        
            drawPos = r->position;
            drawPos.x += 5;
        
            mainFont->drawString( attemptString, drawPos, alignRight );

            delete [] attemptString;

            
            setDrawColor( 1, 0, 0, 1 );
            char *deathsString = autoSprintf( "%d", r->robberDeaths );
        
            drawPos = r->position;
            drawPos.x += 6.5;
        
            mainFont->drawString( deathsString, drawPos, alignRight );

            delete [] deathsString;


            altColor = ! altColor;
            }
        }

    if( mHouseList.size() == 0 && mWebRequest == -1 ) {
        // request done, and list still empty
        
        // empty list back from server.

        setDrawColor( 1, 1, 1, 1 );

        doublePair drawPos = { 0, 0 };
        
        mainFont->drawString( translate( "emptyList" ), drawPos, alignCenter );
        }
    
        
    }




HouseRecord *RobPickList::getHitRecord( float inX, float inY ) {
    for( int i=0; i<mHouseList.size(); i++ ) {
        HouseRecord *r = mHouseList.getElement( i );
    
        if( inX > - lineWidthLeft && inX < lineWidthRight &&
            fabs( inY - r->position.y ) < lineHeight / 2 ) {
            // hit

            return r;
            }
        }
    
    return NULL;
    }



// inName NOT destroyed
// result destroyed by caller
static char *convertToLastName( const char *inName ) {
    
    char *convertedName;
            
    int partCount;
    char **parts = split( inName, " ",
                          &partCount );
            
    if( partCount != 3 ) {
        // default to using full name
        convertedName = stringDuplicate( inName );
        }
    else {
        convertedName = stringDuplicate( parts[2] );
        }
    for( int i=0; i<partCount; i++ ) {
        delete [] parts[i];
        }
    delete [] parts;
    
    return convertedName;
    }



void RobPickList::setTip( HouseRecord *inRecord ) {
    char *tip;
        
    if( mRobberyLog ) {
        char *lastName = convertToLastName( inRecord->lastRobberName );

        tip = autoSprintf( translate( "replayPickListTip" ), 
                           lastName, inRecord->lootValue, 
                           inRecord->robAttempts, inRecord->robberDeaths );
        delete [] lastName;
        }
    else {
        // transform into possessive form of last name only
            
        char *lastName = convertToLastName( inRecord->characterName );
            
        char *possessiveName;
            
        // add possessive to end
        if( lastName[ strlen( lastName ) - 1 ] == 's' ) {
            possessiveName = autoSprintf( "%s'", lastName );
            }
        else {
            possessiveName = autoSprintf( "%s's", lastName );
            }
            
        delete [] lastName;
            
            

        tip = autoSprintf( translate( "robPickListTip" ), 
                           inRecord->robAttempts, 
                           possessiveName, 
                           inRecord->robberDeaths );
        delete [] possessiveName;
        }
        
    setToolTipDirect( tip );

    delete [] tip;
    }




void RobPickList::pointerUp( float inX, float inY ) {
    HouseRecord *r = getHitRecord( inX, inY );
    
    if( r != NULL ) {
        
        // unselect all
        for( int j=0; j<mHouseList.size(); j++ ) {
            HouseRecord *rB = mHouseList.getElement( j );
            rB->selected = false;
            }
            
        r->selected = true;
        setTip( r );

        fireActionPerformed( this );
        return;
        }
    }





void RobPickList::pointerMove( float inX, float inY ) {
    HouseRecord *r = getHitRecord( inX, inY );
    
    if( r != NULL ) {
        setTip( r );
        }
    }


void RobPickList::pointerDrag( float inX, float inY ) {
    pointerUp( inX, inY );
    }


void RobPickList::pointerDown( float inX, float inY ) {
    pointerUp( inX, inY );
    }




void RobPickList::clearHouseList() {
    for( int i=0; i<mHouseList.size(); i++ ) {
        delete [] mHouseList.getElement(i)->characterName;
        delete [] mHouseList.getElement(i)->rawCharacterName;
        delete [] mHouseList.getElement(i)->lastRobberName;
        }
    mHouseList.deleteAll();
    }

