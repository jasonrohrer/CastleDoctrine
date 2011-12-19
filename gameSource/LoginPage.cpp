#include "LoginPage.h"


#include "minorGems/game/Font.h"


extern Font *mainFontFixed;



LoginPage::LoginPage()
        : mEmailField( mainFontFixed, 0, 0, 20 ),
          mTicketField( mainFontFixed, 0, 2, 20 ) {
    
    mFields[0] = &mEmailField;
    mFields[1] = &mTicketField;
    }

          
        
LoginPage::~LoginPage() {
    }



void LoginPage::step() {
    }


void LoginPage::draw( doublePair inViewCenter, 
                      double inViewSize ) {
    
    for( int i=0; i<2; i++ ) {
        mFields[i]->draw();
        }
    }



void LoginPage::pointerUp( float inX, float inY ) {
    for( int i=0; i<2; i++ ) {
        mFields[i]->pointerUp( inX, inY );
        }
    }

    

void LoginPage::keyDown( unsigned char inASCII ) {
    for( int i=0; i<2; i++ ) {
        mFields[i]->keyDown( inASCII );
        }
    }


void LoginPage::specialKeyDown( int inKeyCode ) {
    for( int i=0; i<2; i++ ) {
        mFields[i]->specialKeyDown( inKeyCode );
        }
    }


