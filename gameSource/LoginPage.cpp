#include "LoginPage.h"
#include "ticketHash.h"
#include "message.h"

#include "serialWebRequests.h"


#include "minorGems/game/Font.h"
#include "minorGems/game/game.h"

#include "minorGems/util/stringUtils.h"
#include "minorGems/util/SettingsManager.h"

#include "minorGems/graphics/openGL/KeyboardHandlerGL.h"


extern Font *mainFont;
extern Font *mainFontFixed;

extern char *serverURL;
extern char *reflectorURL;

extern int userID;
extern char *userEmail;
extern char *downloadCode;
extern int serverSequenceNumber;
extern int playerIsAdmin;

extern char gamePlayingBack;


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
                        "23456789ABCDEFGHJKLMNPQRSTUVWXYZ-" ),
          mLoginButton( mainFont, 4, -4, translate( "loginButton" ) ),
          mHaveServerURL( false ),
          mLoggedIn( false ),
          mRequestSteps( 0 ),
          mWebRequest( -1 ) {
    
    mFields[0] = &mEmailField;
    mFields[1] = &mTicketField;

    minRequestSteps = (int)( 60 / frameRateFactor );
    
    addComponent( &mLoginButton );
    addComponent( &mEmailField );
    addComponent( &mTicketField );
    
    mLoginButton.addActionListener( this );
    }

          
        
LoginPage::~LoginPage() {
    
    if( mWebRequest != -1 ) {
        clearWebRequestSerial( mWebRequest );
        }
    }



char LoginPage::getLoginDone() {
    return mLoggedIn;
    }



