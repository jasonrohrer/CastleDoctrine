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

    char *toHash = autoSprintf( "%s%d", codeToHash, serverSequenceNumber );
    
    char *hash = computeSHA1Digest( toHash );
    
    delete [] toHash;

    char *result = autoSprintf( "sequence_number=%d&ticket_hash=%s",
                                serverSequenceNumber,
                                hash );
    delete [] hash;

    serverSequenceNumber++;
    
    return result;
    }

