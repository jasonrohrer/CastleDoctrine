#include "InventorySlotButton.h"


#include "tools.h"


#include "minorGems/util/stringUtils.h"

#include "minorGems/game/drawUtils.h"
#include "minorGems/game/gameGraphics.h"



InventorySlotButton::InventorySlotButton( Font *inDisplayFont,
                                          double inX, double inY,
                                          double inDrawScale )
        :
        // lie about sprite size so that button padding isn't so huge 
        SpriteButton( NULL, 18, 18, inX, inY, inDrawScale ),
        mFont( inDisplayFont ),
        mObjectID( -1 ),
        mQuantity( 0 ),
        mRingOn( false ) {
    
    mPixWidth *= 2;
    
    mOverrideHighlightColor = true;
    }



void InventorySlotButton::setObject( int inID ) {
    mObjectID = inID;
    
    if( mObjectID == -1 ) {
        setMouseOverTip( "" );
        mSprite = NULL;
        mQuantity = 0;
        }
    else {
        setMouseOverTip( getToolDescription( mObjectID ) );
        mSprite = getToolSprite( mObjectID );
        mQuantity = 1;
        }
    }


        
int InventorySlotButton::getObject() {
    return mObjectID;
    }



void InventorySlotButton::setQuantity( int inQuantity ) {
    mQuantity = inQuantity;
    
    if( mQuantity == 0 ) {
        setObject( -1 );
        }
    }


        
int InventorySlotButton::getQuantity() {
    return mQuantity;
    }




void InventorySlotButton::drawContents() {
    SpriteButton::drawContents();
    
    if( mQuantity > 1 ) {
        
        
        // darken corner

        double verts[6];
        float colors[12];
        
        verts[0] = -mWide / 2;
        verts[1] = -mHigh / 2;

        colors[0] = 0;
        colors[1] = 0;
        colors[2] = 0;
        colors[3] = 0;
        
        
        verts[2] = mWide / 2;
        verts[3] = -mHigh / 2;

        colors[4] = 0;
        colors[5] = 0;
        colors[6] = 0;
        colors[7] = 0.75;


        verts[4] = mWide / 2;
        verts[5] = mHigh / 2;

        
        colors[8] = 0;
        colors[9] = 0;
        colors[10] = 0;
        colors[11] = 0;

        drawTrianglesColor( 1, verts, colors );
        


        doublePair textPos = { 0.5 - mDrawScale, -0.5 };
        
        char *text = autoSprintf( "%d", mQuantity );
        
        setDrawColor( 0.828, 0.647, 0.212, 1 );

        mFont->drawString( text, textPos, alignRight );
    
        delete [] text;
        }
    
    }




void InventorySlotButton::setRingOn( char inRingOn ) {
    mRingOn = inRingOn;
    }





void InventorySlotButton::draw() {

    if( mRingOn ) {

        // ring
        setDrawColor( 0.25, 1.0, 0.93, 1 );
        
        double fullWide = mWide + 4 * mPixWidth;
        double fullHigh = mHigh + 4 * mPixWidth;

        drawRect( - fullWide / 2, - fullHigh / 2, 
                  fullWide / 2, fullHigh / 2 );


        // background inside ring
        setDrawColor( 0, 0, 0, 1 );
        
        fullWide -= 2 * mPixWidth;
        fullHigh -= 2 * mPixWidth;
        
        drawRect( - fullWide / 2, - fullHigh / 2, 
                  fullWide / 2, fullHigh / 2 );
        }
    

    // draw button's contents on top
    SpriteButton::draw();
    }
