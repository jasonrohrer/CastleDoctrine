#ifndef GAME_PAGE_INCLUDED
#define GAME_PAGE_INCLUDED


#include "minorGems/game/doublePair.h"

#include "minorGems/util/SimpleVector.h"

#include "PageComponent.h"


class GamePage {
        

    public:
        

        virtual ~GamePage();


        void base_step();
        
        void base_draw( doublePair inViewCenter, 
                       double inViewSize );
        
        // inFresh set to true when returning to this page
        // after visiting other pages
        // set to false after returning from pause.
        void base_makeActive( char inFresh );
        void base_makeNotActive();


        // default implementations do nothing
        // sub classes implement whichever they need
        void base_pointerMove( float inX, float inY );

        void base_pointerDown( float inX, float inY );

        void base_pointerDrag( float inX, float inY );

        void base_pointerUp( float inX, float inY );

        void base_keyDown( unsigned char inASCII );
        
        void base_keyUp( unsigned char inASCII );

        void base_specialKeyDown( int inKeyCode );

        void base_specialKeyUp( int inKeyCode );


        
    protected:
        
        // add for default event handling
        void addComponent( PageComponent *inComponent );
        




        // subclasses override these to provide custom functionality

        virtual void step() {
            };
        
        virtual void draw( doublePair inViewCenter, 
                               double inViewSize ) {
            };
        
        // inFresh set to true when returning to this page
        // after visiting other pages
        // set to false after returning from pause.
        virtual void makeActive( char inFresh ) {
            };
        virtual void makeNotActive() {
            };


        // default implementations do nothing
        // sub classes implement whichever they need
        virtual void pointerMove( float inX, float inY ) {
            };

        virtual void pointerDown( float inX, float inY ) {
            };

        virtual void pointerDrag( float inX, float inY ) {
            };

        virtual void pointerUp( float inX, float inY ) {
            };

        virtual void keyDown( unsigned char inASCII ) {
            };
        
        virtual void keyUp( unsigned char inASCII ) {
            };

        virtual void specialKeyDown( int inKeyCode ) {
            };

        virtual void specialKeyUp( int inKeyCode ) {
            };
        


        
        GamePage();
        
        SimpleVector<PageComponent*> mComponents;
        

        char mStatusError;
        const char *mStatusMessageKey;


    };


#endif