void LoginPage::step() {
    if( mWebRequest != -1 ) {
        mRequestSteps ++;
        }



    if( !mHaveServerURL ) {
        if( mWebRequest != -1 ) {
            int stepResult = stepWebRequestSerial( mWebRequest );

            if( mRequestSteps > minRequestSteps ) {
            
                switch( stepResult ) {
                    case 0:
                        break;
                    case -1:
                        mStatusError = true;
                        mStatusMessageKey = "err_webRequest";
                        clearWebRequestSerial( mWebRequest );
                        mWebRequest = -1;
                        acceptInput();
                        mLoginButton.setVisible( true );
                        break;
                    case 1: {
                        char *result = getWebResultSerial( mWebRequest );
                        clearWebRequestSerial( mWebRequest );
                        mWebRequest = -1;
                        
                        if( strstr( result, "DENIED" ) != NULL ) {
                            mStatusError = true;
                            mStatusMessageKey = "loginFailed";
                            acceptInput();
                            mLoginButton.setVisible( true );
                            }
                        else {
                            char *possibleURL = trimWhitespace( result );
                            
                            // make sure it looks like a real URL
                            if( strstr( possibleURL, "http" ) != NULL &&
                                strstr( possibleURL, ".php" ) != NULL ) {
                            
                                serverURL = possibleURL;
                                mHaveServerURL = true;

                                // start login process again, now that we
                                // have the server URL
                                // NOT a fresh login, but continuing
                                // the same login action as perceived
                                // by the user.
                                startLogin( false );
                                }
                            else {
                                delete [] possibleURL;
                            
                                mStatusError = true;
                                mStatusMessageKey = "reflectorFailed";
                                clearWebRequestSerial( mWebRequest );
                                mWebRequest = -1;
                                acceptInput();
                                mLoginButton.setVisible( true );
                                }                        
                            }
                        
                        delete [] result;
                        }
                        break;
                    }
                }
            }
        }
    else if( userID == -1 ) {
        if( mWebRequest != -1 ) {
            
            int stepResult = stepWebRequestSerial( mWebRequest );

            if( mRequestSteps > minRequestSteps ) {
                
                switch( stepResult ) {
                    case 0:
                        break;
                    case -1:
                        mStatusError = true;
                        mStatusMessageKey = "err_webRequest";
                        clearWebRequestSerial( mWebRequest );
                        mWebRequest = -1;
                        acceptInput();
                        mLoginButton.setVisible( true );
                        break;
                    case 1: {
                        char *result = getWebResultSerial( mWebRequest );
                        clearWebRequestSerial( mWebRequest );
                        mWebRequest = -1;
                        
                        if( strstr( result, "DENIED" ) != NULL ) {
                            mStatusError = true;
                            mStatusMessageKey = "loginFailed";
                            acceptInput();
                            mLoginButton.setVisible( true );
                            }
                        else {
                            int minClientVersion;
                            
                            char endString[10];
                            
                            int numRead = sscanf( result, "%d %d %d %d %9s",
                                                  &minClientVersion,
                                                  &userID,
                                                  &serverSequenceNumber,
                                                  &playerIsAdmin,
                                                  endString );
                            if( numRead != 5 ) {
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
                            else if( strcmp( endString, "OK" ) == 0 ) {
                                // good email, good client version!
                                // good response (ends with OK)

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
                                    "%s?action=check_hmac&user_id=%d"
                                    "&%s",
                                    serverURL, userID, ticketHash );
                                delete [] ticketHash;

                                mWebRequest = startWebRequestSerial( "GET", 
                                                               fullRequestURL, 
                                                               NULL );
        
                                printf( "Starting web request with URL %s\n", 
                                        fullRequestURL );

                                delete [] fullRequestURL;
                                
                                setWaiting( true );
                                }
                            else {
                                // doesn't end with OK
                                mStatusError = true;
                                mStatusMessageKey = "err_badServerResponse";
                                printf( "Server response: %s\n", result );
                                acceptInput();
                                mLoginButton.setVisible( true );
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

            int result = stepWebRequestSerial( mWebRequest );

            if( mRequestSteps > minRequestSteps ) {
                
                if( result != 0 ) {
                    setWaiting( false );
                    }

                switch( result ) {
                    case 0:
                        break;
                    case -1:
                        mStatusError = true;
                        mStatusMessageKey = "err_webRequest";
                        clearWebRequestSerial( mWebRequest );
                        mWebRequest = -1;
                        // reset entire process
                        userID = -1;
                        acceptInput();
                        mLoginButton.setVisible( true );
                        break;
                    case 1: {
                        char *result = getWebResultSerial( mWebRequest );
                        clearWebRequestSerial( mWebRequest );
                        mWebRequest = -1;
                        
                        if( strstr( result, "OK" ) != NULL ) {
                            printf( "Request not denied, result %s\n",
                                    result );
                            
                            mLoggedIn = true;

                            if( userEmail != NULL ) {
                                delete [] userEmail;
                                }
                            if( downloadCode != NULL ) {
                                delete [] downloadCode;
                                }

                            userEmail = mEmailField.getText();
                            downloadCode = mTicketField.getText();
                            
                            
                            if( !gamePlayingBack ) {
                                
                                SettingsManager::setSetting( "email",
                                                             userEmail );
                                SettingsManager::setSetting( "downloadCode",
                                                             downloadCode );
                                }
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
    setWaiting( false );
    }



void LoginPage::makeActive( char inFresh ) {
    if( mWebRequest == -1 && !mLoggedIn ) {
        acceptInput();
        }

    if( inFresh ) {
        mStatusMessageKey = NULL;
        mStatusError = false;

        if( userEmail != NULL && downloadCode != NULL ) {
            mEmailField.setText( userEmail );
            mTicketField.setText( downloadCode );
            
            mLoginButton.setVisible( false );
            
            startLogin();
            }
        }
    }



void LoginPage::makeNotActive() {
    for( int i=0; i<2; i++ ) {
        mFields[i]->unfocus();
        }
    }



void LoginPage::startLogin( char inFreshLogin ) {

    if( inFreshLogin ) {
        // add pause before advancing screen to avoid flicker
        // on fast response
        mRequestSteps = 0;
        }

    mLoginButton.setVisible( false );
    for( int i=0; i<2; i++ ) {
        mFields[i]->unfocus();
        mFields[i]->setActive( false );
        }
        
    char *email = mEmailField.getText();

    char *fullRequestURL;


    if( !mHaveServerURL ) {
        fullRequestURL = autoSprintf( 
            "%s?action=reflect&email=%s",
            reflectorURL, email );        
        }
    else {

        fullRequestURL = autoSprintf( 
            "%s?action=check_user&email=%s",
            serverURL, email );
        }
    
    delete [] email;

    mWebRequest = startWebRequestSerial( "GET", fullRequestURL, NULL );
        
    printf( "Starting web request with URL %s\n", fullRequestURL );

    delete [] fullRequestURL;

    mStatusError = false;
    mStatusMessageKey = "loginMessage";

    setWaiting( true );
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


