#include "LoginPage.h"
#include "ticketHash.h"
#include "message.h"


#include "minorGems/game/Font.h"
#include "minorGems/game/game.h"

#include "minorGems/util/stringUtils.h"
#include "minorGems/util/SettingsManager.h"

#include "minorGems/graphics/openGL/KeyboardHandlerGL.h"


extern Font *mainFont;
extern Font *mainFontFixed;

extern char *serverURL;

extern int userID;
extern char *downloadCode;
extern int serverSequenceNumber;


extern double frameRateFactor;

extern int versionNumber;



static int minRequestSteps;



LoginPage::LoginPage()
        : mEmailField( mainFontFixed, mainFont, 0, 1, 13, false, 
                       translate( "email" ),
                       NULL,
                       // forbid only spaces
                       " "),
          mTicketField( mainFontFixed, mainFont, 0, -1, 13, true,
                        translate( "downloadCode" ),
                        // allow only ticket code characters
                        "ABCDEFWHJKXMNPTY" ),
          mLoginButton( mainFont, 4, -4, translate( "loginButton" ) ),
          mLoggedIn( false ),
          mRequestSteps( 0 ),
          mWebRequest( -1 ) {
    
    mFields[0] = &mEmailField;
    mFields[1] = &mTicketField;

    minRequestSteps = (int)( 60 / frameRateFactor );


    char *email = SettingsManager::getStringSetting( "email" );
    char *code = SettingsManager::getStringSetting( "downloadCode" );
    
    if( email != NULL && code != NULL ) {
        mEmailField.setText( email );
        mTicketField.setText( code );
        
        mLoginButton.setVisible( false );
        
        startLogin();
        }
    
    if( email != NULL ) {
        delete [] email;
        }
    if( code != NULL ) {
        delete [] code;
        }

    addComponent( &mLoginButton );
    addComponent( &mEmailField );
    addComponent( &mTicketField );
    
    mLoginButton.addActionListener( this );
    }

          
        
LoginPage::~LoginPage() {
    
    if( mWebRequest != -1 ) {
        clearWebRequest( mWebRequest );
        }
    }



char LoginPage::getLoginDone() {
    return mLoggedIn;
    }



void LoginPage::step() {
    if( mWebRequest != -1 ) {
        mRequestSteps ++;
        }
    
    if( userID == -1 ) {
        if( mWebRequest != -1 ) {
            
            int stepResult = stepWebRequest( mWebRequest );

            if( mRequestSteps > minRequestSteps ) {
                
                switch( stepResult ) {
                    case 0:
                        break;
                    case -1:
                        mStatusError = true;
                        mStatusMessageKey = "err_webRequest";
                        clearWebRequest( mWebRequest );
                        mWebRequest = -1;
                        acceptInput();
                        mLoginButton.setVisible( true );
                        break;
                    case 1: {
                        char *result = getWebResult( mWebRequest );
                        clearWebRequest( mWebRequest );
                        mWebRequest = -1;
                        
                        if( strstr( result, "DENIED" ) != NULL ) {
                            mStatusError = true;
                            mStatusMessageKey = "loginFailed";
                            acceptInput();
                            mLoginButton.setVisible( true );
                            }
                        else {
                            int minClientVersion;
                            
                            int numRead = sscanf( result, "%d %d %d",
                                                  &minClientVersion,
                                                  &userID,
                                                  &serverSequenceNumber );
                            if( numRead != 3 ) {
                                mStatusError = true;
                                mStatusMessageKey = "err_badServerResponse";
                                printf( "Server response: %s\n", result );
                                acceptInput();
                                mLoginButton.setVisible( true );
                                }
                            else if( minClientVersion > versionNumber ) {
                                mStatusError = true;
                                mStatusMessageKey = "outOfDateClient";
                                acceptInput();
                                mLoginButton.setVisible( true );
                                }
                            else {
                                // good email, good client version!

                                // need separate server call
                                // from checking out a house
                                // (because that can fail if house is 
                                //  currently being robbed)


                                // now check download code securely
                                if( downloadCode != NULL ) {
                                    delete [] downloadCode;
                                    }
                                downloadCode = mTicketField.getText();

                                char *ticketHash = getTicketHash();

                                char *fullRequestURL = autoSprintf( 
                                    "%s?action=check_hash&user_id=%d"
                                    "&%s",
                                    serverURL, userID, ticketHash );
                                delete [] ticketHash;

                                mWebRequest = startWebRequest( "GET", 
                                                               fullRequestURL, 
                                                               NULL );
        
                                printf( "Starting web request with URL %s\n", 
                                        fullRequestURL );

                                delete [] fullRequestURL;
                                }
                            }
                        
                        
                        delete [] result;
                        }
                        break;
                    }
                }
            }
        }
    else if( !mLoggedIn ) {
        if( mWebRequest != -1 ) {

            int result = stepWebRequest( mWebRequest );

            if( mRequestSteps > minRequestSteps ) {
                
                switch( result ) {
                    case 0:
                        break;
                    case -1:
                        mStatusError = true;
                        mStatusMessageKey = "err_webRequest";
                        clearWebRequest( mWebRequest );
                        mWebRequest = -1;
                        // reset entire process
                        userID = -1;
                        acceptInput();
                        mLoginButton.setVisible( true );
                        break;
                    case 1: {
                        char *result = getWebResult( mWebRequest );
                        clearWebRequest( mWebRequest );
                        mWebRequest = -1;
                        
                        if( strstr( result, "OK" ) != NULL ) {
                            printf( "Request not denied, result %s\n",
                                    result );
                            
                            mLoggedIn = true;

                            char *email = mEmailField.getText();
                            char *code = mTicketField.getText();
                            
                            SettingsManager::setSetting( "email",
                                                         email );
                            SettingsManager::setSetting( "downloadCode",
                                                         code );
                            delete [] email;
                            delete [] code;                            
                            }
                        else {                            
                            mStatusError = true;
                            mStatusMessageKey = "loginFailed";
                            // reset entire process
                            userID = -1;
                            acceptInput();
                            mLoginButton.setVisible( true );
                            }
                        delete [] result;
                        }
                        break;
                    }
                }
            }
        }
    }



