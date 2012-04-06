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

        
        // fetch new prices from server
        virtual void refreshPrices();
        
        
        virtual int getSelectedObject();
        

        virtual void step();
        
        virtual void draw();
        


    protected:
        
        GamePage *mParentPage;


        int mWebRequest;

        SimpleVector<ObjectPriceRecord> mObjectList;
        
        int mSelectedIndex;
        
        double mPixWidth;

        SpriteButton mUpButton;
        SpriteButton mDownButton;


    };



#endif

        
