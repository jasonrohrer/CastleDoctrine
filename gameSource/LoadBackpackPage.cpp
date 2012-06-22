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
        : mToolPicker( 8, 5, true ),
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
        mPackSlots[i] = new InventorySlotButton( mainFont, 
                                                 slotCenter.x, slotCenter.y,
                                                 1 / 16.0 );
        slotCenter.x += 1.5;
        
        addComponent( mPackSlots[i] );
        mPackSlots[i]->addActionListener( this );
        }

    
    slotCenter.x = -8;
    slotCenter.y = 2;

    int numVaultRows = NUM_VAULT_SLOTS / NUM_PACK_SLOTS;
    
    int slot = 0;
    
    for( int r=0; r<numVaultRows; r++ ) {
        slotCenter.x = -8;

        for( int i=0; i<NUM_PACK_SLOTS; i++ ) {
            
            mVaultSlots[slot] = 
                new InventorySlotButton( mainFont, slotCenter.x, slotCenter.y,
                                         1 / 16.0 );
            slotCenter.x += 1.5;
        
            addComponent( mVaultSlots[slot] );
            mVaultSlots[slot]->addActionListener( this );
            slot ++;
            }
        slotCenter.y -= 1.5;
        }

    // for testing
    //mPackSlots[0]->setObject( 500 );
    //mPackSlots[2]->setObject( 501 );
    
    }


        
LoadBackpackPage::~LoadBackpackPage() {

    for( int i=0; i<NUM_PACK_SLOTS; i++ ) {
        delete mPackSlots[i];
        }

    for( int i=0; i<NUM_VAULT_SLOTS; i++ ) {
        delete mVaultSlots[i];
        }
    }



void LoadBackpackPage::setVaultContents( char *inVaultContents ) {
    for( int i=0; i<NUM_VAULT_SLOTS; i++ ) {
        // clear slots
        mVaultSlots[i]->setObject( -1 );
        }

    SimpleVector<QuantityRecord> tempRecords;
    
    fromString( inVaultContents, &tempRecords );
    
    for( int i=0; i<tempRecords.size(); i++ ) {
        QuantityRecord *r = tempRecords.getElement( i );
        
        mVaultSlots[i]->setObject( r->objectID );
        mVaultSlots[i]->setQuantity( r->quantity );
        }    
    }



char *LoadBackpackPage::getVaultContents() {
    SimpleVector<QuantityRecord> tempRecords;
    
    for( int i=0; i<NUM_VAULT_SLOTS; i++ ) {
        int id = mVaultSlots[i]->getObject();
        if( id != -1 ) {
            
            QuantityRecord r = { id,
                                 mVaultSlots[i]->getQuantity() };
            tempRecords.push_back( r );
            }
        }
    
    return toString( &tempRecords );
    }



void LoadBackpackPage::fromString( 
    char *inListString, 
    SimpleVector<QuantityRecord> *inEmptyList ) {
    
    // empty pack represented by "#"
    if( strcmp( inListString, "#" ) != 0 ) {
        int numParts;
        char **parts = split( inListString, "#", &numParts );

        for( int i=0; i<numParts; i++ ) {
            int id, quantity;
            int numRead = sscanf( parts[i], "%d:%d", &id, &quantity  );
                
            if( numRead == 2 ) {
                QuantityRecord r = { id, quantity };
                inEmptyList->push_back( r );
                }
            delete [] parts[i];
            }
        delete [] parts;
        }
    }



