#include "FetchRobberyReplayPage.h"
#include "ticketHash.h"
#include "message.h"


#include "minorGems/game/Font.h"
#include "minorGems/game/game.h"

#include "minorGems/util/stringUtils.h"


extern Font *mainFont;


extern char *serverURL;

extern int userID;



FetchRobberyReplayPage::FetchRobberyReplayPage() 
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


        
FetchRobberyReplayPage::~FetchRobberyReplayPage() {
    if( mWebRequest != -1 ) {
        clearWebRequest( mWebRequest );
        }
    clearRecord();
    }


void FetchRobberyReplayPage::clearRecord() {
    if( mLogRecord.robberName != NULL ) {
        delete [] mLogRecord.robberName;
        delete [] mLogRecord.victimName;
        delete [] mLogRecord.houseMap;
        delete [] mLogRecord.loadout;
        delete [] mLogRecord.moveList;        
        }

    mLogRecord.robberName = NULL;
    mLogRecord.victimName = NULL;
    mLogRecord.houseMap = NULL;
    mLogRecord.loadout = NULL;
    mLogRecord.moveList = NULL;
    
    mRecordReady = false;
    }



void FetchRobberyReplayPage::setLogID( int inID ) {
    mLogID = inID;
    }



char FetchRobberyReplayPage::getRecordReady() {
    return mRecordReady;
    }



char FetchRobberyReplayPage::getReturnToMenu() {
    return mReturnToMenu;
    }



RobberyLog FetchRobberyReplayPage::getLogRecord() {
    return mLogRecord;
    }



void FetchRobberyReplayPage::actionPerformed( GUIComponent *inTarget ) {
    if( inTarget == &mMenuButton ) {
        mReturnToMenu = true;
        }
    }


void FetchRobberyReplayPage::step() {
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
                    
                    if( tokens->size() != 5 ) {
                        mStatusError = true;
                        mStatusMessageKey = "err_badServerResponse";
                        mMenuButton.setVisible( true );
                    
                        for( int i=0; i<tokens->size(); i++ ) {
                            delete [] *( tokens->getElement( i ) );
                            }
                        }
                    else {    
                        mLogRecord.robberName = *( tokens->getElement( 0 ) );
                        mLogRecord.victimName = *( tokens->getElement( 1 ) );
                        mLogRecord.houseMap = *( tokens->getElement( 2 ) );
                        mLogRecord.loadout = *( tokens->getElement( 3 ) );
                        mLogRecord.moveList = *( tokens->getElement( 4 ) );

                        mRecordReady = true;
                        }
                    
                    delete tokens;
                    }
                        
                        
                delete [] result;
                }
                break;
            }
        }
    }


        
void FetchRobberyReplayPage::makeActive( char inFresh ) {
    if( !inFresh ) {
        return;
        }

    clearRecord();
    
    char *ticketHash = getTicketHash();

    char *fullRequestURL = autoSprintf( 
        "%s?action=get_robbery_log&user_id=%d&log_id=%d"
        "&%s",
        serverURL, userID, mLogID, ticketHash );
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
    }

