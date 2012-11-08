#include "LiveHousePage.h"

#include "SpriteButton.h"

#include "HouseObjectPicker.h"

#include "inventory.h"



#include "minorGems/ui/event/ActionListener.h"
#include "minorGems/util/SimpleVector.h"



#define NUM_SLOTS_PER_PICKER_GRID_ROW  4

#define NUM_PICKER_GRID_ROWS  3

#define NUM_SLOTS_PER_PICKER_GRID_PAGE ( NUM_PICKER_GRID_ROWS * \
                                         NUM_SLOTS_PER_PICKER_GRID_ROW )



class PickerGridPage : public LiveHousePage, public ActionListener {
        
    public:
        
        PickerGridPage( char inTools = false );
        
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
        
        
        HouseObjectPicker *mPickerPageSlots[ NUM_SLOTS_PER_PICKER_GRID_PAGE ];
        
        SpriteButton mUpButton;
        SpriteButton mDownButton;


        int mDisplayOffset;
        
        void setUpDownVisibility();
        void populateSlots();
        

    };

