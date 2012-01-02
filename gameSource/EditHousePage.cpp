#include "EditHousePage.h"
#include "ticketHash.h"

#include "minorGems/game/Font.h"
#include "minorGems/game/game.h"
#include "minorGems/game/drawUtils.h"

#include "minorGems/util/stringUtils.h"


extern Font *mainFont;


extern char *serverURL;

extern int userID;



EditHousePage::EditHousePage() 
        : mWebRequest( -1 ),
          mStatusError( false ),
          mStatusMessageKey( NULL ),
          mGridDisplay( 0, 0 ),
          mDoneButton( mainFont, 9, -5, translate( "doneEdit" ) ) {
    }


        
EditHousePage::~EditHousePage() {
    if( mWebRequest != -1 ) {
        clearWebRequest( mWebRequest );
        }
    }



void EditHousePage::setHouseMap( char *inHouseMap ) {
    mGridDisplay.setHouseMap( inHouseMap );
    }




void EditHousePage::step() {
    if( mWebRequest != -1 ) {
            
        int result = stepWebRequest( mWebRequest );
                
        switch( result ) {
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
                        
                if( strstr( result, "DENIED" ) != NULL ) {
                    mStatusError = true;
                    mStatusMessageKey = "houseBeingRobbed";
                    }
                else {
                    // house checked out!
                    
                    //int size = strlen( result );
                    
                    //mHouseMap = new char[ size + 1 ];
                    
                    //sscanf( result, "%s", mHouseMap );
                    }
                        
                        
                delete [] result;
                }
                break;
            }
        }
    }


        
void EditHousePage::draw( doublePair inViewCenter, 
                          double inViewSize ) {

    if( mStatusMessageKey != NULL ) {
    
        doublePair labelPos = { 0, -12 };

        if( mStatusError ) {
            setDrawColor( 1, 0, 0, 1 );
            }
        
        mainFont->drawString( translate( mStatusMessageKey ), 
                              labelPos, alignCenter );
        }

    if( mWebRequest == -1 ) {
        mDoneButton.draw();
        }

    mGridDisplay.draw();
    }


        
void EditHousePage::makeActive() {
    /*
    char *ticketHash = getTicketHash();

    char *fullRequestURL = autoSprintf( 
        "%s?action=start_edit_house&user_id=%d"
        "&%s",
        serverURL, userID, ticketHash );
    delete [] ticketHash;
    
    mWebRequest = startWebRequest( "GET", 
                                   fullRequestURL, 
                                   NULL );
    
    printf( "Starting web request with URL %s\n", 
            fullRequestURL );

    delete [] fullRequestURL;
    */
    }



void EditHousePage::makeNotActive() {
    }
        

        
void EditHousePage::pointerMove( float inX, float inY ) {
    if( mWebRequest != -1 ) {
        return;
        }
    
    mGridDisplay.pointerMove( inX, inY );
    mDoneButton.pointerMove( inX, inY );
    }



void EditHousePage::pointerDown( float inX, float inY ) {
    if( mWebRequest != -1 ) {
        return;
        } 

    mGridDisplay.pointerDown( inX, inY );
    mDoneButton.pointerDown( inX, inY );
    }



void EditHousePage::pointerDrag( float inX, float inY ) {
    if( mWebRequest != -1 ) {
        return;
        }

    mGridDisplay.pointerDrag( inX, inY );
    mDoneButton.pointerDrag( inX, inY );
    }



void EditHousePage::pointerUp( float inX, float inY ) {
    if( mWebRequest != -1 ) {
        return;
        }
    
    mGridDisplay.pointerUp( inX, inY );
    mDoneButton.pointerUp( inX, inY );
    }

