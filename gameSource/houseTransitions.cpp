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




#define NUM_BUILT_IN_TRIGGERS 5

static const char *builtInTriggerNames[NUM_BUILT_IN_TRIGGERS] = { 
    "player",
    "power",
    "noPower",
    "powerNorth",
    "noPowerNorth",
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



// returns true if something changed, which might require an additional
// transition step for propagation
static char applyPowerTransitions( int *inMapIDs, 
                                   int *inMapStates, int inMapW, int inMapH ) {
    int numCells = inMapW * inMapH;

    char *powerMap = new char[ numCells ];
    memset( powerMap, false, numCells );

    // separately track whether each cell is passing power only left-to-right
    // or only top-to-bottom (can be both!)
    // fully conductive cells that become powered have both of these set
    char *leftRightPowerMap = new char[numCells];
    memset( leftRightPowerMap, false, numCells );

    char *topBottomPowerMap = new char[numCells];
    memset( topBottomPowerMap, false, numCells );


    char change = false;
    
    // first, start power at cells that are powered currently

    for( int i=0; i<numCells; i++ ) {
        if( isPropertySet( inMapIDs[i], inMapStates[i], powered ) ) {
            powerMap[i] = true;
            change = true;
            }
        }

    while( change ) {
        // keep propagating power through conductive materials
        change = false;
        
        for( int i=0; i<numCells; i++ ) {
            if( powerMap[i] &&
                leftRightPowerMap[i] &&
                topBottomPowerMap[i] ) {
                // ignore cells already fully powered
                continue;
                }
            
            if( i == 292 || i == 324 ) {
                printf( "Here\n" );
                }
            

            if( isPropertySet( inMapIDs[i], inMapStates[i], conductive ) ) {
                // look for neighbors with power
                // continue as soon as one found
                
                int y = i / inMapW;
                int x = i % inMapW;
                
                if( y > 0 &&
                    powerMap[ i - inMapW ] &&
                    ( ! leftRightPowerMap[ i - inMapW ]
                      || topBottomPowerMap[ i - inMapW ] ) ) {
                    powerMap[i] = true;
                    leftRightPowerMap[i] = true;
                    topBottomPowerMap[i] = true;
                    change = true;
                    continue;
                    }
                if( y < inMapH - 1 &&
                    powerMap[ i + inMapW ] &&
                    ( ! leftRightPowerMap[ i + inMapW ]
                      || topBottomPowerMap[ i + inMapW ] ) ) {
                    powerMap[i] = true;
                    leftRightPowerMap[i] = true;
                    topBottomPowerMap[i] = true;
                    change = true;
                    continue;
                    }
                if( x > 0 &&
                    powerMap[ i - 1 ] &&
                    ( leftRightPowerMap[ i - 1 ]
                      || ! topBottomPowerMap[ i - 1 ] ) ) {
                    powerMap[i] = true;
                    leftRightPowerMap[i] = true;
                    topBottomPowerMap[i] = true;
                    change = true;
                    continue;
                    }            
                if( x < inMapW - 1 &&
                    powerMap[ i + 1 ] &&
                    ( leftRightPowerMap[ i + 1 ]
                      || ! topBottomPowerMap[ i + 1 ] ) ) {
                    powerMap[i] = true;
                    leftRightPowerMap[i] = true;
                    topBottomPowerMap[i] = true;
                    change = true;
                    continue;
                    }
                }
            else {
                if( !leftRightPowerMap[i] &&
                    isPropertySet( inMapIDs[i], inMapStates[i], 
                                   conductiveLeftToRight ) ) {
            
                    // look for left or right neighbor with power
                    int x = i % inMapW;
                    
                    if( x > 0 &&
                        powerMap[ i - 1 ] &&
                        ( leftRightPowerMap[ i - 1 ]
                          || ! topBottomPowerMap[ i - 1 ] )  ) {
                        
                        powerMap[i] = true;
                        leftRightPowerMap[i] = true;
                        change = true;
                        }
                    if( x < inMapW - 1 &&
                        powerMap[ i + 1 ] &&
                        ( leftRightPowerMap[ i + 1 ]
                          || ! topBottomPowerMap[ i + 1 ] ) ) {
                        
                        powerMap[i] = true;
                        leftRightPowerMap[i] = true;
                        change = true;
                        }
                    }
                

                if( !topBottomPowerMap[i] &&
                    isPropertySet( inMapIDs[i], inMapStates[i], 
                                   conductiveTopToBottom ) ) {
            
                    // look for top or bottom neighbor with power
                    int y = i / inMapW;
                    
                    if( y > 0 &&
                        powerMap[ i - inMapW ] &&
                        ( ! leftRightPowerMap[ i - inMapW ]
                          || topBottomPowerMap[ i - inMapW ] ) ) {
                        
                        powerMap[i] = true;
                        topBottomPowerMap[i] = true;
                        change = true;
                        }
                    if( y < inMapH - 1 &&
                        powerMap[ i + inMapW ] &&
                        ( ! leftRightPowerMap[ i + inMapW ]
                          || topBottomPowerMap[ i + inMapW ] ) ) {
                        
                        powerMap[i] = true;
                        topBottomPowerMap[i] = true;
                        change = true;
                        }
                    }
                
                }
            }
        }
    
    
    // now execute transitions for cells based on power or noPower
    
    char transitionHappened = false;
    

    for( int i=0; i<numCells; i++ ) {
        
        TransitionTriggerRecord *r;
        
        SimpleVector<TransitionTriggerRecord *> applicableTriggers;
        

        if( powerMap[i]  ) {
            // this cell is powered
            r = 
                &( triggerRecords[ getTriggerID( (char*)"power" ) ] );
            
            applicableTriggers.push_back( r );
            }
        else {            
            // this cell itself isn't powered
            r = 
                &( triggerRecords[ getTriggerID( (char*)"noPower" ) ] );
            
            applicableTriggers.push_back( r );
            }
        
            
        // check if it's northern neighbor is powered
        int y = i / inMapW;

        char powerNorth = false;
            
        if( y < inMapH - 1 ) {
            // check above
            
            if( powerMap[ i + inMapW ] ) {
                powerNorth = true;
                }
            }
        
        if( powerNorth ) {
            r = 
                &( triggerRecords[ getTriggerID( (char*)"powerNorth" ) ] );
            applicableTriggers.push_back( r );
            }
        else {
            r = 
                &( triggerRecords[ getTriggerID( (char*)"noPowerNorth" ) ] );
            
            applicableTriggers.push_back( r );
            }
        
        
        for( int a=0; a<applicableTriggers.size(); a++ ) {
            TransitionTriggerRecord *r = 
                *( applicableTriggers.getElement( a ) );
        
            for( int j=0; j<r->numTransitions; j++ ) {
        
                TransitionRecord *transRecord = 
                    &( r->transitions[j] );
            
                if( transRecord->targetID == inMapIDs[i]
                    &&
                    transRecord->targetStartState == inMapStates[i] 
                    &&
                    transRecord->targetEndState != inMapStates[i] ) {
                
                    inMapStates[ i ] = transRecord->targetEndState;

                    transitionHappened = true;
                    }
                }
            }
        
        }


    delete [] powerMap;
    delete [] leftRightPowerMap;
    delete [] topBottomPowerMap;

    return transitionHappened;
    }





void applyTransitions( int *inMapIDs, int *inMapStates, int inMapW, int inMapH,
                       int inRobberIndex ) {

    int playerTileID = inMapIDs[ inRobberIndex ];
    int playerTileState = inMapStates[ inRobberIndex ];
    


    TransitionTriggerRecord *r = 
            &( triggerRecords[ getTriggerID( (char*)"player" ) ] );

    for( int i=0; i<r->numTransitions; i++ ) {
        
        TransitionRecord *transRecord = &( r->transitions[i] );
        
        if( transRecord->targetID == playerTileID
            &&
            transRecord->targetStartState == playerTileState 
            &&
            transRecord->targetEndState != playerTileState ) {
            
            printf( "Transition hit\n" );

            inMapStates[ inRobberIndex ] = transRecord->targetEndState;
            }
        }
    
    char transitionHappened = true;
    while( transitionHappened ) {
        transitionHappened = 
            applyPowerTransitions( inMapIDs, inMapStates, inMapW, inMapH );
        }

    }

