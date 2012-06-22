#include "LoadBackpackPage.h"

#include "message.h"
#include "tools.h"

#include "minorGems/game/Font.h"
#include "minorGems/game/game.h"
#include "minorGems/game/drawUtils.h"

#include "minorGems/util/stringUtils.h"


#include <time.h>


extern Font *mainFont;





LoadBackpackPage::LoadBackpackPage() 
        : mVaultContents( NULL ),
          // starts empty
          mPurchaseList( stringDuplicate( "#" ) ),
          mToolPicker( 8, 5, true ),
          mDoneButton( mainFont, 8, -5, translate( "doneEdit" ) ),
          mUndoButton( mainFont, 8, -1, translate( "undo" ), 'z', 'Z' ),
          mBuyButton( "left.tga", 5, 5, 1 / 16.0 ),
          mDone( false ) {

    addComponent( &mDoneButton );
    addComponent( &mUndoButton );
    addComponent( &mToolPicker );
    addComponent( &mBuyButton );

    mDoneButton.addActionListener( this );
    mUndoButton.addActionListener( this );
    mUndoButton.setVisible( false );
    mToolPicker.addActionListener( this );

    mBuyButton.addActionListener( this );
    mBuyButton.setVisible( false );
    mBuyButton.setMouseOverTip( translate( "buyTip" ) );
    
    doublePair slotCenter = { -8, 5 };

    for( int i=0; i<NUM_PACK_SLOTS; i++ ) {
        mPackSlots[i] = new InventorySlotButton( slotCenter.x, slotCenter.y,
                                                 1 / 16.0 );
        slotCenter.x += 1.5;
        
        addComponent( mPackSlots[i] );
        mPackSlots[i]->addActionListener( this );
        }
    // for testing
    //mPackSlots[0]->setObject( 500 );
    //mPackSlots[2]->setObject( 501 );
    
    }


        
LoadBackpackPage::~LoadBackpackPage() {
    
    if( mVaultContents != NULL ) {
        delete [] mVaultContents;
        }

    if( mPurchaseList != NULL ) {
        delete [] mPurchaseList;
        }

    for( int i=0; i<NUM_PACK_SLOTS; i++ ) {
        delete mPackSlots[i];
        }
    }



void LoadBackpackPage::setVaultContents( char *inVaultContents ) {
    if( mVaultContents != NULL ) {
        delete [] mVaultContents;
        }
    mVaultContents = stringDuplicate( inVaultContents );
    }



char *LoadBackpackPage::getVaultContents() {
    return stringDuplicate( mVaultContents );
    }



void LoadBackpackPage::setBackpackContents( char *inBackpackContents ) {
    for( int i=0; i<NUM_PACK_SLOTS; i++ ) {
        // clear slots
        mPackSlots[i]->setObject( -1 );
        }

    // empty pack represented by "#"
    if( strcmp( inBackpackContents, "#" ) != 0 ) {
        int numParts;
        char **parts = split( inBackpackContents, "#", &numParts );

        int numFilledSlots = 0;
        
        for( int i=0; i<numParts; i++ ) {
            if( numFilledSlots < NUM_PACK_SLOTS ) {            
                int id, quantity;
                int numRead = sscanf( parts[i], "%d:%d", &id, &quantity  );
                
                if( numRead == 2 ) {
                    for( int j=0; j<quantity; j++ ) {
                        if( numFilledSlots < NUM_PACK_SLOTS ) {
                            mPackSlots[numFilledSlots]->setObject( id );
                            numFilledSlots++;
                            }
                        }
                    }
                }
            delete [] parts[i];
            }
        delete [] parts;
        }
    

    checkBuyButtonStatus();
    }



void LoadBackpackPage::addToQuantity( 
    SimpleVector<QuantityRecord> *inOldQuanties, int inAddObjectID ) {
    
    char found = false;
    for( int j=0; j<inOldQuanties->size(); j++ ) {
        QuantityRecord *r = inOldQuanties->getElement( j );
        
        if( r->objectID == inAddObjectID ) {
            r->quantity ++;
            found = true;
            break;
            }
                    }
    
    if( !found ) {
        // new record for this object ID
        
        QuantityRecord r = { inAddObjectID, 1 };
        inOldQuanties->push_back( r );
        }
    }




