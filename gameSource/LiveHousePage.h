#ifndef LIVE_HOUSE_PAGE_INCLUDED
#define LIVE_HOUSE_PAGE_INCLUDED



#include "GamePage.h"




// base class for live house checkouts that handles pinging house on server
// to keep it checked out
class LiveHousePage : public GamePage {
        
    public:
        
        LiveHousePage();
        
        virtual ~LiveHousePage();
        
        virtual void step();

        virtual void makeActive( char inFresh );

    protected:
        
        int mWebRequest;

        // track this globally
        // User only has one house checked out at a time, but they
        // might move from Page to Page while having that same house checked
        // out.  
        static int sLastPingTime;


        // subclass should update this every time a user action happens
        int mLastActionTime;

    };



#endif
