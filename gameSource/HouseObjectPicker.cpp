#include "HouseObjectPicker.h"


#include "ticketHash.h"
#include "houseObjects.h"

#include "minorGems/game/Font.h"
#include "minorGems/game/game.h"
#include "minorGems/game/drawUtils.h"

#include "minorGems/util/stringUtils.h"


extern Font *mainFont;

extern char *serverURL;

extern int userID;

extern double frameRateFactor;



// these cannot be manually placed by user
#define BLOCK_LIST_SIZE 4

static const char *blockList[BLOCK_LIST_SIZE] = 
{ "floor",
  "start",
  "vault_goal",
  "wall_exterior" 
    };


                                    

HouseObjectPicker::HouseObjectPicker( double inX, double inY,
                                      GamePage *inParentPage )
        : PageComponent( inX, inY ),
          mParentPage( inParentPage ),
          mSelectedIndex( -1 ),
          mPixWidth( 1/16.0 ),
          mUpButton( "up.tga", -1.25, 1, mPixWidth ),
          mDownButton( "down.tga", -1.25, -1, mPixWidth ) {

    mUpButton.setVisible( false );
    mDownButton.setVisible( false );
    
    addComponent( &mUpButton );
    addComponent( &mDownButton );

    mUpButton.addActionListener( this );
    mDownButton.addActionListener( this );

    
    int numObjects;
    int *idList = getFullObjectIDList( &numObjects );
    
    for( int i=0; i<numObjects; i++ ) {
        ObjectPriceRecord r;
        r.id = idList[i];
        r.price = 10;
        
        const char *name = getObjectName( r.id );
        
        char blocked = false;
        for( int b=0; b<BLOCK_LIST_SIZE; b++ ) {
            if( strcmp( name, blockList[b] ) == 0 ) {
                blocked = true;
                break;
                }
            }
        
        if( !blocked ) {
            mObjectList.push_back( r );
            mLocalPresentIDs.push_back( idList[i] );
            }
        }
    delete [] idList;
    
    if( mObjectList.size() > 0 ) {
        mSelectedIndex = 0;
        mUpButton.setVisible( true );
        mDownButton.setVisible( true );
        }
    
    }



HouseObjectPicker::~HouseObjectPicker() {
    }



void HouseObjectPicker::actionPerformed( GUIComponent *inTarget ) {
    char change = false;
    
    if( inTarget == &mUpButton ) {
        mSelectedIndex --;
        if( mSelectedIndex < 0 ) {
            mSelectedIndex = mObjectList.size() - 1;
            }
        change = true;
        }
    else if( inTarget == &mDownButton ) {
        mSelectedIndex ++;
        if( mSelectedIndex > mObjectList.size() - 1 ) {
            mSelectedIndex = 0;
            }
        change = true;    
        }

    
    if( change ) {
        
        mParentPage->setToolTipDirect( 
            (char *)getObjectDescription( 
                mObjectList.getElement( mSelectedIndex )->id, 0 ) );
        
        fireActionPerformed( this );
        }
    
    }




int HouseObjectPicker::getSelectedObject() {
    
    if( mSelectedIndex >= 0 ) {
        ObjectPriceRecord *r = mObjectList.getElement( mSelectedIndex );
        
        return r->id;
        }
    else {
        return -1;
        }
    }



void HouseObjectPicker::step() {
    }



void HouseObjectPicker::draw() {

    if( mSelectedIndex >= 0 ) {
        ObjectPriceRecord *r = mObjectList.getElement( mSelectedIndex );
        
        int numOrientations = getNumOrientations( r->id, 0 );
        
        int orientation = 0;
        if( numOrientations == 4 ) {
            // default to left-facing
            orientation = 3;
            }
        if( numOrientations == 2 ) {
            // default to horizontal
            orientation = 1;
            }
        
        
        SpriteHandle sprite = getObjectSprite( r->id, orientation, 0 );

        doublePair center = { 0, 0 };
        

        setDrawColor( 0.5, 0.5, 0.5, 1 );

        drawSquare( center, 1 );
        
        setDrawColor( 0, 0, 0, 1 );

        drawSquare( center, 1 - mPixWidth );

        setDrawColor( 1, 1, 1, 1 );
        drawSprite( sprite, center, 1.0/16.0 );

        char *priceString = autoSprintf( "$%d", r->price );
        
        doublePair pricePos ={ -0.25, -1.5 };
        
        mainFont->drawString( priceString, pricePos, alignLeft );

        delete [] priceString;        
        }
    }




int *HouseObjectPicker::getIDList( int *outNumIDs ) {
    int numIDs = mObjectList.size();
    
    int *resultList = new int[ numIDs ];
    

    for( int i=0; i<numIDs; i++ ) {
        ObjectPriceRecord *r = mObjectList.getElement( i );
        
        resultList[i] = r->id;
        }
    
    *outNumIDs = numIDs;
    return resultList;
    }

        
        
void HouseObjectPicker::setPrices( ObjectPriceRecord *inRecords, 
                                   int inNumRecords ) {

    mObjectList.deleteAll();

    for( int i=0; i<inNumRecords; i++ ) {
        ObjectPriceRecord r = inRecords[i];
        
        // only allow server-specified objects that we also have locally
        // (server can specify a subset of our local objects, and we
        //  ignore the rest)
        for( int j=0; j<mLocalPresentIDs.size(); j++ ) {
            if( r.id == *( mLocalPresentIDs.getElement( j ) ) ) {
                mObjectList.push_back( r );
                break;
                }
            }
        }
    }



int HouseObjectPicker::getPrice( int inObjectID ) {
    int numIDs = mObjectList.size();
    
    for( int i=0; i<numIDs; i++ ) {
        ObjectPriceRecord *r = mObjectList.getElement( i );
        
        if( r->id == inObjectID ) {
            return r->price;
            }
        }

    return -1;
    }