void LoadBackpackPage::setBackpackContents( char *inBackpackContents ) {
    for( int i=0; i<NUM_PACK_SLOTS; i++ ) {
        // clear slots
        mPackSlots[i]->setObject( -1 );
        }

    SimpleVector<QuantityRecord> tempRecords;
    
    fromString( inBackpackContents, &tempRecords );
    
    int numFilledSlots = 0;
        
    for( int i=0; i<tempRecords.size(); i++ ) {
        if( numFilledSlots < NUM_PACK_SLOTS ) {            
            QuantityRecord *r = tempRecords.getElement( i );
            
            for( int j=0; j<r->quantity; j++ ) {
                if( numFilledSlots < NUM_PACK_SLOTS ) {
                    mPackSlots[numFilledSlots]->setObject( r->objectID );
                    numFilledSlots++;
                    }
                }
            }
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




char *LoadBackpackPage::toString( 
    SimpleVector<QuantityRecord> *inQuantities ) {
    
    
    SimpleVector<char*> stringParts;
    for( int i=0; i<inQuantities->size(); i++ ) {
        
        QuantityRecord *r = inQuantities->getElement( i );
        
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




char *LoadBackpackPage::getBackpackContents() {

    SimpleVector<QuantityRecord> quantities;
    
    for( int i=0; i<NUM_PACK_SLOTS; i++ ) {
        int id = mPackSlots[i]->getObject();
        
        if( id != -1 ) {
            addToQuantity( &quantities, id );
            }
        }

    return toString( &quantities );
    }




char *LoadBackpackPage::getPurchaseList() {
    
    return toString( &mPurchaseRecords );
    }


void LoadBackpackPage::setPurchaseList( char *inPurchaseList ) {
    mPurchaseRecords.deleteAll();
    
    fromString( inPurchaseList, &mPurchaseRecords );
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
    else {
        
        // check if backpack slot clicked
        char foundHit = false;
        
        for( int i=0; i<NUM_PACK_SLOTS; i++ ) {
            if( inTarget == mPackSlots[i] ) {
                int id = mPackSlots[i]->getObject();
                
                if( id != -1 ) {
                    
                    mPackSlots[i]->setObject( -1 );
                    
                    char foundInVault = false;

                    for( int j=0; j<NUM_VAULT_SLOTS; j++ ) {
                        if( mVaultSlots[j]->getObject() == id ) {
                            mVaultSlots[j]->setQuantity( 
                                mVaultSlots[j]->getQuantity() + 1 );
                            
                            foundInVault = true;
                            break;
                            }
                        }
                    if( !foundInVault ) {
                        // find empty slot and stick it there
                        for( int j=0; j<NUM_VAULT_SLOTS; j++ ) {
                            if( mVaultSlots[j]->getObject() == -1 ) {
                                mVaultSlots[j]->setObject( id );
                                break;
                                }
                            }
                        }
                    
                    checkBuyButtonStatus();
                    }
                
                foundHit = true;
                break;
                }
            }
        
        if( !foundHit ) {
            // check for clicks of vault slots

            for( int i=0; i<NUM_VAULT_SLOTS; i++ ) {
                if( inTarget == mVaultSlots[i] ) {
                    int id = mVaultSlots[i]->getObject();
                
                    if( id != -1 ) {
                        
                        // but ignore them if backpack full
                        int emptyIndex = -1;
            
                        for( int j=0; j<NUM_PACK_SLOTS; j++ ) {
                            if( mPackSlots[j]->getObject() == -1 ) {
                                emptyIndex = j;
                                break;
                                }
                            }
                        
                        if( emptyIndex != -1 ) {

                            mVaultSlots[i]->setQuantity( 
                                mVaultSlots[i]->getQuantity() - 1 );
                            
                            mPackSlots[emptyIndex]->setObject( id );
                            
                            checkBuyButtonStatus();
                            }
                        }
                    break;
                    }
                }
            }

        
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
    
    drawMessage( "loadBackpackDescription", labelPos, false );



    setDrawColor( 1, 1, 1, 1 );

    labelPos = mPackSlots[0]->getCenter();
    
    labelPos.y += 1;
    labelPos.x -= 1;

    mainFont->drawString( translate( "backpackLabel" ), labelPos, alignLeft );
    
    
    labelPos = mVaultSlots[0]->getCenter();
    
    labelPos.y += 1;
    labelPos.x -= 1;

    mainFont->drawString( translate( "vaultLabel" ), labelPos, alignLeft );
    



    labelPos.x = 8;
    labelPos.y = 2;
    
    drawMessage( "editBalance", labelPos, false );
    
    labelPos.y = 1.25;

    char *balanceMessage = autoSprintf( "$%d", mLootValue );
    
    drawMessage( balanceMessage, labelPos, false );
    
    delete [] balanceMessage;
    }