char *LoadBackpackPage::getBackpackContents() {

    SimpleVector<QuantityRecord> quantities;
    
    

    for( int i=0; i<NUM_PACK_SLOTS; i++ ) {
        int id = mPackSlots[i]->getObject();
        
        if( id != -1 ) {
            addToQuantity( &quantities, id );
            }
        }

    SimpleVector<char*> stringParts;
    for( int i=0; i<quantities.size(); i++ ) {
        
        QuantityRecord *r = quantities.getElement( i );
        
        char *part = autoSprintf( "%d:%d", r->objectID, r->quantity );
            
        stringParts.push_back( part );
        }


    if( stringParts.size() > 0 ) {
        
        char **partArray = stringParts.getElementArray();
    
        char *contentsString = join( partArray, stringParts.size(), "#" );
        
        delete [] partArray;
        
        for( int i=0; i<stringParts.size(); i++ ) {
            delete [] *( stringParts.getElement( i ) );
            }
        
        return contentsString;
        }
    else {
        // empty pack represented by "#"
        return stringDuplicate( "#" );
        }
    }




char *LoadBackpackPage::getPurchaseList() {
    return stringDuplicate( mPurchaseList );
    }


void LoadBackpackPage::setPurchaseList( char *inPurchaseList ) {
    if( mPurchaseList != NULL ) {
        delete [] mPurchaseList;
        }
    mPurchaseList = stringDuplicate( inPurchaseList );
    }



void LoadBackpackPage::setPriceList( char *inPriceList ) {
    mToolPicker.setPriceList( inPriceList );
    }



void LoadBackpackPage::setLootValue( int inLootValue ) {
    mLootValue = inLootValue;
    }



void LoadBackpackPage::checkBuyButtonStatus() {
    int selectedObject = mToolPicker.getSelectedObject();
        
    int price = mToolPicker.getPrice( selectedObject );
    
    if( price > mLootValue ) {
        mBuyButton.setVisible( false );
        return;
        }
    
    // we can afford it

    for( int i=0; i<NUM_PACK_SLOTS; i++ ) {
        if( mPackSlots[i]->getObject() == -1 ) {
            // empty slot found
            mBuyButton.setVisible( true );
            return;
            }
        }
    // no empty slots
    mBuyButton.setVisible( false );
    }





void LoadBackpackPage::actionPerformed( GUIComponent *inTarget ) {
    if( inTarget == &mDoneButton ) {
        mDone = true;
        }
    else if( inTarget == &mToolPicker ) {
        checkBuyButtonStatus();
        }
    else if( inTarget == &mUndoButton ) {
        // figure out how undo makes sense here
        
        }
    else if( inTarget == &mBuyButton ) {
        int selectedObject = mToolPicker.getSelectedObject();
        
        int price = mToolPicker.getPrice( selectedObject );

        for( int i=0; i<NUM_PACK_SLOTS; i++ ) {
            if( mPackSlots[i]->getObject() == -1 ) {
                // empty slot!
                mPackSlots[i]->setObject( selectedObject );
                mLootValue -= price;
                

                addToQuantity( &mPurchaseRecords, selectedObject );
                break;
                }
            }
        
        checkBuyButtonStatus();
        }
    
    
    }





        
void LoadBackpackPage::makeActive( char inFresh ) {
    LiveHousePage::makeActive( inFresh );
    
    if( !inFresh ) {
        return;
        }
    
    mDone = false;
    }
        



void LoadBackpackPage::draw( doublePair inViewCenter, 
                             double inViewSize ) {
        
    doublePair labelPos = { 0, 6.75 };
    
    drawMessage( translate( "loadBackpackDescription" ), labelPos, false );
    


    labelPos.x = 8;
    labelPos.y = 2;
    
    drawMessage( translate( "editBalance" ), labelPos, false );
    
    labelPos.y = 1.25;

    char *balanceMessage = autoSprintf( "$%d", mLootValue );
    
    drawMessage( balanceMessage, labelPos, false );
    
    delete [] balanceMessage;
    }
