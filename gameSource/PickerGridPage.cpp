#include "PickerGridPage.h"

#include "message.h"

#include "minorGems/util/stringUtils.h"



PickerGridPage::PickerGridPage() {
    doublePair pos = {-8, 5};
    
    
    int slot = 0;
    
    for( int y = 0; y<PICKER_GRID_NUM_ROWS; y++ ) {
        pos.x = -8;
        
        for( int x = 0; x<PICKER_GRID_ROW; x++ ) {
        
            mPickerPageSlots[slot] = new HouseObjectPicker( pos.x, pos.y );
            
            addComponent( mPickerPageSlots[slot] );
            mPickerPageSlots[slot]->addActionListener( this );

            slot++;
            pos.x += 3;
            }
        pos.y -= 3;
        }
    
    }



PickerGridPage::~PickerGridPage() {
    for( int i=0; i<PICKER_GRID_PAGE; i++ ) {
        delete mPickerPageSlots[i];
        }
    }
        


void PickerGridPage::pullFromPicker( HouseObjectPicker *inPicker ) {
    mObjectList.deleteAll();
    
    int numRecords;
    ObjectPriceRecord *records = inPicker->getPrices( &numRecords );
    
    for( int i=0; i<PICKER_GRID_PAGE; i++ ) {
        mPickerPageSlots[i]->setVisible( false );
        }
    

    for( int i=0; i<numRecords; i++ ) {
        mObjectList.push_back( records[i] );
        
        if( i < PICKER_GRID_PAGE ) {
            // each slot is a picker with one item in it
            mPickerPageSlots[i]->setPrices( &( records[i] ), 1 );
            mPickerPageSlots[i]->setVisible( true );
            }
        }
    delete [] records;
    }

                
int PickerGridPage::getSelectedObject() {
    return mSelectedObject;
    }


void PickerGridPage::actionPerformed( GUIComponent *inTarget ) {
    for( int i=0; i<PICKER_GRID_PAGE; i++ ) {
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
