#include "GamePage.h"



GamePage::GamePage() {
    }


GamePage::~GamePage() {

    }




void GamePage::base_step(){
    step();
    }



void GamePage::base_draw( doublePair inViewCenter, 
                double inViewSize ){

    draw( inViewCenter, inViewSize );
    }



void GamePage::base_makeActive( char inFresh ){
    makeActive( inFresh );
    }


void GamePage::base_makeNotActive(){
    
    makeNotActive();
    }



void GamePage::base_pointerMove( float inX, float inY ){
    
    pointerMove( inX, inY );
    }



void GamePage::base_pointerDown( float inX, float inY ){
    pointerDown( inX, inY );
    }



void GamePage::base_pointerDrag( float inX, float inY ){
    pointerDrag( inX, inY );
    }



void GamePage::base_pointerUp( float inX, float inY ){
    pointerUp( inX, inY );
    }



void GamePage::base_keyDown( unsigned char inASCII ){
    keyDown( inASCII );
    }


        
void GamePage::base_keyUp( unsigned char inASCII ){
    keyUp( inASCII );
    }



void GamePage::base_specialKeyDown( int inKeyCode ){
    specialKeyDown( inKeyCode );
    }



void GamePage::base_specialKeyUp( int inKeyCode ){
    specialKeyUp( inKeyCode );
    }



void GamePage::addButton( Button *inButton ){
    }