void LoginPage::draw( doublePair inViewCenter, 
                      double inViewSize ) {


    doublePair labelPos = { 0, -7 };

    drawMessage( "quitMessage", labelPos );
    }



void LoginPage::acceptInput() {
    mEmailField.focus();
        
    for( int i=0; i<2; i++ ) {
        mFields[i]->setActive( true );
        }
    }



void LoginPage::makeActive( char inFresh ) {
    if( mWebRequest == -1 && !mLoggedIn ) {
        acceptInput();
        }

    if( inFresh ) {
        mStatusMessageKey = NULL;
        mStatusError = false;
        }
    }



void LoginPage::makeNotActive() {
    for( int i=0; i<2; i++ ) {
        mFields[i]->unfocus();
        }
    }



void LoginPage::startLogin() {
    mLoginButton.setVisible( false );
    for( int i=0; i<2; i++ ) {
        mFields[i]->unfocus();
        mFields[i]->setActive( false );
        }
        
    char *email = mEmailField.getText();

    char *fullRequestURL = autoSprintf( 
        "%s?action=check_user&email=%s",
        serverURL, email );

    delete [] email;

    mRequestSteps = 0;
    mWebRequest = startWebRequest( "GET", fullRequestURL, NULL );
        
    printf( "Starting web request with URL %s\n", fullRequestURL );

    delete [] fullRequestURL;

    mStatusError = false;
    mStatusMessageKey = "loginMessage";
    }



void LoginPage::actionPerformed( GUIComponent *inTarget ) {
    if( inTarget == &mLoginButton ) {
        startLogin();
        }
    }



void LoginPage::switchFields() {
    if( mFields[0]->isFocused() ) {
        mFields[1]->focus();
        }
    else {
        mFields[0]->focus();
        }
    }

    

void LoginPage::keyDown( unsigned char inASCII ) {
    if( mWebRequest != -1 || mLoggedIn  ) {
        return;
        }

    if( inASCII == 9 ) {
        // tab
        switchFields();
        return;
        }

    if( inASCII == 10 || inASCII == 13 ) {
        // enter key
        
        if( mTicketField.isFocused() ) {
            startLogin();
            
            return;
            }
        else if( mEmailField.isFocused() ) {
            switchFields();
            }
        }
    }



void LoginPage::specialKeyDown( int inKeyCode ) {
    if( mWebRequest != -1 || mLoggedIn  ) {
        return;
        }

    if( inKeyCode == MG_KEY_DOWN ||
        inKeyCode == MG_KEY_UP ) {
        
        switchFields();
        return;
        }
    }


