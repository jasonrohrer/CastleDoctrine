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

        int mLastPingTime;


        // subclass should update this every time a user action happens
        int mLastActionTime;

    };



#endif
