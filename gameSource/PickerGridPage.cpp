#include "PickerGridPage.h"

#include "message.h"

#include "minorGems/util/stringUtils.h"



PickerGridPage::PickerGridPage( char inTools ) 
        : mUpButton( "up.tga", 7, 1, 1/16.0 ),
          mDownButton( "down.tga", 7, -1, 1/16.0 ),
          mDisplayOffset( 0 )  {
    
    doublePair pos = {-4.5, 5};
    
    
    int slot = 0;
    
    for( int y = 0; y<NUM_PICKER_GRID_ROWS; y++ ) {
        pos.x = -4.5;
        
        for( int x = 0; x<NUM_SLOTS_PER_PICKER_GRID_ROW; x++ ) {
        
            mPickerPageSlots[slot] = new HouseObjectPicker( pos.x, pos.y,
                                                            inTools );
            
            addComponent( mPickerPageSlots[slot] );
            mPickerPageSlots[slot]->addActionListener( this );

            slot++;
            pos.x += 3;
            }
        pos.y -= 3.5;
        }
    
    
    // up/down button to right of top/bottom rows
    doublePair upPosition = 
        mPickerPageSlots[ NUM_SLOTS_PER_PICKER_GRID_ROW - 1 ]->getPosition();
    doublePair downPosition = 
        mPickerPageSlots[ NUM_SLOTS_PER_PICKER_GRID_PAGE - 1 ]->getPosition();
    
    upPosition.x += 2;
    downPosition.x += 2;
    
    mUpButton.setPosition( upPosition.x, upPosition.y );
    mDownButton.setPosition( downPosition.x, downPosition.y );
    
    addComponent( &mUpButton );
    addComponent( &mDownButton );
    
    mUpButton.addActionListener( this );
    mDownButton.addActionListener( this );
    }



PickerGridPage::~PickerGridPage() {
    for( int i=0; i<NUM_SLOTS_PER_PICKER_GRID_PAGE; i++ ) {
        delete mPickerPageSlots[i];
        }
    }
        


void PickerGridPage::pullFromPicker( HouseObjectPicker *inPicker ) {
    mObjectList.deleteAll();
    
    int numRecords;
    ObjectPriceRecord *records = inPicker->getPrices( &numRecords );


    mObjectList.push_back( records, numRecords );
    
    populateSlots();
    
    delete [] records;
    }


                
int PickerGridPage::getSelectedObject() {
    return mSelectedObject;
    }



void PickerGridPage::actionPerformed( GUIComponent *inTarget ) {
    if( inTarget == &mUpButton ) {
        mDisplayOffset -= NUM_SLOTS_PER_PICKER_GRID_PAGE;
        populateSlots();
        return;
        }
    else if( inTarget == &mDownButton ) {
        mDisplayOffset += NUM_SLOTS_PER_PICKER_GRID_PAGE;
        populateSlots();
        return;
        }
        
    for( int i=0; i<NUM_SLOTS_PER_PICKER_GRID_PAGE; i++ ) {
        if( inTarget == mPickerPageSlots[i] ) {
            // found
            
            mSelectedObject = mPickerPageSlots[i]->getSelectedObject();
            mDone = true;
            break;
            }
        }
    }



void PickerGridPage::makeActive( char inFresh ) {
    mDone = false;
    }




void PickerGridPage::draw( doublePair inViewCenter, 
                          double inViewSize ) {
    
    doublePair labelPos = { 8, 2 };
        
    drawMessage( "editBalance", labelPos, false );
    
    labelPos.y = 1.25;

    char *balanceMessage = autoSprintf( "$%d", mLootValue );
    
    drawMessage( balanceMessage, labelPos, false );
    
    delete [] balanceMessage;
    }



void PickerGridPage::setUpDownVisibility() {
    mUpButton.setVisible( mDisplayOffset > 0 );
    
    mDownButton.setVisible( mObjectList.size() - mDisplayOffset > 
                            NUM_SLOTS_PER_PICKER_GRID_PAGE );
    }




void PickerGridPage::populateSlots() {
    
    for( int i=0; i<NUM_SLOTS_PER_PICKER_GRID_PAGE; i++ ) {
        mPickerPageSlots[i]->setVisible( false );
        }

    int slotNumber = 0;
    for( int i=mDisplayOffset; 
         i<mObjectList.size() && slotNumber < NUM_SLOTS_PER_PICKER_GRID_PAGE; 
         i++ ) {
        
        mPickerPageSlots[slotNumber]->setPrices( 
            mObjectList.getElement( i ), 1 );
        
        mPickerPageSlots[slotNumber]->setVisible( true );

        slotNumber++;
        }
    setUpDownVisibility();
    }
