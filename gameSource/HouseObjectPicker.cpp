#include "HouseObjectPicker.h"


#include "ticketHash.h"
#include "houseObjects.h"

#include "minorGems/game/Font.h"
#include "minorGems/game/game.h"
#include "minorGems/game/drawUtils.h"

#include "minorGems/util/stringUtils.h"


extern Font *mainFont;

extern char *serverURL;

extern int userID;

extern double frameRateFactor;



// these cannot be manually placed by user
#define BLOCK_LIST_SIZE 4

static const char *blockList[BLOCK_LIST_SIZE] = 
{ "floor",
  "start",
  "vault_goal",
  "wall_exterior" 
    };


                                    

HouseObjectPicker::HouseObjectPicker( double inX, double inY,
                                      GamePage *inParentPage )
        : PageComponent( inX, inY ),
          mParentPage( inParentPage ),
          mWebRequest( -1 ),
          mSelectedIndex( -1 ),
          mPixWidth( 1/16.0 ),
          mUpButton( "up.tga", -1.25, 1, mPixWidth ),
          mDownButton( "down.tga", -1.25, -1, mPixWidth ) {

    mUpButton.setVisible( false );
    mDownButton.setVisible( false );
    
    addComponent( &mUpButton );
    addComponent( &mDownButton );

    mUpButton.addActionListener( this );
    mDownButton.addActionListener( this );

    
    int numObjects;
    int *idList = getFullObjectIDList( &numObjects );
    
    for( int i=0; i<numObjects; i++ ) {
        ObjectPriceRecord r;
        r.id = idList[i];
        r.price = 10;
        
        const char *name = getObjectName( r.id );
        
        char blocked = false;
        for( int b=0; b<BLOCK_LIST_SIZE; b++ ) {
            if( strcmp( name, blockList[b] ) == 0 ) {
                blocked = true;
                break;
                }
            }
        
        if( !blocked ) {
            mObjectList.push_back( r );
            }
        }
    delete [] idList;
    
    if( mObjectList.size() > 0 ) {
        mSelectedIndex = 0;
        mUpButton.setVisible( true );
        mDownButton.setVisible( true );
        }
    
    }



HouseObjectPicker::~HouseObjectPicker() {
    if( mWebRequest != -1 ) {
        clearWebRequest( mWebRequest );
        }
    }



void HouseObjectPicker::actionPerformed( GUIComponent *inTarget ) {
    char change = false;
    
    if( inTarget == &mUpButton ) {
        mSelectedIndex --;
        if( mSelectedIndex < 0 ) {
            mSelectedIndex = mObjectList.size() - 1;
            }
        change = true;
        }
    else if( inTarget == &mDownButton ) {
        mSelectedIndex ++;
        if( mSelectedIndex > mObjectList.size() - 1 ) {
            mSelectedIndex = 0;
            }
        change = true;    
        }

    
    if( change ) {
        
        mParentPage->setToolTipDirect( 
            (char *)getObjectDescription( 
                mObjectList.getElement( mSelectedIndex )->id ) );
        
        fireActionPerformed( this );
        }
    
    }



// fetch new results from server
void HouseObjectPicker::refreshPrices() {
    if( mWebRequest != -1 ) {
        clearWebRequest( mWebRequest );
        }

    mObjectList.deleteAll();
    mSelectedIndex = -1;
    
    mUpButton.setVisible( false );
    mDownButton.setVisible( false );
    
    // FIXME
    if( true ) {
        return;
        }
    /*

    // request house list from server
    char *ticketHash = getTicketHash();
        
    const char *action = "list_houses";
    
    if( mRobberyLog ) {
        action = "list_logged_robberies";
        }
    
    
    char *actionString = autoSprintf( 
        "action=%s&skip=%d&limit=%d&user_id=%d"
        "&%s",
        action, mCurrentSkip, linesPerPage, userID, ticketHash );
    delete [] ticketHash;
            
    
    mWebRequest = startWebRequest( "POST", 
                                   serverURL, 
                                   actionString );
    
    delete [] actionString;

    // nothing selected now (so selection might have changed)
    fireActionPerformed( this );
    */
    }



int HouseObjectPicker::getSelectedObject() {
    
    if( mSelectedIndex >= 0 ) {
        ObjectPriceRecord *r = mObjectList.getElement( mSelectedIndex );
        
        return r->id;
        }
    else {
        return -1;
        }
    }



void HouseObjectPicker::step() {
    /*
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
                        
                        if( numParts != 5 ) {
                            printf( "Unexpected number of parts on house "
                                    "list line: %d\n", numParts );
                            }
                        else {
                            HouseRecord r;
                            
                            r.selected = false;
                            r.draw = false;
                            r.position.x = 0;
                            r.position.y = topOffset - i * lineHeight;
                            
                            sscanf( parts[0], "%d", &( r.uniqueID ) );
                            
                            
                            char found;
                            r.characterName = 
                                trimName(
                                    replaceAll( parts[1], "_", " ", &found ),
                                    lineWidthLeft);
                            
                            r.lastRobberName = 
                                trimName (
                                    replaceAll( parts[2], "_", " ", &found ),
                                    lineWidthLeft );
                            

                            sscanf( parts[3], "%d", &( r.lootValue ) );
                            sscanf( parts[4], "%d", &( r.robAttempts ) );

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
    */    
    }



void HouseObjectPicker::draw() {

    if( mSelectedIndex >= 0 ) {
        ObjectPriceRecord *r = mObjectList.getElement( mSelectedIndex );
        
        SpriteHandle sprite = getObjectSprite( r->id, 0, 0 );

        doublePair center = { 0, 0 };
        

        setDrawColor( 0.5, 0.5, 0.5, 1 );

        drawSquare( center, 1 );
        
        setDrawColor( 0, 0, 0, 1 );

        drawSquare( center, 1 - mPixWidth );

        setDrawColor( 1, 1, 1, 1 );
        drawSprite( sprite, center, 1.0/16.0 );

        char *priceString = autoSprintf( "$%d", r->price );
        
        doublePair pricePos ={ -0.25, -1.5 };
        
        mainFont->drawString( priceString, pricePos, alignLeft );

        delete [] priceString;        
        }
    }

