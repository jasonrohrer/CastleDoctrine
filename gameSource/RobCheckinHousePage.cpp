#include "RobCheckinHousePage.h"
#include "ticketHash.h"
#include "message.h"


#include "minorGems/game/Font.h"
#include "minorGems/game/game.h"

#include "minorGems/util/stringUtils.h"


extern Font *mainFont;


extern char *serverURL;

extern int userID;



RobCheckinHousePage::RobCheckinHousePage() 
        : mWebRequest( -1 ),
          mMoveList( NULL ),
          mHouseMap( NULL ),
          mSuccess( false ),
          mHomeButton( mainFont, 4, -4, translate( "returnHome" ) ),
          mStartOverButton( mainFont, 4, -4, translate( "startOver" ) ),
          mReturnToHome( false ),
          mStartOver( true ) {

    addComponent( &mHomeButton );
    mHomeButton.addActionListener( this );

    addComponent( &mStartOverButton );
    mStartOverButton.addActionListener( this );
    }


        
RobCheckinHousePage::~RobCheckinHousePage() {
    if( mWebRequest != -1 ) {
        clearWebRequest( mWebRequest );
        }
    if( mBackpackContents != NULL ) {
        delete [] mBackpackContents;
        }
    if( mHouseMap != NULL ) {
        delete [] mHouseMap;
        }
    if( mMoveList != NULL ) {
        delete [] mMoveList;
        }
    }



char RobCheckinHousePage::getReturnToHome() {
    return mReturnToHome;
    }



char RobCheckinHousePage::getStartOver() {
    return mStartOver;
    }



void RobCheckinHousePage::setBackpackContents( char *inBackpackContents ) {
    if( mBackpackContents != NULL ) {
        delete [] mBackpackContents;
        }
    mBackpackContents = stringDuplicate( inBackpackContents );
    }



void RobCheckinHousePage::setHouseMap( char *inHouseMap ) {
    if( mHouseMap != NULL ) {
        delete [] mHouseMap;
        }
    mHouseMap = stringDuplicate( inHouseMap );
    }



void RobCheckinHousePage::setMoveList( char *inMoveList ) {
    if( mMoveList != NULL ) {
        delete [] mMoveList;
        }
    mMoveList = stringDuplicate( inMoveList );
    }



void RobCheckinHousePage::setSuccess( char inSuccess ) {
    mSuccess = inSuccess;
    }



void RobCheckinHousePage::actionPerformed( GUIComponent *inTarget ) {
    if( inTarget == &mHomeButton ) {
        mReturnToHome = true;
        }
    else if( inTarget == &mStartOverButton ) {
        mStartOver = true;
        }
    }


void RobCheckinHousePage::step() {
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
                mHomeButton.setVisible( true );
                break;
            case 1: {
                char *result = getWebResult( mWebRequest );
                clearWebRequest( mWebRequest );
                mWebRequest = -1;
                     
                printf( "Web result = %s\n", result );
   
                if( strstr( result, "DENIED" ) != NULL ) {
                    mStatusError = true;
                    mStatusMessageKey = "houseCheckInFailed";
                    mHomeButton.setVisible( true );
                    }
                else {
                    // house checked in!
                    
                    SimpleVector<char *> *tokens =
                        tokenizeString( result );
                    
                    if( tokens->size() != 2 ||
                        strcmp( *( tokens->getElement( 1 ) ), "OK" ) != 0 ) {
                        mStatusError = true;
                        mStatusMessageKey = "err_badServerResponse";
                        mHomeButton.setVisible( true );                    
                        }
                    else {
                        int value;

                        sscanf( *( tokens->getElement( 0 ) ), 
                                "%d", &value );
                        

                        
                        if( mSuccess ) {
                            mStatusError = false;

                            char *robReport = 
                                autoSprintf( translate( "robSuccess" ), 
                                             value );
                            
                            setStatusDirect( robReport, false );
                            
                            delete [] robReport;
                            
                            mHomeButton.setVisible( true );
                            }
                        else {
                            mStatusError = true;
                            mStatusMessageKey = "deathMessage";
                            mStartOverButton.setVisible( true );
                            }
                        
                        }

                    for( int i=0; i<tokens->size(); i++ ) {
                        delete [] *( tokens->getElement( i ) );
                        }
                    delete tokens;
                    }
                        
                        
                delete [] result;
                }
                break;
            }
        }
    }



        
void RobCheckinHousePage::makeActive( char inFresh ) {
    if( !inFresh ) {
        return;
        }
    
    // send back to server            
    char *ticketHash = getTicketHash();
        
            
    
    char *actionString = autoSprintf( 
        "action=end_rob_house&user_id=%d"
        "&%s"
        "&success=%d"
        "&backpack_contents=%s"
        "&move_list=%s"
        "&house_map=%s",
        userID, ticketHash, mSuccess, mBackpackContents, 
        mMoveList, mHouseMap );
    delete [] ticketHash;
            
    
    mWebRequest = startWebRequest( "POST", 
                                   serverURL, 
                                   actionString );
    
    printf( "Starting web request %s %s\n", serverURL, actionString );

    delete [] actionString;

    mReturnToHome = false;
    mStartOver = false;
    
    mStatusError = false;
    mStatusMessageKey = NULL;

    setStatusDirect( NULL, false );
    

    mHomeButton.setVisible( false );
    mStartOverButton.setVisible( false );
    }


