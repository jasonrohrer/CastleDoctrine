#include "houseTransitions.h"


#include "houseObjects.h"



#include "minorGems/util/SimpleVector.h"
#include "minorGems/util/stringUtils.h"

#include "minorGems/io/file/File.h"


// describes a transition
typedef struct TransitionRecord {
        int triggerID;

        int targetID;
        int targetStartState;
        
        int targetEndState;
    } TransitionRecord;




// contains all transitions for a given trigger
typedef struct TransitionTriggerRecord {
    
        int triggerID;
        

        // can be 0 and NULL if no transitions defined for this triggerID
        int numTransitions;
        TransitionRecord *transitions;
        
        // used when parsing records
        int numFilled;
        
    } TransitionTriggerRecord;


static int numTriggerRecords = 0;
static TransitionTriggerRecord *triggerRecords = NULL;




#define NUM_BUILT_IN_TRIGGERS 2

static const char *builtInTriggerNames[NUM_BUILT_IN_TRIGGERS] = { 
    "player",
    "power"
    };
                                              
    

static int getTriggerID( char *inTriggerName ) {
    for( int i=0; i<NUM_BUILT_IN_TRIGGERS; i++ ) {
        
        if( strcmp( builtInTriggerNames[i], inTriggerName ) == 0 ) {
            return i;
            }
        }
    return -1;
    }





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
            
            // samples:
            // player   #   floor_trap:1   =>  2
            // player   #   floor_trap   =>  2


            
            char triggerName[100];
            char targetName[100];
            
            int startState;
            int endState;
            
            
            int numRead = sscanf( 
                trimmedLine, 
                "%99[^# \t]   #   %99[^:=> \t] : %d   =>   %d", 
                triggerName, targetName, 
                &startState, &endState );
            
            char scanFail = false;

            if( numRead != 4 ) {
                
                numRead = sscanf( trimmedLine, 
                                  "%99[^# \t]   #   %99[^:=> \t]   =>   %d", 
                                  triggerName, targetName, 
                                  &endState );
                if( numRead != 3 ) {
                    scanFail = true;
                    }
                else {
                    startState = -1;
                    }
                }

            if( ! scanFail ) {
                
                TransitionRecord r;
            
                r.triggerID = getTriggerID( triggerName );
                
                r.targetID = getObjectID( targetName );
                
                r.targetStartState = startState;
                r.targetEndState = endState;
                
                if( r.triggerID != -1 && r.targetID != -1 ) {
                    parsedRecords.push_back( r );
                    }
                else {
                    scanFail = true;
                    }
                }

            if( scanFail ) {
                printf( "Failed to parse transition line:\n"
                        "    '%s'\n", trimmedLine );
                }

            }
        
        delete [] trimmedLine;
        }
    
    delete [] lines;


    
    SimpleVector<int> uniqueTriggerIDs;
    
    SimpleVector<int> occurrenceCounts;

    int maxTriggerID = 0;

    for( int i=0; i<parsedRecords.size(); i++ ) {
    
        TransitionRecord *r = parsedRecords.getElement(i);

        int exisitingIndex = uniqueTriggerIDs.getElementIndex( r->triggerID );
        if( exisitingIndex == -1 ) {
            
            uniqueTriggerIDs.push_back( r->triggerID );
            occurrenceCounts.push_back( 1 );
            
            if( r->triggerID > maxTriggerID ) {
                maxTriggerID = r->triggerID;
                }
            }
        else {
            ( *( occurrenceCounts.getElement( exisitingIndex ) ) ) ++;
            }
        }


    // make trigger-indexed bins
    
    numTriggerRecords = maxTriggerID + 1;
    
    triggerRecords = new TransitionTriggerRecord[ numTriggerRecords ];
    

    for( int i=0; i<uniqueTriggerIDs.size(); i++ ) {
        
        int id = *( uniqueTriggerIDs.getElement( i ) );
        int count = *( occurrenceCounts.getElement( i ) );
        
        triggerRecords[id].numTransitions = count;
        triggerRecords[id].numFilled = 0;
        if( count != 0 ) {
            triggerRecords[id].transitions = new TransitionRecord[ count ];
            }
        else {
            triggerRecords[id].transitions = NULL;
            }
        }

    
    // finally, stick each parsed transition into an appropriate bin

    for( int i=0; i<parsedRecords.size(); i++ ) {
        
        TransitionRecord r = *( parsedRecords.getElement(i) );
        
        TransitionTriggerRecord *triggerRecord = 
            &( triggerRecords[ r.triggerID ] );
        
        triggerRecord->transitions[ triggerRecord->numFilled ] = r;
        
        triggerRecord->numFilled ++;
        }
    

    }



void freeHouseTransitions() {
    for( int i=0; i<numTriggerRecords; i++ ) {
        TransitionTriggerRecord *triggerRecord = &( triggerRecords[ i ] );
    
        if( triggerRecord->transitions != NULL ) {
            delete [] triggerRecord->transitions;
            }
        }
    delete [] triggerRecords;
    triggerRecords = NULL;
    numTriggerRecords = 0;    
    }



void applyTransitions( int *inMapIDs, int *inMapStates, int inMapW, int inMapH,
                       int inRobberIndex ) {

    int playerTileID = inMapIDs[ inRobberIndex ];
    int playerTileState = inMapStates[ inRobberIndex ];
    


    TransitionTriggerRecord *triggerRecord = 
            &( triggerRecords[ getTriggerID( (char*)"player" ) ] );

    for( int i=0; i<triggerRecords->numTransitions; i++ ) {
        
        TransitionRecord *transRecord = &( triggerRecord->transitions[i] );
        
        if( transRecord->targetID == playerTileID
            &&
            transRecord->targetStartState == playerTileState 
            &&
            transRecord->targetEndState != playerTileState ) {
            
            printf( "Transition hit\n" );

            inMapStates[ inRobberIndex ] = transRecord->targetEndState;
            }
        }
    }

