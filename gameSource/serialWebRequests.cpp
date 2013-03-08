
#include "serialWebRequests.h"

#include "minorGems/game/game.h"
#include "minorGems/util/SimpleVector.h"
#include "minorGems/util/stringUtils.h"

#include "ticketHash.h"



extern int webRetrySeconds;

static int nextHandle = 0;


// can be used externally to follow current request
int currentActiveSerialWebRequest = -1;



typedef struct SerialWebRecord {
        // for finding request externally
        int handle;
        
        // handle for the active web request
        int activeHandle;
        

        char done;
        
        // false until first step done
        // don't start timing until we're actually running the request
        // (don't want to retry a request early because slow requests were
        //  queued in front of it).
        char started;
        
        time_t startTime;
        
        int retryCount;

        // save for retry
        char *method;
        char *url;
        char *body;
        
        } SerialWebRecord;
        

static SimpleVector<SerialWebRecord> serialRecords;


static char serverShutdown = false;




int startWebRequestSerial( const char *inMethod, const char *inURL,
                           const char *inBody ) {
    //if( true ) return startWebRequest( inMethod, inURL, inBody );

    SerialWebRecord r;

    r.method = NULL;
    r.url = NULL;
    r.body = NULL;
    

    if( inMethod != NULL ) {
        r.method = stringDuplicate( inMethod );
        }
    if( inURL != NULL ) {
        r.url = stringDuplicate( inURL );
        }
    if( inBody != NULL ) {
        r.body = stringDuplicate( inBody );
        }
        

    r.activeHandle = startWebRequest( r.method, r.url, r.body );
    
    r.handle = nextHandle;
    nextHandle ++;
    
    r.done = false;

    r.started = false;
    
    r.retryCount = 0;

    serialRecords.push_back( r );

    printf( "Starting web request %d\n", r.activeHandle );
    
    return r.handle;
    }



void checkForServerShutdown( SerialWebRecord *inR ) {
    char *result = getWebResult( inR->activeHandle );

    if( result != NULL ) {
        
        if( strstr( result, "SHUTDOWN" ) != NULL ) {
            serverShutdown = true;
            }

        delete [] result;
        }
    }


static void checkForRequestRetry( SerialWebRecord *r ) {
    time_t timePassed = game_time( NULL ) - r->startTime;

    if( timePassed > webRetrySeconds ) {
        // start a fresh request

        clearWebRequest( r->activeHandle );

        r->url = replaceTicketHash( r->url );
        r->body = replaceTicketHash( r->body );

        r->activeHandle = startWebRequest( r->method, r->url, r->body );

        r->startTime = game_time( NULL );
        r->retryCount++;
        }
    
    }




int stepWebRequestSerial( int inHandle ) {

    //if( true ) return stepWebRequest( inHandle );
    

    int numRecords = serialRecords.size();
    
    for( int i=0; i<numRecords; i++ ) {
        
        SerialWebRecord *r = serialRecords.getElement( i );
        
        if( r->handle == inHandle ) {
            
            if( ! r->started ) {                
                r->startTime = game_time( NULL );
                r->started = true;
                
                currentActiveSerialWebRequest = r->handle;
                }            

            // found our request, just step it and return
            int stepResult = stepWebRequest( r->activeHandle );

            if( stepResult != 0 ) {
                // not still processing (done or hit error)
                r->done = true;
                
                currentActiveSerialWebRequest = -1;
                
                if( stepResult == 1 ) {
                    checkForServerShutdown( r );
                    
                    if( serverShutdown ) {
                        // block client's normal response to this
                        // request, by reporting that it's still in
                        // progress, so that client shutdown behavior
                        // can take over
                        return 0;
                        }
                    }
                }
            else {
                checkForRequestRetry( r );
                }
            

            return stepResult;
            }
        else {
            // not our request
            
            // is it a request that needs stepping?
            if( ! r->done ) {
                
                if( ! r->started ) {                
                    r->startTime = game_time( NULL );
                    r->started = true;
                    
                    currentActiveSerialWebRequest = r->handle;
                    }

                int stepResult = stepWebRequest( r->activeHandle );

                if( stepResult != 0 ) {
                    // not still processing (done or hit error)
                    r->done = true;
                    
                    currentActiveSerialWebRequest = -1;

                    if( stepResult == 1 ) {
                        checkForServerShutdown( r );
                        
                        if( serverShutdown ) {
                            // block client's normal response to this
                            // request, by reporting that it's still in
                            // progress, so that client shutdown behavior
                            // can take over
                            return 0;
                            }
                        }
                    }
                else {
                    checkForRequestRetry( r );
                    }
                
                // OUR request not done yet, because we're still stepping
                // requests that come before in the queue
                return 0;
                }
            // else this request is in the queue, but it's done, so move
            // past it to the next request and check that
            }
        }
    

    // if we got here, we haven't found our request at all.... error?
    return -1;
    }



char *getWebResultSerial( int inHandle ) {
    int numRecords = serialRecords.size();

    for( int i=0; i<numRecords; i++ ) {
        
        SerialWebRecord *r = serialRecords.getElement( i );
        
        if( r->handle == inHandle ) {
            return getWebResult( r->activeHandle );
            }
        }
    
    return NULL;
    }




int getWebRequestRetryStatus( int inHandle ) {
    int numRecords = serialRecords.size();
    
    for( int i=0; i<numRecords; i++ ) {
        
        SerialWebRecord *r = serialRecords.getElement( i );
        
        if( r->handle == inHandle ) {
            
            if( ! r->started ) {
                return 0;
                }

            if( r->retryCount < 1 ) {
                
                time_t timePassed = game_time( NULL ) - r->startTime;
                if( timePassed < webRetrySeconds / 2 ) {
                    return 0;
                    }
                else {
                    return 1;
                    }
                }
            else {
                return 1 + r->retryCount;
                }
            }
        }

    // not found?
    return 0;
    }



void clearWebRequestSerial( int inHandle ) {
    //if( true ) return clearWebRequest( inHandle );
    
    int numRecords = serialRecords.size();
    
    for( int i=0; i<numRecords; i++ ) {
        
        SerialWebRecord *r = serialRecords.getElement( i );
        
        if( r->handle == inHandle ) {
            clearWebRequest( r->activeHandle );
            
            if( r->method != NULL ) {
                delete r->method;
                }
            if( r->url != NULL ) {
                delete r->url;
                }
            if( r->body != NULL ) {
                delete r->body;
                }
            
            serialRecords.deleteElement( i );
            return;
            }
        }
    
    // else not found in queue?
    }



char getServerShutdown() {
    return serverShutdown;
    }

