#ifndef PAGE_COMPONENT_INCLUDED
#define PAGE_COMPONENT_INCLUDED


#include "minorGems/ui/GUIComponent.h"

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
        
    protected:
        
        PageComponent() {
            };
        
        
    };



#endif
