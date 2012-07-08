#ifndef PAGE_COMPONENT_INCLUDED
#define PAGE_COMPONENT_INCLUDED


#include "minorGems/ui/GUIComponent.h"

#include "minorGems/util/SimpleVector.h"

#include "minorGems/game/doublePair.h"


class PageComponent : public GUIComponent {
    public:
        
        virtual ~PageComponent() {
            };

        
        virtual doublePair getCenter();
        
        
        virtual void setParent( PageComponent *inParent );

        // default implementations pass tool tip up to parent

        virtual void setToolTip( const char *inTipKey );
        
        // inTipMessage destroyed by caller
        virtual void setToolTipDirect( const char *inTipMessage );


        
        virtual char isVisible() {
            return true;
            }
        
        
        virtual char isActive() {
            return true;
            }
        
        
        virtual void base_step();
        
        virtual void base_draw( doublePair inViewCenter, 
                                double inViewSize );
        
        void setPosition( double inX, double inY );


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
        
        // inX, inY specify center position of this component
        // Positions of sub components are relative to this center position
        PageComponent( double inX, double inY );
        
        
        // add for default event handling
        void addComponent( PageComponent *inComponent );


        // clears hover or partially-pressed status
        virtual void clearState() {
            };


        virtual void step() {
            };
        
        virtual void draw() {
            };

        
        // these calls receive pointer coordinates relative to component center
        
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
        
        double mX, mY;

        PageComponent *mParent;
        

    };



#endif
