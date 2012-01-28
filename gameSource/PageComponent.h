#ifndef PAGE_COMPONENT_INCLUDED
#define PAGE_COMPONENT_INCLUDED


#include "minorGems/ui/GUIComponent.h"

#include "minorGems/util/SimpleVector.h"

#include "minorGems/game/doublePair.h"


class PageComponent : public GUIComponent {
    public:
        
        virtual ~PageComponent() {
            };

        
        virtual char isVisible() {
            return true;
            }
        
        
        virtual char isActive() {
            return true;
            }
        
        
        virtual void base_step();
        
        virtual void base_draw( doublePair inViewCenter, 
                                double inViewSize );


        void base_pointerMove( float inX, float inY );

        void base_pointerDown( float inX, float inY );

        void base_pointerDrag( float inX, float inY );

        void base_pointerUp( float inX, float inY );

        void base_keyDown( unsigned char inASCII );
        
        void base_keyUp( unsigned char inASCII );

        void base_specialKeyDown( int inKeyCode );

        void base_specialKeyUp( int inKeyCode );

        
        void base_clearState();
        
        
    protected:
        
        PageComponent() {
            };
        
        // add for default event handling
        void addComponent( PageComponent *inComponent );


        // clears hover or partially-pressed status
        virtual void clearState() {
            };


        virtual void step() {
            };
        
        virtual void draw() {
            };
        
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
        
        SimpleVector<PageComponent*> mComponents;
        

        
    };



#endif
