#include "GamePage.h"



GamePage::GamePage() {
    }


GamePage::~GamePage() {

    }




void GamePage::base_step(){
    for( int i=0; i<mComponents.size(); i++ ) {
        PageComponent *c = *( mComponents.getElement( i ) );
        
        if( c->isVisible() && c->isActive() ) {
            c->step();
            }
        }
    
    step();
    }



void GamePage::base_draw( doublePair inViewCenter, 
                          double inViewSize ){
    
    for( int i=0; i<mComponents.size(); i++ ) {
        PageComponent *c = *( mComponents.getElement( i ) );
    
        if( c->isVisible() ) {
            c->draw();
            }
        }
    
    draw( inViewCenter, inViewSize );
    }



void GamePage::base_makeActive( char inFresh ){
    makeActive( inFresh );
    }


void GamePage::base_makeNotActive(){
    for( int i=0; i<mComponents.size(); i++ ) {
        PageComponent *c = *( mComponents.getElement( i ) );
        
        c->clearState();
        }
    
    makeNotActive();
    }



void GamePage::base_pointerMove( float inX, float inY ){
    for( int i=0; i<mComponents.size(); i++ ) {
        PageComponent *c = *( mComponents.getElement( i ) );
    
        if( c->isVisible() && c->isActive() ) {
            c->pointerMove( inX, inY );
            }
        }
    
    pointerMove( inX, inY );
    }



void GamePage::base_pointerDown( float inX, float inY ){
    for( int i=0; i<mComponents.size(); i++ ) {
        PageComponent *c = *( mComponents.getElement( i ) );
    
        if( c->isVisible() && c->isActive() ) {
            c->pointerDown( inX, inY );
            }
        }
    
    pointerDown( inX, inY );
    }



void GamePage::base_pointerDrag( float inX, float inY ){
    for( int i=0; i<mComponents.size(); i++ ) {
        PageComponent *c = *( mComponents.getElement( i ) );
    
        if( c->isVisible() && c->isActive() ) {
            c->pointerDrag( inX, inY );
            }
        }

    pointerDrag( inX, inY );
    }



void GamePage::base_pointerUp( float inX, float inY ){
    for( int i=0; i<mComponents.size(); i++ ) {
        PageComponent *c = *( mComponents.getElement( i ) );
    
        if( c->isVisible() && c->isActive() ) {
            c->pointerUp( inX, inY );
            }
        }

    pointerUp( inX, inY );
    }



void GamePage::base_keyDown( unsigned char inASCII ){
    for( int i=0; i<mComponents.size(); i++ ) {
        PageComponent *c = *( mComponents.getElement( i ) );
    
        if( c->isVisible() && c->isActive() ) {
            c->keyDown( inASCII );
            }
        }

    keyDown( inASCII );
    }


        
void GamePage::base_keyUp( unsigned char inASCII ){
    for( int i=0; i<mComponents.size(); i++ ) {
        PageComponent *c = *( mComponents.getElement( i ) );
    
        if( c->isVisible() && c->isActive() ) {
            c->keyUp( inASCII );
            }
        }

    keyUp( inASCII );
    }



void GamePage::base_specialKeyDown( int inKeyCode ){
    for( int i=0; i<mComponents.size(); i++ ) {
        PageComponent *c = *( mComponents.getElement( i ) );
    
        if( c->isVisible() && c->isActive() ) {
            c->specialKeyDown( inKeyCode );
            }
        }

    specialKeyDown( inKeyCode );
    }



void GamePage::base_specialKeyUp( int inKeyCode ){
    for( int i=0; i<mComponents.size(); i++ ) {
        PageComponent *c = *( mComponents.getElement( i ) );
    
        if( c->isVisible() && c->isActive() ) {
            c->specialKeyUp( inKeyCode );
            }
        }

    specialKeyUp( inKeyCode );
    }



void GamePage::addComponent( PageComponent *inComponent ){

    mComponents.push_back( inComponent );
    }


