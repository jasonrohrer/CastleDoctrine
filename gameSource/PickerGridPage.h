#include "LiveHousePage.h"

#include "SpriteButton.h"

#include "HouseObjectPicker.h"

#include "inventory.h"



#include "minorGems/ui/event/ActionListener.h"
#include "minorGems/util/SimpleVector.h"



#define PICKER_GRID_ROW  5

#define PICKER_GRID_NUM_ROWS  5

#define PICKER_GRID_PAGE ( PICKER_GRID_NUM_ROWS * PICKER_GRID_ROW )



class PickerGridPage : public LiveHousePage, public ActionListener {
        
    public:
        
        PickerGridPage();
        
        virtual ~PickerGridPage();
        
        // pulls object list and prices from picker
        void pullFromPicker( HouseObjectPicker *inPicker );
        
        void setLootValue( int inLootValue ) {
            mLootValue = inLootValue;
            }
        

        char getDone() {
            return mDone;
            }
        
        virtual int getSelectedObject();


        virtual void actionPerformed( GUIComponent *inTarget );
        
        
        virtual void draw( doublePair inViewCenter, 
                           double inViewSize );
        
        virtual void makeActive( char inFresh );



    protected:
        
        int mLootValue;

        char mDone;
        int mSelectedObject;
        

        SimpleVector<ObjectPriceRecord> mObjectList;
        
        
        HouseObjectPicker *mPickerPageSlots[ PICKER_GRID_PAGE ];
        

    };

