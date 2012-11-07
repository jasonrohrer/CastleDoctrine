#include "FetchSelfTestReplayPage.h"
#include "ticketHash.h"
#include "message.h"

#include "nameProcessing.h"


#include "minorGems/game/Font.h"
#include "minorGems/game/game.h"

#include "minorGems/util/stringUtils.h"


extern Font *mainFont;


extern char *serverURL;

extern int userID;



FetchSelfTestReplayPage::FetchSelfTestReplayPage() 
        : mWebRequest( -1 ),
          mRecordReady( false ),
          mMenuButton( mainFont, 4, -4, translate( "returnMenu" ) ),
          mReturnToMenu( false ) {

    // clearRecord will do the rest of the NULL-ing
    mLogRecord.robberName = NULL;
    clearRecord();

    addComponent( &mMenuButton );
    mMenuButton.addActionListener( this );
    }


        
FetchSelfTestReplayPage::~FetchSelfTestReplayPage() {
    if( mWebRequest != -1 ) {
        clearWebRequest( mWebRequest );
        }
    clearRecord();
    }


void FetchSelfTestReplayPage::clearRecord() {
    if( mLogRecord.robberName != NULL ) {
        delete [] mLogRecord.robberName;
        delete [] mLogRecord.victimName;
        delete [] mLogRecord.houseMap;
        delete [] mLogRecord.backpackContents;
        delete [] mLogRecord.moveList;        
        }

    mLogRecord.robberName = NULL;
    mLogRecord.victimName = NULL;
    mLogRecord.houseMap = NULL;
    mLogRecord.backpackContents = NULL;
    mLogRecord.moveList = NULL;
    
    mRecordReady = false;
    }



void FetchSelfTestReplayPage::setOwnerID( int inID ) {
    mOwnerID = inID;
    }



char FetchSelfTestReplayPage::getRecordReady() {
    return mRecordReady;
    }



char FetchSelfTestReplayPage::getReturnToMenu() {
    return mReturnToMenu;
    }



RobberyLog FetchSelfTestReplayPage::getLogRecord() {
    return mLogRecord;
    }



void FetchSelfTestReplayPage::actionPerformed( GUIComponent *inTarget ) {
    if( inTarget == &mMenuButton ) {
        mReturnToMenu = true;
        }
    }







void FetchSelfTestReplayPage::step() {
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
                mMenuButton.setVisible( true );
                break;
            case 1: {
                char *result = getWebResult( mWebRequest );
                clearWebRequest( mWebRequest );
                mWebRequest = -1;
                     
                printf( "Web result = %s\n", result );
   
                if( strstr( result, "DENIED" ) != NULL ) {
                    mStatusError = true;
                    mStatusMessageKey = "robberyLogNotFound";
                    mMenuButton.setVisible( true );
                    }
                else {
                    // got a log!
                    
                    SimpleVector<char *> *tokens =
                        tokenizeString( result );
                    
                    if( tokens->size() != 4 ||
                        strcmp( *( tokens->getElement( 3 ) ), "OK" ) != 0 ) {

                        mStatusError = true;
                        mStatusMessageKey = "err_badServerResponse";
                        mMenuButton.setVisible( true );
                    
                        for( int i=0; i<tokens->size(); i++ ) {
                            delete [] *( tokens->getElement( i ) );
                            }
                        }
                    else {    
                        mLogRecord.robberName = 
                            nameParse( *( tokens->getElement( 0 ) ) );
                        mLogRecord.victimName = 
                            stringDuplicate( translate( "nameSelf" ) );
                        mLogRecord.houseMap = *( tokens->getElement( 1 ) );
                        mLogRecord.backpackContents = stringDuplicate( "" );
                        mLogRecord.moveList = *( tokens->getElement( 2 ) );

                        mLogRecord.lootValue = 0;
                        
                        mRecordReady = true;

                        delete [] *( tokens->getElement( 3 ) );
                        }
                    
                    delete tokens;
                    }
                        
                        
                delete [] result;
                }
                break;
            }
        }
    }


        
void FetchSelfTestReplayPage::makeActive( char inFresh ) {
    if( !inFresh ) {
        return;
        }

    clearRecord();
    
    char *ticketHash = getTicketHash();

    char *fullRequestURL = autoSprintf( 
        "%s?action=get_self_test_log&user_id=%d&house_owner_id=%d"
        "&%s",
        serverURL, userID, mOwnerID, ticketHash );
    delete [] ticketHash;
    
    mWebRequest = startWebRequest( "GET", 
                                   fullRequestURL, 
                                   NULL );
    
    printf( "Starting web request with URL %s\n", 
            fullRequestURL );

    delete [] fullRequestURL;
    
    mMenuButton.setVisible( false );

    mStatusError = false;
    mStatusMessageKey = NULL;

    mReturnToMenu = false;
    }

