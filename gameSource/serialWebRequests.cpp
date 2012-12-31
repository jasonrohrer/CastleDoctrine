
#include "serialWebRequests.h"

#include "minorGems/game/game.h"
#include "minorGems/util/SimpleVector.h"


typedef struct SerialWebRecord {
        int handle;
        
        char done;

        } SerialWebRecord;
        

static SimpleVector<SerialWebRecord> serialRecords;



int startWebRequestSerial( const char *inMethod, const char *inURL,
                           const char *inBody ) {
    if( true ) return startWebRequest( inMethod, inURL, inBody );

    SerialWebRecord r;

    r.handle = startWebRequest( inMethod, inURL, inBody );

    r.done = false;
    
    serialRecords.push_back( r );
    }



int stepWebRequestSerial( int inHandle ) {

    if( true ) return stepWebRequest( inHandle );
    

    int numRecords = serialRecords.size();
    
    for( int i=0; i<numRecords; i++ ) {
        
        SerialWebRecord *r = serialRecords.getElement( i );
        
        if( r->handle == inHandle ) {
            // found our request, just step it and return
            int stepResult = stepWebRequest( inHandle );

            if( stepResult != 0 ) {
                // not still processing (done or hit error)
                r->done = true;
                }

            return stepResult;
            }
        else {
            // not our request
            
            // is it a request that needs stepping?
            if( ! r->done ) {
                int stepResult = stepWebRequest( r->handle );

                if( stepResult != 0 ) {
                    // not still processing (done or hit error)
                    r->done = true;
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
    // don't need to deal with queue for this one
    return getWebResult( inHandle );
    }



void clearWebRequestSerial( int inHandle ) {
    if( true ) return clearWebRequest( inHandle );
    
    int numRecords = serialRecords.size();
    
    for( int i=0; i<numRecords; i++ ) {
        
        SerialWebRecord *r = serialRecords.getElement( i );
        
        if( r->handle == inHandle ) {
            serialRecords.deleteElement( i );
            clearWebRequestSerial( inHandle );
            return;
            }
        }
    
    // else not found in queue?
    }
