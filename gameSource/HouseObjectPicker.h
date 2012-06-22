#ifndef HOUSE_OBJECT_PICKER_INCLUDED
#define HOUSE_OBJECT_PICKER_INCLUDED



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
        
        // if inTools is true, only tools shown
        // else only house objects (non-tools) shown
        HouseObjectPicker( double inX, double inY,
                           char inTools = false );
        

        virtual ~HouseObjectPicker();
        
        
        virtual void actionPerformed( GUIComponent *inTarget );
        
        
        virtual int getSelectedObject();
        
        // gets list of object IDs that are actually shown on picker
        // (doesn't included unplaceable objects)
        // destroyed by caller
        virtual int *getIDList( int *outNumIDs );


        virtual void setPriceList( const char *inPriceList );
        
        
        // inRecords destroyed by caller
        // the records in this list determine what is shown on picker
        // (Can be a subset of what is returned by getIDList)
        // Thus, price list from server can override what objects are
        // shown in picker (by pairing down list only, not adding to it).
        virtual void setPrices( ObjectPriceRecord *inRecords, 
                                int inNumRecords );


        // returns -1 if no price set
        virtual int getPrice( int inObjectID );



        virtual void step();
        
        virtual void draw();
        


    protected:

        virtual void pointerMove( float inX, float inY );
        virtual void pointerDrag( float inX, float inY );

        char mShowTools;


        SimpleVector<ObjectPriceRecord> mObjectList;
        // to block server-listed objects that we don't have locally
        SimpleVector<int> mLocalPresentIDs;
        
        int mSelectedIndex;
        
        double mPixWidth;

        SpriteButton mUpButton;
        SpriteButton mDownButton;
        

        void triggerToolTip();
        
        char isInside( float inX, float inY );

    };



#endif

        
