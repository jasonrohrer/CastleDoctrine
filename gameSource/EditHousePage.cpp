#include "EditHousePage.h"
#include "ticketHash.h"

#include "minorGems/game/Font.h"
#include "minorGems/game/game.h"

#include "minorGems/util/stringUtils.h"


extern Font *mainFont;


extern char *serverURL;

extern int userID;
extern int serverSequenceNumber;



EditHousePage::EditHousePage() 
        : mWebRequest( -1 ),
          mHouseMap( NULL ),
          mStatusError( false ),
          mStatusMessageKey( NULL ) {
    }


        
EditHousePage::~EditHousePage() {
    if( mWebRequest != -1 ) {
        clearWebRequest( mWebRequest );
        }
    if( mHouseMap != NULL ) {
        delete [] mHouseMap;
        }
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
                    
                    int size = strlen( result );
                    
                    mHouseMap = new char[ size + 1 ];
                    
                    sscanf( result, "%s", mHouseMap );
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

    }


        
void EditHousePage::makeActive() {
    char *ticketHash = getTicketHash();

    char *fullRequestURL = autoSprintf( 
        "%s?action=start_edit_house&user_id=%d"
        "&sequence_number=%d"
        "&ticket_hash=%s",
        serverURL, userID, serverSequenceNumber,
        ticketHash );
    delete [] ticketHash;
    
    mWebRequest = startWebRequest( "GET", 
                                   fullRequestURL, 
                                   NULL );
    
    printf( "Starting web request with URL %s\n", 
            fullRequestURL );

    delete [] fullRequestURL;
    }



void EditHousePage::makeNotActive() {
    }
        

        
void EditHousePage::pointerMove( float inX, float inY ) {
    }



void EditHousePage::pointerDown( float inX, float inY ) {
    }



void EditHousePage::pointerDrag( float inX, float inY ) {
    }



void EditHousePage::pointerUp( float inX, float inY ) {
    }

