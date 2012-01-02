#include "minorGems/game/doublePair.h"



class HouseGridDisplay {
    
    public:

        HouseGridDisplay( double inX, double inY );
        
        virtual ~HouseGridDisplay();
        

        // destroyed by caller
        void setHouseMap( char *inHouseMap );
        
        
        // destroyed by caller
        char *getHouseMap();
        
            

        virtual void step();
        
        virtual void draw();

        virtual void pointerMove( float inX, float inY );
        virtual void pointerDown( float inX, float inY );
        virtual void pointerDrag( float inX, float inY );
        
        // returns true if grid changed as a result of
        // this event
        virtual char pointerUp( float inX, float inY );
        

    private:
        
        double mX, mY;
        
        char *mHouseMap;
        
        
        int mHighlightIndex;
        

        int getTileIndex( double inX, double inY );

        doublePair getTilePos( int inIndex );
        
        double mTileRadius;
    };

