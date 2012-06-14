#include "gameElements.h"


char readInfoFile( File *inFile, int *outID, char **outDescription ) {
    char completeRecord = true;
    
    if( inFile->exists() ) {
                
        char *info = inFile->readFileContents();
                
        char *infoStart = info;

        int numRead = sscanf( info, "%d", outID );
                
        if( numRead != 1 ) {
            delete [] infoStart;
            return false;
            }

        // skip the first "
        int readChar = ' ';

        while( readChar != '"' && readChar != '\0' ) {
            readChar = info[0];
            info = &( info[1] );
            }

                
        char *descriptionString = new char[1000];
        // scan a string of up to 999 characters, stopping
        // at the first " character
        numRead = sscanf( info, "%999[^\"]",
                          descriptionString );

        delete [] infoStart;
                

        if( numRead == 1 ) {
            *outDescription = stringDuplicate( descriptionString );
            }
        else {
            *outDescription = NULL;
            completeRecord = false;
            }
        delete [] descriptionString;
        }
    else {
        completeRecord = false;
        }


    return completeRecord;
    }
