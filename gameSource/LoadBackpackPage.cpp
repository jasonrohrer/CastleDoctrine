#include "LoadBackpackPage.h"

#include "message.h"
#include "tools.h"

#include "minorGems/game/Font.h"
#include "minorGems/game/game.h"
#include "minorGems/game/drawUtils.h"

#include "minorGems/util/stringUtils.h"




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

    mDoneButton.setMouseOverTip( "" );

    mDoneButton.addActionListener( this );
    mUndoButton.addActionListener( this );
    mUndoButton.setVisible( false );
    
    mToolPicker.addActionListener( this );


    mBuyButton.addActionListener( this );
    mBuyButton.setVisible( false );
    
    doublePair slotCenter = { -8, 5 };

    for( int i=0; i<NUM_PACK_SLOTS; i++ ) {
        mPackSlots[i] = new InventorySlotButton( mainFont, 
                                                 slotCenter.x, slotCenter.y,
                                                 1 / 32.0 );
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
                                         1 / 32.0 );
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
    vaultSlotsFromString( inVaultContents, mVaultSlots );
    }



char *LoadBackpackPage::getVaultContents() {

    return stringFromInventorySlots( mVaultSlots, NUM_VAULT_SLOTS );
    }



void LoadBackpackPage::setBackpackContents( char *inBackpackContents ) {

    backpackSlotsFromString( inBackpackContents,
                             mPackSlots );
       
    checkBuyButtonStatus();
    }



char *LoadBackpackPage::getBackpackContents() {

    return stringFromInventorySlots( mPackSlots, NUM_PACK_SLOTS );
    }




char *LoadBackpackPage::getPurchaseList() {
    
    return toString( &mPurchaseRecords );
    }


void LoadBackpackPage::setPurchaseList( char *inPurchaseList ) {
    mPurchaseRecords.deleteAll();
    
    fromString( inPurchaseList, &mPurchaseRecords );

    // this is a fresh loading session
    // undoing old purchase history here would be confusing (and sometimes
    // incorrect)
    mPurchaseHistory.deleteAll();
    mUndoButton.setVisible( false );
    }



void LoadBackpackPage::setPriceList( char *inPriceList ) {
    mToolPicker.setPriceList( inPriceList );
       
    checkBuyButtonStatus();
    }



void LoadBackpackPage::setLootValue( int inLootValue ) {
    mLootValue = inLootValue;

    checkBuyButtonStatus();
    }



void LoadBackpackPage::checkBuyButtonStatus() {
    int selectedObject = mToolPicker.getSelectedObject();
    
    if( selectedObject == -1 ) {
        mBuyButton.setVisible( false );
        return;
        }

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

            char *tip = autoSprintf( translate( "buyTip" ),
                                     getToolDescription( selectedObject ) );
        
            mBuyButton.setMouseOverTip( tip );
            delete [] tip;                

            return;
            }
        }
    // no empty slots
    mBuyButton.setVisible( false );
    }



void LoadBackpackPage::checkUndoStatus() {
            
    if( mPurchaseHistory.size() == 0 ) {
        // history exhausted
        mUndoButton.setVisible( false );
        }
    else {
        mUndoButton.setVisible( true );
        
        int lastBuy = 
            *( mPurchaseHistory.getElement( mPurchaseHistory.size() - 1 ) );
        

        char *tip = autoSprintf( translate( "backpackUndoTip" ),
                                 getToolDescription( lastBuy ) );
        
        mUndoButton.setMouseOverTip( tip );
        delete [] tip;
        }
    }

        
                


void LoadBackpackPage::actionPerformed( GUIComponent *inTarget ) {
    if( inTarget == &mDoneButton ) {
        mDone = true;
        }
    else if( inTarget == &mToolPicker ) {
        if( mToolPicker.shouldShowGridView() ) {
            mShowGridToolPicker = true;
            }
        else {
            checkBuyButtonStatus();
            }
        }
    else if( inTarget == &mUndoButton ) {
        if( mPurchaseHistory.size() > 0 ) {
            // undo last one
            int idToUnbuy = 
                *( mPurchaseHistory.getElement( 
                       mPurchaseHistory.size() - 1 ) );
            mPurchaseHistory.deleteElement( mPurchaseHistory.size() - 1 );

            subtractFromQuantity( &mPurchaseRecords, idToUnbuy );
            
            mLootValue += mToolPicker.getPrice( idToUnbuy );

            // find in backpack?
            char found = false;
            for( int i=NUM_PACK_SLOTS - 1; i>=0; i-- ) {
                if( mPackSlots[i]->getObject() == idToUnbuy ) {
                    mPackSlots[i]->setObject( -1 );
                    found = true;
                    break;
                    }
                }
            
            if( !found ) {
                // check vault
                for( int i=0; i<NUM_VAULT_SLOTS; i++ ) {
                    if( mVaultSlots[i]->getObject() == idToUnbuy ) {
                        mVaultSlots[i]->setQuantity( 
                            mVaultSlots[i]->getQuantity() - 1 );
                        found = true;
                        break;
                        }
                    }
                }
            }
        checkBuyButtonStatus();
        
        checkUndoStatus();

        actionHappened();
        }
    else if( inTarget == &mBuyButton ) {
        int selectedObject = mToolPicker.getSelectedObject();
        
        int price = mToolPicker.getPrice( selectedObject );

        for( int i=0; i<NUM_PACK_SLOTS; i++ ) {
            if( mPackSlots[i]->getObject() == -1 ) {
                // empty slot!
                mPackSlots[i]->setObject( selectedObject );
                mLootValue -= price;
                
                mToolPicker.useSelectedObject();

                addToQuantity( &mPurchaseRecords, selectedObject );
                mPurchaseHistory.push_back( selectedObject );
                
                checkUndoStatus();

                break;
                }
            }
        
        checkBuyButtonStatus();
        actionHappened();
        }
    else {
        
        // check if backpack slot clicked
        char foundHit = false;
        
        for( int i=0; i<NUM_PACK_SLOTS; i++ ) {
            if( inTarget == mPackSlots[i] ) {
                int id = mPackSlots[i]->getObject();
                
                if( id != -1 ) {
                    // click on a non-empty pack slot counts as an action
                    actionHappened();

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
                        // click on a non-empty vault slot counts as an action
                        actionHappened();

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
    mShowGridToolPicker = false;
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
