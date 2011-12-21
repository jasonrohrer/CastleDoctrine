#include "LoginPage.h"


#include "minorGems/game/Font.h"

#include "minorGems/graphics/openGL/KeyboardHandlerGL.h"


extern Font *mainFont;
extern Font *mainFontFixed;



LoginPage::LoginPage()
        : mEmailField( mainFontFixed, mainFont, 0, 1, 15 ),
          mTicketField( mainFontFixed, mainFont, 0, -1, 15 ) {
    
    mFields[0] = &mEmailField;
    mFields[1] = &mTicketField;
    }

          
        
LoginPage::~LoginPage() {
    }



void LoginPage::step() {
    for( int i=0; i<2; i++ ) {
        mFields[i]->step();
        }
    }


void LoginPage::draw( doublePair inViewCenter, 
                      double inViewSize ) {
    
    for( int i=0; i<2; i++ ) {
        mFields[i]->draw();
        }
    }



void LoginPage::makeActive() {
    mEmailField.focus();
    }


void LoginPage::makeNotActive() {
    for( int i=0; i<2; i++ ) {
        mFields[i]->unfocus();
        }
    }



void LoginPage::pointerUp( float inX, float inY ) {
    for( int i=0; i<2; i++ ) {
        mFields[i]->pointerUp( inX, inY );
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


