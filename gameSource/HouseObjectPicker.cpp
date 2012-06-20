#include "HouseObjectPicker.h"


#include "ticketHash.h"
#include "houseObjects.h"
#include "tools.h"

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
                                      char inTools )
        : PageComponent( inX, inY ),
          mShowTools( inTools ), 
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
    int *idList;

    if( mShowTools ) {
        idList = getFullToolIDList( &numObjects );
        }
    else {
        idList = getFullObjectIDList( &numObjects );
        }
    
    for( int i=0; i<numObjects; i++ ) {
        ObjectPriceRecord r;
        r.id = idList[i];
        r.price = 10;
        
        const char *name;
        if( mShowTools ) {
            name = getToolName( r.id );
            }
        else {
            name = getObjectName( r.id );
            }
        
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




void HouseObjectPicker::triggerToolTip() {
    if( mShowTools ) {
        setToolTipDirect( 
            (char *)getToolDescription( 
                mObjectList.getElement( mSelectedIndex )->id ) );
        }
    else {    
        setToolTipDirect( 
            (char *)getObjectDescription( 
                mObjectList.getElement( mSelectedIndex )->id, 0 ) );
        }
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
        triggerToolTip();
        
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

        int orientation = 0;

        if( !mShowTools ) {    
            int numOrientations = getNumOrientations( r->id, 0 );
            
            if( numOrientations == 4 ) {
                // default to left-facing
                orientation = 3;
                }
            if( numOrientations == 2 ) {
                // default to horizontal
                orientation = 1;
                }
            }
        
        
        
        SpriteHandle sprite;

        if( mShowTools ) {
            sprite = getToolSprite( r->id );
            }
        else {
            sprite = getObjectSprite( r->id, orientation, 0 );
            }
        
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




void HouseObjectPicker::setPriceList( char *inPriceList ) {
    // parse it
    int numBigParts;
    char **bigParts = split( inPriceList, ":", &numBigParts );
    
    if( numBigParts == 3 ) {
        
        char *listBody = bigParts[1];
        
        int numPairs;
        char **pairs = split( listBody, "#", &numPairs );
        
        ObjectPriceRecord *records = new ObjectPriceRecord[ numPairs ];
        
        for( int i=0; i<numPairs; i++ ) {
            int numParts;
            char **parts = split( pairs[i], "@", &numParts );
            
            if( numParts == 2 ) {
                
                // default in case scan fails
                records[i].id = -1;
                records[i].price = 1;

                sscanf( parts[0], "%d", &( records[i].id ) );

                sscanf( parts[1], "%d", &( records[i].price ) );
                }
            
            for( int p=0; p<numParts; p++ ) {
                delete [] parts[p];
                }
            delete [] parts;

            delete [] pairs[i];
            }
        delete [] pairs;

        setPrices( records, numPairs );
        delete [] records;
        }
    
    for( int i=0; i<numBigParts; i++ ) {
        delete [] bigParts[i];
        }
    delete [] bigParts;
    }

        
        
void HouseObjectPicker::setPrices( ObjectPriceRecord *inRecords, 
                                   int inNumRecords ) {

    mObjectList.deleteAll();

    for( int i=0; i<inNumRecords; i++ ) {
        ObjectPriceRecord r = inRecords[i];
        
        // only allow server-specified objects that we also have locally
        // (server can specify a subset of our local objects, and we
        //  ignore the rest)

        // this also automatically filters inRecords (the full price list)
        // for either houseObjects or tools, depending on what this picker
        // is displaying (because mLocalPresentIDs will only have one or
        //  the other)
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



char HouseObjectPicker::isInside( float inX, float inY ) {
    return fabs( inX ) < 1 &&
        fabs( inY ) < 1;
    }



void HouseObjectPicker::pointerMove( float inX, float inY ) {
    if( isInside( inX, inY ) ) {
        triggerToolTip();
        }
    }



void HouseObjectPicker::pointerDrag( float inX, float inY ) {
    pointerMove( inX, inY );
    }




