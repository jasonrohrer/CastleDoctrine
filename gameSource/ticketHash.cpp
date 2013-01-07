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

    char *toHash = autoSprintf( "%d", serverSequenceNumber );
    
    char *hash = hmac_sha1( codeToHash, toHash );
    
    delete [] toHash;

    char *result = autoSprintf( "sequence_number=%d&ticket_hmac=%s",
                                serverSequenceNumber,
                                hash );
    delete [] hash;

    serverSequenceNumber++;
    
    return result;
    }

