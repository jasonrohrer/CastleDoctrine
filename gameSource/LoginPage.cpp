#include "LoginPage.h"
#include "ticketHash.h"

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
          mWebRequest( -1 ),
          mStatusError( false ),
          mStatusMessageKey( NULL ) {
    
    mFields[0] = &mEmailField;
    mFields[1] = &mTicketField;

    minRequestSteps = (int)( 60 / frameRateFactor );


    char *email = SettingsManager::getStringSetting( "email" );
    char *code = SettingsManager::getStringSetting( "downloadCode" );
    
    if( email != NULL && code != NULL ) {
        mEmailField.setText( email );
        mTicketField.setText( code );
        
        startLogin();
        }
    
    if( email != NULL ) {
        delete [] email;
        }
    if( code != NULL ) {
        delete [] code;
        }    
    }

          
        
LoginPage::~LoginPage() {
    
    if( mWebRequest != -1 ) {
        clearWebRequest( mWebRequest );
        }
    }



void LoginPage::step() {
    if( mWebRequest != -1 ) {
        mRequestSteps ++;
        }
    else {
        for( int i=0; i<2; i++ ) {
            mFields[i]->step();
            }
        }
    
    if( userID == -1 ) {
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
                        mEmailField.focus();
                        break;
                    case 1: {
                        char *result = getWebResult( mWebRequest );
                        clearWebRequest( mWebRequest );
                        mWebRequest = -1;
                        
                        if( strstr( result, "DENIED" ) != NULL ) {
                            mStatusError = true;
                            mStatusMessageKey = "loginFailed";
                            mEmailField.focus();
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
                                mEmailField.focus();
                                }
                            else if( minClientVersion > versionNumber ) {
                                mStatusError = true;
                                mStatusMessageKey = "outOfDateClient";
                                mEmailField.focus();
                                }
                            else {
                                // good email, good client version!

                                // FIXME:  need separate server call
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
                        mEmailField.focus();
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
                            mEmailField.focus();
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
    
        


    for( int i=0; i<2; i++ ) {
        mFields[i]->draw();
        }

    if( mWebRequest == -1 && !mLoggedIn ) {
        mLoginButton.draw();
        }


    setDrawColor( 1, 1, 1, 1 );
    
    doublePair labelPos = { 0, -7 };

    mainFont->drawString( translate( "quitMessage" ), 
                          labelPos, alignCenter );


    if( mStatusMessageKey != NULL ) {
        labelPos.y = -5;

        if( mStatusError ) {
            setDrawColor( 1, 0, 0, 1 );
            }
        
        mainFont->drawString( translate( mStatusMessageKey ), 
                              labelPos, alignCenter );
        }
    }



void LoginPage::makeActive() {
    if( mWebRequest == -1 && !mLoggedIn ) {
        mEmailField.focus();
        }
    }


void LoginPage::makeNotActive() {
    for( int i=0; i<2; i++ ) {
        mFields[i]->unfocus();
        }
    }


void LoginPage::pointerMove( float inX, float inY ) {
    if( mWebRequest != -1 ) {
        return;
        }
    
    mLoginButton.pointerMove( inX, inY );
    }


void LoginPage::pointerDown( float inX, float inY ) {
    if( mWebRequest != -1 || mLoggedIn ) {
        return;
        }
    
    mLoginButton.pointerDown( inX, inY );
    }


void LoginPage::pointerDrag( float inX, float inY ) {
    if( mWebRequest != -1 || mLoggedIn  ) {
        return;
        }
    
    mLoginButton.pointerDrag( inX, inY );
    }



void LoginPage::startLogin() {
    for( int i=0; i<2; i++ ) {
        mFields[i]->unfocus();
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



void LoginPage::pointerUp( float inX, float inY ) {
    if( mWebRequest != -1 || mLoggedIn  ) {
        return;
        }
    
    for( int i=0; i<2; i++ ) {
        mFields[i]->pointerUp( inX, inY );
        }

    if( mLoginButton.pointerUp( inX, inY ) ) {
        // login pressed
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
    if( inASCII == 9 ) {
        // tab
        switchFields();
        return;
        }
    
    
    for( int i=0; i<2; i++ ) {
        mFields[i]->keyDown( inASCII );
        }
    }



void LoginPage::keyUp( unsigned char inASCII ) {
    for( int i=0; i<2; i++ ) {
        mFields[i]->keyUp( inASCII );
        }
    }



void LoginPage::specialKeyDown( int inKeyCode ) {
    if( inKeyCode == MG_KEY_DOWN ||
        inKeyCode == MG_KEY_UP ) {
        
        switchFields();
        return;
        }
    

    for( int i=0; i<2; i++ ) {
        mFields[i]->specialKeyDown( inKeyCode );
        }
    }



void LoginPage::specialKeyUp( int inKeyCode ) {
    for( int i=0; i<2; i++ ) {
        mFields[i]->specialKeyUp( inKeyCode );
        }
    }



