
#include "houseTransitions.h"


#include "minorGems/util/SimpleVector.h"
#include "minorGems/util/stringUtils.h"

#include "minorGems/io/file/File.h"


// describes a transition
typedef struct TransitionRecord {
        int triggerID;

        int targetID;
        int targetStartState;
        
        int targetEndState;
    }




// contains all transitions for a given trigger
typedef struct TransitionTriggerRecord {
    
        int triggerID;
        
        
        int numTransitions;
        
        TransitionRecord *transitions;

    } TransitionTriggerRecord;





void initHouseTransitions() {
    File elementsDir( NULL, "gameElements" );

    if( !elementsDir.exists() || !elementsDir.isDirectory() ) {
        return;
        }
    
    File *transitionsFile = elementsDir.getChildFile( "transitions.txt" );
    
    if( transitionsFile == NULL ) {
        return;
        }
    else if( !transitionsFile->exists() || transitionsFile->isDirectory() ) {
        delete transitionsFile;
        return;
        }

    char *transitionsText = transitionsFile->readFileContents();
    delete transitionsFile;


    // turn all newlines into \n
    
    char found;
    char *cleanedText1 = replaceAll( transitionsText, "\r\n", "\n", &found );
    
    delete [] transitionsText;
    
    char *cleanedText2 = replaceAll( cleanedText1, "\r", "\n", &found );

    delete [] cleanedText1;
    
    
    // process lines

    int numLines;
    char **lines = split( cleanedText2, "\n", &numLines );
    
    delete [] cleanedText2;
    

    SimpleVector<TransitionRecord> parsedRecords;

    for( int i=0; i<numLines; i++ ) {
        char *trimmedLine = trimWhitespace( lines[i] );
        delete [] lines[i];

        if( strcmp( trimmedLine, "" ) != 0 ) {
            
            

            }

        delete [] trimmedLine;
        }
    
    delete [] lines;
    }



void freeHouseTransitions();


// applies transition rules to transform inMapIDs and inMapStates
void applyTransitions( int *inMapIDs, int *inMapStates, int inMapW, int inMapH,
                       int inRobberIndex );
