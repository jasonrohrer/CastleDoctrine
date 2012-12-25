#include "LiveHousePage.h"

#include "TextField.h"
#include "TextButton.h"

#include "SpriteToggleButton.h"


#include "RobHouseGridDisplay.h"

#include "Gallery.h"

#include "inventory.h"



#include "minorGems/ui/event/ActionListener.h"




class RobHousePage : public LiveHousePage, public ActionListener {
        
    public:
        
        RobHousePage( const char *inDoneButtonKey = "doneRob" );
        
        virtual ~RobHousePage();
        

        // set to false to hide backpack buttons
        // defaults to showing them
        void showBackpack( char inShow );
        

        // destroyed by caller
        void setWifeName( const char *inWifeName );
        void setSonName( const char *inSonName );
        void setDaughterName( const char *inDaughterName );
        
        // destroyed by caller
        void setHouseMap( char *inHouseMap );
        char *getHouseMap();

        // destroyed by caller
        void setBackpackContents( char *inBackpackContents );
        char *getBackpackContents();

        void setGalleryContents( char *inGalleryContents );
        
        void setWifeMoney( int inMoney );

        void setMusicSeed( int inMusicSeed );

        char getSuccess() {
            return mGridDisplay.getSuccess();
            }

        char *getMoveList() {
            return mGridDisplay.getMoveList();
            }
        

        char getWifeKilled() {
            return mGridDisplay.getWifeKilled();
            }

        char getWifeRobbed() {
            return mGridDisplay.getWifeRobbed();
            }
        
        char getAnyFamilyKilled() {
            return mGridDisplay.getAnyFamilyKilled();
            }


        char getDone() {
            return mDone;
            }
            
        void setDescription( const char *inDescription );
        


        virtual void actionPerformed( GUIComponent *inTarget );


        virtual void step();
        
        virtual void draw( doublePair inViewCenter, 
                   double inViewSize );

        
        virtual void makeActive( char inFresh );

    protected:
        char mShowBackpack;

        RobHouseGridDisplay mGridDisplay;
        TextButton mDoneButton;
        
        SpriteToggleButton mMusicToggleButton;

        Gallery mGallery;
        
        int mMusicSeed;

        InventorySlotButton *mPackSlots[ NUM_PACK_SLOTS ];

        const char *mDoneButtonKey;
        
        char mDone;

        char *mDescription;
        
        char *mDeathMessage;
        
    };

