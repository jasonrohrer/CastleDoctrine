#include "LoadBackpackPage.h"

#include "message.h"
#include "tools.h"

#include "minorGems/game/Font.h"
#include "minorGems/game/game.h"
#include "minorGems/game/drawUtils.h"

#include "minorGems/util/stringUtils.h"




extern Font *mainFont;





LoadBackpackPage::LoadBackpackPage() 
        : mSellMode( false ),
          mToolPicker( 8, 5, true ),
          mDoneButton( mainFont, 8, -5, translate( "doneEdit" ) ),
          mSellModeButton( mainFont, 8, -3, translate( "sellMode" ) ),
          mBuyModeButton( mainFont, 8, -2, translate( "buyMode" ) ),
          mUndoButton( mainFont, 8, -0.5, translate( "undo" ), 'z', 'Z' ),
          mBuyButton( "left.tga", 5, 5, 1 / 16.0 ),
          mDone( false ) {

    addComponent( &mDoneButton );
    addComponent( &mUndoButton );
    addComponent( &mToolPicker );
    addComponent( &mBuyButton );

    addComponent( &mSellModeButton );
    addComponent( &mBuyModeButton );
    
    mSellModeButton.setMouseOverTip( translate( "sellModeTip" ) );
    mBuyModeButton.setMouseOverTip( translate( "buyModeTip" ) );
    

    mDoneButton.setMouseOverTip( "" );

    mDoneButton.addActionListener( this );
    mUndoButton.addActionListener( this );
    mUndoButton.setVisible( false );
    
    mSellModeButton.addActionListener( this );
    mBuyModeButton.addActionListener( this );
    mBuyModeButton.setVisible( false );


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
    

    checkSellModeStatus();
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



char *LoadBackpackPage::getSellList() {
    
    return toString( &mSellRecords );
    }


void LoadBackpackPage::setSellList( char *inSellList ) {
    mSellRecords.deleteAll();
    
    fromString( inSellList, &mSellRecords );

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
    
    if( mSellMode ) {
        mBuyButton.setVisible( false );
        return;
        }

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
        
        const char *tipKey = "backpackUndoTip";
        
        if( lastBuy < 0 ) {
            // last buy was a sell
            
            // convert negative ID into a valid, positive ID
            lastBuy = -lastBuy;

            tipKey = "backpackSellUndoTip";
            }
        
        char *tip = autoSprintf( translate( tipKey ),
                                 getToolDescription( lastBuy ) );
        
        mUndoButton.setMouseOverTip( tip );
        delete [] tip;
        }
    }



void LoadBackpackPage::checkSellModeStatus() {
    mSellModeButton.setVisible( !mSellMode );
    mBuyModeButton.setVisible( mSellMode );
    
    int packTransferMode = 1;
    int vaultTransferMode = 2;
    
    if( mSellMode ) {
        packTransferMode = 3;
        vaultTransferMode = 3;
        }
    for( int i=0; i<NUM_PACK_SLOTS; i++ ) {
        mPackSlots[i]->setTransferStatus( packTransferMode );
        int id = mPackSlots[i]->getObject();
        
        if( id != -1 ) {
            mPackSlots[i]->setSellPrice( mToolPicker.getSellBackPrice( id ) );
            }
        }
    for( int i=0; i<NUM_VAULT_SLOTS; i++ ) {
        mVaultSlots[i]->setTransferStatus( vaultTransferMode );
        int id = mVaultSlots[i]->getObject();
        
        if( id != -1 ) {
            mVaultSlots[i]->setSellPrice( mToolPicker.getSellBackPrice( id ) );
            }
        }
    


    checkBuyButtonStatus();
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

            if( idToUnbuy < 0 ) {
                // undoing a sale instead (negative ID number)
                
                int idToUnsell = -idToUnbuy;
                
                subtractFromQuantity( &mSellRecords, idToUnsell );
            
                mLootValue -= mToolPicker.getSellBackPrice( idToUnsell );

                // stick undone sales back in vault
                char found = false;
                
                // slot already contains these items?
                for( int i=0; i<NUM_VAULT_SLOTS; i++ ) {
                    if( mVaultSlots[i]->getObject() == idToUnsell ) {
                        mVaultSlots[i]->setQuantity( 
                            mVaultSlots[i]->getQuantity() + 1 );
                        found = true;
                        break;
                        }
                    }
                if( ! found ) {
                    // stick in an empty slot
                    for( int i=0; i<NUM_VAULT_SLOTS; i++ ) {
                        if( mVaultSlots[i]->getObject() == -1 ) {
                            mVaultSlots[i]->setObject( idToUnsell );
                            mVaultSlots[i]->setSellPrice( 
                                mToolPicker.getSellBackPrice( idToUnsell ) );
                            found = true;
                            break;
                            }
                        }
                    }       
                }
            else {
                // undoing a purchase
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
            }
        checkBuyButtonStatus();
        
        checkUndoStatus();

        actionHappened();
        }
    else if( inTarget == &mBuyButton ) {
        int selectedObject = mToolPicker.getSelectedObject();
        
        int price = mToolPicker.getPrice( selectedObject );
        int sellPrice = mToolPicker.getSellBackPrice( selectedObject );;
        for( int i=0; i<NUM_PACK_SLOTS; i++ ) {
            if( mPackSlots[i]->getObject() == -1 ) {
                // empty slot!
                mPackSlots[i]->setObject( selectedObject );
                mPackSlots[i]->setSellPrice( sellPrice );
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
    else if( inTarget == &mSellModeButton ) {
        mSellMode = true;
        checkSellModeStatus();
        }
    else if( inTarget == &mBuyModeButton ) {
        mSellMode = false;
        checkSellModeStatus();
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
                    
                    if( mSellMode ) {

                        mLootValue += mToolPicker.getSellBackPrice( id );
                        
                        addToQuantity( &mSellRecords, id );
                        mPurchaseHistory.push_back( -id );
                        
                        checkUndoStatus();
                        }
                    else {
                        
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
                                    mVaultSlots[j]->setSellPrice(
                                        mToolPicker.getSellBackPrice( id ) );
                                    break;
                                    }
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
                        
                        if( mSellMode ) {
                            mVaultSlots[i]->setQuantity( 
                                    mVaultSlots[i]->getQuantity() - 1 );
                            
                            mLootValue += mToolPicker.getSellBackPrice( id );
                        
                            addToQuantity( &mSellRecords, id );
                            mPurchaseHistory.push_back( -id );
                        
                            checkUndoStatus();
                            }
                        else {
                            // ignore transfer click if backpack full
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
                                mPackSlots[emptyIndex]->setSellPrice(
                                    mToolPicker.getSellBackPrice( id ) );
                                
                                checkBuyButtonStatus();
                                }
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
    mSellMode = false;
    checkSellModeStatus();
    }
        



void LoadBackpackPage::draw( doublePair inViewCenter, 
                             double inViewSize ) {
        
    doublePair labelPos = { 0, 6.75 };

    if( mSellMode ) {    
        drawMessage( "loadBackpackSellModeDescription", labelPos, false );
        }    
    else {
        drawMessage( "loadBackpackDescription", labelPos, false );
        }
    


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
