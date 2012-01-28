#ifndef GAME_PAGE_INCLUDED
#define GAME_PAGE_INCLUDED


#include "minorGems/game/doublePair.h"

#include "PageComponent.h"


class GamePage : public PageComponent {
        

    public:
        

        virtual ~GamePage();
        
        void setStatus( const char *inStatusMessageKey, char inError );


        
        // inFresh set to true when returning to this page
        // after visiting other pages
        // set to false after returning from pause.
        void base_makeActive( char inFresh );
        void base_makeNotActive();


        // override to draw status message
        virtual void base_draw( doublePair inViewCenter, 
                                double inViewSize );


        
        
    protected:
        


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
        

        char mStatusError;
        const char *mStatusMessageKey;


    };


#endif

