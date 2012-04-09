#ifndef HOUSE_OBJECT_PICKER_INCLUDED
#define HOUSE_OBJECT_PICKER_INCLUDED



#include "GamePage.h"
#include "PageComponent.h"

#include "SpriteButton.h"



typedef struct ObjectPriceRecord {
        int id;
        int price;
        } ObjectPriceRecord;


// fires actionPerformed when selection changes
class HouseObjectPicker : public PageComponent, public ActionListener, 
                          public ActionListenerList {
        
    public:
        
        
        HouseObjectPicker( double inX, double inY,
                           GamePage *inParentPage );
        

        virtual ~HouseObjectPicker();
        
        
        virtual void actionPerformed( GUIComponent *inTarget );
        
        
        virtual int getSelectedObject();
        
        // gets list of object IDs that are actually shown on picker
        // (doesn't included unplaceable objects)
        // destroyed by caller
        virtual int *getIDList( int *outNumIDs );
        
        
        // inRecords destroyed by caller
        // the records in this list determine what is shown on picker
        // (Can be a subset of what is returned by getIDList)
        // Thus, price list from server can override what objects are
        // shown in picker (by pairing down list only, not adding to it).
        virtual void setPrices( ObjectPriceRecord *inRecords, 
                                int inNumRecords );
        


        virtual void step();
        
        virtual void draw();
        


    protected:
        
        GamePage *mParentPage;


        SimpleVector<ObjectPriceRecord> mObjectList;
        // to block server-listed objects that we don't have locally
        SimpleVector<int> mLocalPresentIDs;
        
        int mSelectedIndex;
        
        double mPixWidth;

        SpriteButton mUpButton;
        SpriteButton mDownButton;


    };



#endif

        
