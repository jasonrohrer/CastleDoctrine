#include "ticketHash.h"

#include "minorGems/util/stringUtils.h"
#include "minorGems/crypto/hashes/sha1.h"


extern char *downloadCode;
extern int serverSequenceNumber;


char *getTicketHash() {

    const char *codeToHash = "";

    if( downloadCode != NULL ) {
        codeToHash = downloadCode;
        }

    
    // strip out "-" separators
    int numParts;
    char **codeParts = split( codeToHash, "-", &numParts );
    
    char *pureCode =
        join( codeParts, numParts, "" );
    
    for( int i=0; i<numParts; i++ ) {
        delete [] codeParts[i];
        }
    delete [] codeParts;



    char *toHash = autoSprintf( "%d", serverSequenceNumber );
    
    char *hash = hmac_sha1( pureCode, toHash );
    
    delete [] pureCode;
    delete [] toHash;

    char *result = autoSprintf( "sequence_number=%d&ticket_hmac=%s",
                                serverSequenceNumber,
                                hash );
    delete [] hash;

    serverSequenceNumber++;
    
    return result;
    }

