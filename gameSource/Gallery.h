#ifndef GALLERY_INCLUDED
#define GALLERY_INCLUDED

#include "PageComponent.h"

#include "GallerySlotButton.h"

#include "GalleryArchive.h"


#define NUM_GALLERY_SLOTS   3


class  Gallery : public PageComponent, public ActionListener {
    public:
        
        Gallery( Font *inDisplayFont, double inX, double inY );
        

        ~Gallery();

        void setGalleryContents( const char *inGalleryContents );
        
        char *getGalleryContents();
        
        virtual void draw();
        
        void actionPerformed( GUIComponent *inTarget );

    protected:
        
        GallerySlotButton *mGallerySlots[ NUM_GALLERY_SLOTS ];

        GalleryArchive mGalleryArchive;
        

    };



#endif

                        
