#include "houseTransitions.h"


#include "houseObjects.h"
#include "tools.h"



#include "minorGems/util/SimpleVector.h"
#include "minorGems/util/stringUtils.h"

#include "minorGems/io/file/File.h"


#include <stdlib.h>



static char mobileObjectsFrozen = false;

void freezeMobileObjects( char inFreeze ) {
    mobileObjectsFrozen = inFreeze;
    }



// describes a transition
typedef struct TransitionRecord {
        int triggerID;

        int triggerState;

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




#define NUM_BUILT_IN_TRIGGERS 6

static const char *builtInTriggerNames[NUM_BUILT_IN_TRIGGERS] = { 
    "player",
    "mobile",
    "power",
    "noPower",
    "powerNorth",
    "noPowerNorth",
    };
                                              
    

// built-in trigger IDs start at 10000
// other trigger IDs match objectIDs
static int getTriggerID( char *inTriggerName ) {
    for( int i=0; i<NUM_BUILT_IN_TRIGGERS; i++ ) {
        
        if( strcmp( builtInTriggerNames[i], inTriggerName ) == 0 ) {
            return i + 10000;
            }
        }

    int toolID = getToolID( inTriggerName );
    
    if( toolID != -1 ) {
        return toolID;
        }
    else {
        return getObjectID( inTriggerName );
        }
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
            // floor_trap:2   #   pitbull:1   =>  2


            
            char triggerName[100];
            char targetName[100];
            
            int triggerState;

            int startState;
            int endState;
            
            
            // :state parts are optional
            // count them
            int colonCount = 0;
            int lineLength = strlen( trimmedLine );
            
            for( int c=0; c<lineLength; c++ ) {
                if( trimmedLine[c] == ':' ) {
                    colonCount ++;
                    }
                }
            
            char scanFail = false;

            if( colonCount == 0 ) {
                int numRead = sscanf( 
                    trimmedLine, 
                    "%99[^# \t]   #   %99[^=> \t]   =>   %d", 
                    triggerName, targetName, 
                    &endState );
                
                if( numRead != 3 ) {
                    scanFail = true;
                    }
                else {
                    triggerState = -1;
                    startState = -1;
                    }
                }
            else if( colonCount == 2 ) {
                int numRead = sscanf( 
                    trimmedLine, 
                    "%99[^:# \t] : %d   #   %99[^:=> \t] : %d   =>   %d", 
                    triggerName, &triggerState, targetName, 
                    &startState, &endState );
            

                if( numRead != 5 ) {
                    scanFail = true;
                    }                
                }
            else if( colonCount == 1 ) {
                // a :state part for either the trigger or the target
                
                int numRead = sscanf( 
                    trimmedLine, 
                    "%99[^:# \t] : %d   #   %99[^=> \t]  =>   %d", 
                    triggerName, &triggerState, targetName, &endState );

                if( numRead != 4 ) {
                    // try opposite
                    numRead = sscanf( 
                    trimmedLine, 
                    "%99[^# \t]   #   %99[^:=> \t] : %d   =>   %d", 
                    triggerName, targetName, 
                    &startState, &endState );
                    
                    if( numRead != 4 ) {
                        scanFail = true;
                        }
                    else {
                        triggerState = -1;
                        }                    
                    }
                else {
                    startState = -1;
                    }
                }
            else {
                scanFail = true;
                }


            if( ! scanFail ) {
                
                TransitionRecord r;
            
                r.triggerID = getTriggerID( triggerName );
                
                r.triggerState = triggerState;

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
    
    for( int i=0; i<numTriggerRecords; i++ ) {
        triggerRecords[i].triggerID = i;
        triggerRecords[i].numTransitions = 0;
        triggerRecords[i].transitions = NULL;
        }
    

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



#include "minorGems/crypto/hashes/sha1.h"

// computes a digest of a map state
// resulting string destroyed by caller
static char *getMapStateChecksum( int *inMapStates, int inMapW, int inMapH ) {
    
    int numCells = inMapW * inMapH;
        
    return
        computeSHA1Digest( (unsigned char*)inMapStates, 
                           numCells * sizeof( int ) );
    
    }




// returns power map with (inMapW * inMapH) cells
static char *propagatePower(  int *inMapIDs, 
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

    delete [] leftRightPowerMap;
    delete [] topBottomPowerMap;

    return powerMap;
    }



// returns true if something changed, which might require an additional
// transition step for propagation
static char applyPowerTransitions( int *inMapIDs, 
                                   int *inMapStates, int inMapW, int inMapH ) {
    
    
    char *powerMap = propagatePower( inMapIDs, inMapStates, inMapW, inMapH );
    

    int numCells = inMapW * inMapH;

    
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
                    ( transRecord->targetStartState == inMapStates[i]
                      || 
                      transRecord->targetStartState == -1 )
                    &&
                    transRecord->targetEndState != inMapStates[i] ) {
                
                    inMapStates[ i ] = transRecord->targetEndState;

                    transitionHappened = true;
                    }
                }
            }
        
        }


    delete [] powerMap;

    return transitionHappened;
    }




static void applyMobileTransitions( int *inMapIDs, int *inMapStates, 
                                    int *inMapMobileIDs, 
                                    int *inMapMobileStates,
                                    int inMapW, int inMapH,
                                    int inRobberIndex ) {
    

    // first, move mobile objects around

    
    // process playerSeeking properties

    int numCells = inMapW * inMapH;    

    int robberX = inRobberIndex % inMapW;
    int robberY = inRobberIndex / inMapW;

    char *moveHappened = new char[ numCells ];
    

    memset( moveHappened, false, numCells );
    

    for( int i=0; i<numCells; i++ ) {
        if( !moveHappened[i] &&
            isPropertySet( inMapMobileIDs[i], 
                           inMapMobileStates[i], playerSeeking ) ) {
            
            int x = i % inMapW;
            int y = i / inMapW;
            
            int dX = robberX - x;
            int dY = robberY - y;
            
            if( dX == 0 && dY == 0 ) {
                continue;
                }

            int destX = x;
            int destY = y;

            if( abs( dX ) > abs( dY ) ) {
                // x move
                
                if( dX < 0 ) {
                    destX--;
                    }
                else {
                    destX++;
                    }
                }
            else {
                // y move

                if( dY < 0 ) {
                    destY--;
                    }
                else {
                    destY++;
                    }
                }

            int destI = destY * inMapW + destX;
            
            
            if( inMapMobileIDs[destI] == 0 &&
                ! isPropertySet( inMapIDs[destI], inMapStates[destI],
                                 blocking ) &&
                ! isPropertySet( inMapIDs[destI], inMapStates[destI],
                                 mobileBlocking ) ) {
                
                inMapMobileIDs[destI] = inMapMobileIDs[i];
                inMapMobileStates[destI] = inMapMobileStates[i];
                
                inMapMobileIDs[i] = 0;
                inMapMobileStates[i] = 0;

                // don't keep moving it if we encounter it later in loop
                moveHappened[destI] = true;
                }
            
            }
        }
    




    // now process trigger-transitions (switch plates, etc) caused by the 
    // presensce of a mobile object

    SimpleVector<int> mobileIndices;
    
    // player is a mobile object
    // and it ALWAYS moves (player movement triggers transitions, for now)
    mobileIndices.push_back( inRobberIndex );

    for( int i=0; i<numCells; i++ ) {
        if( inMapMobileIDs[i] != 0 ) {
            // ignore other mobiles that didn't move just now
            // (so we don't trigger same switch over and over with a stationary
            //  mobile)
            if( moveHappened[i] ) {
                mobileIndices.push_back( i );
                }
            }
        }
    
    delete [] moveHappened;


    TransitionTriggerRecord *r = 
            &( triggerRecords[ getTriggerID( (char*)"mobile" ) ] );

    
    for( int j=0; j<mobileIndices.size(); j++ ) {
        int mobIndex = *( mobileIndices.getElement( j ) );
        
        int mobOverTileID = inMapIDs[ mobIndex ];
        int mobOverTileState = inMapStates[ mobIndex ];

        for( int i=0; i<r->numTransitions; i++ ) {
        
            TransitionRecord *transRecord = &( r->transitions[i] );
        
            if( transRecord->targetID == mobOverTileID
                &&
                ( transRecord->targetStartState == mobOverTileState
                  ||
                  transRecord->targetStartState == -1 )
                &&
                transRecord->targetEndState != mobOverTileState ) {
                
                printf( "Mobile-triggered transition hit\n" );

                inMapStates[ mobIndex ] = transRecord->targetEndState;
                
                // only allow one transition triggered per mobile object
                break;
                }
            }
        }


    // now process any transitions for mobile objects
    // that occupy same tile as player

    if( inMapMobileIDs[ inRobberIndex ] != 0 ) {
        
        r = &( triggerRecords[ getTriggerID( (char*)"player" ) ] );


        int playerOnMobID = inMapMobileIDs[ inRobberIndex ];
        int playerOnMobState = inMapMobileStates[ inRobberIndex ];
        

        for( int i=0; i<r->numTransitions; i++ ) {
            TransitionRecord *transRecord = &( r->transitions[i] );
        
            if( transRecord->targetID == playerOnMobID
                &&
                ( transRecord->targetStartState == playerOnMobState
                  ||
                  transRecord->targetStartState == -1 )
                &&
                transRecord->targetEndState != playerOnMobState ) {
                
                inMapMobileStates[ inRobberIndex ] = 
                    transRecord->targetEndState;
                
                // only allow one transition triggered for
                // the mobile that overlaps with player
                break;
                }
            }
        }
    
    

    }
    




void applyTransitions( int *inMapIDs, int *inMapStates, 
                       int *inMapMobileIDs, int *inMapMobileStates,
                       int inMapW, int inMapH,
                       int inRobberIndex ) {
    
    if( !mobileObjectsFrozen ) {
        applyMobileTransitions( inMapIDs, inMapStates,
                                inMapMobileIDs, inMapMobileStates,
                                inMapW, inMapH, inRobberIndex );
        }
    
    int numCells = inMapW * inMapH;

    
    // now process power transitions

    
    char transitionHappened = true;
    char loopDetected = false;
    
    // track checksums of states seen so far so that we can avoid a loop
    SimpleVector<char *> seenStates;
    
    // add start state
    seenStates.push_back( getMapStateChecksum( inMapStates, inMapW, inMapH ) );
    

    while( transitionHappened && ! loopDetected ) {
        transitionHappened = 
            applyPowerTransitions( inMapIDs, inMapStates, inMapW, inMapH );

        if( transitionHappened ) {
            char *newChecksum = 
                getMapStateChecksum( inMapStates, inMapW, inMapH );
            
            for( int i=0; i<seenStates.size(); i++ ) {
                char *oldChecksum = *( seenStates.getElement( i ) );
            
                if( strcmp( oldChecksum, newChecksum ) == 0 ) {
                    loopDetected = true;
                    }
                }
            
            seenStates.push_back( newChecksum );
            }
        }
    
    if( loopDetected ) {
        // make sure that all looping elements "settle down" into a 
        // consistent state (otherwise, we see various flip-flops due
        // to propagation times elsewhere in the map)
        
        // in case of looping, all elements involved in the loop settle
        // down into the lowest-seen state number that they encounter 
        // during execution of the loop
        
        int numCells = inMapW * inMapH;
        
        int *lowestSeenStates = new int[ numCells ];
        
        memcpy( lowestSeenStates, inMapStates, numCells * sizeof( int ) );
        
        // run one more time until we return to this state
        char *returnToChecksum = 
            getMapStateChecksum( inMapStates, inMapW, inMapH );
        

        applyPowerTransitions( inMapIDs, inMapStates, inMapW, inMapH );
        char *lastChecksum = 
            getMapStateChecksum( inMapStates, inMapW, inMapH );
        
        while( strcmp( lastChecksum, returnToChecksum ) != 0 ) {
            // a mid-loop state
            
            for( int i=0; i<numCells; i++ ) {
                
                if( inMapStates[i] < lowestSeenStates[i] ) {
                    // a lower state number seen for this cell
                    lowestSeenStates[i] = inMapStates[i];
                    }
                }
            delete [] lastChecksum;

            applyPowerTransitions( inMapIDs, inMapStates, inMapW, inMapH );
            lastChecksum = 
                getMapStateChecksum( inMapStates, inMapW, inMapH );
            }

        // returned to start-of-loop state
        delete [] lastChecksum;
        delete [] returnToChecksum;

        // set all cells in map to lowest-seen states from the loop
        memcpy( inMapStates, lowestSeenStates, numCells * sizeof( int ) );
        delete [] lowestSeenStates;
        }
    


    for( int i=0; i<seenStates.size(); i++ ) {
        delete [] *( seenStates.getElement( i ) );
        }




    // finally, apply transitions to mobile objects based on where they
    // are standing

    // indices might have changed
    SimpleVector<int> mobileIndices;

    for( int i=0; i<numCells; i++ ) {
        if( inMapMobileIDs[i] != 0 ) {
            mobileIndices.push_back( i );
            }
        }

    for( int j=0; j<mobileIndices.size(); j++ ) {
        int mobIndex = *( mobileIndices.getElement( j ) );
        
        int mobID = inMapMobileIDs[ mobIndex ];
        int mobState = inMapMobileStates[ mobIndex ];
        
        
        int mobOverTileID = inMapIDs[ mobIndex ];
        int mobOverTileState = inMapStates[ mobIndex ];

        
        inMapMobileStates[ mobIndex ] = checkTransition( mobID, mobState,
                                                         mobOverTileID,
                                                         mobOverTileState );
        }
    


    }




int checkTransition( int inTargetID, int inTargetState,
                     int inTriggerID, int inTriggerState ) {
    
    // all transitions triggered by this trigger
    TransitionTriggerRecord *r = &( triggerRecords[ inTriggerID ] );
        
    for( int i=0; i<r->numTransitions; i++ ) {
        
        TransitionRecord *transRecord = &( r->transitions[i] );
        
        if( ( transRecord->triggerState == inTriggerState
              ||
              transRecord->triggerState == -1
              )
            &&
            transRecord->targetID == inTargetID
            &&
            ( transRecord->targetStartState == inTargetState
              ||
              transRecord->targetStartState == -1 )
            &&
            transRecord->targetEndState != inTargetState ) {
                
            // return first one found
            return  transRecord->targetEndState;
            }
        }
    
    // default
    return inTargetState;
    }




// applies transition rule for a tool to transform inMapIDs and inMapStates
void applyToolTransition( int *inMapIDs, int *inMapStates, 
                          int *inMapMobileIDs, int *inMapMobileStates,
                          int inMapW, int inMapH,
                          int inToolID, int inToolTargetIndex ) {

    printf( "Applying transition for %d at index %d\n", inToolID,
            inToolTargetIndex );
    
    
    int tileID = inMapIDs[ inToolTargetIndex ];
    int tileState = inMapStates[ inToolTargetIndex ];
    
    int mobileID = inMapMobileIDs[ inToolTargetIndex ];
    int mobileState = inMapMobileStates[ inToolTargetIndex ];

    
    // all transitions triggered by this trigger
    TransitionTriggerRecord *r = &( triggerRecords[ inToolID ] );
    
    for( int i=0; i<r->numTransitions; i++ ) {
        
        TransitionRecord *transRecord = &( r->transitions[i] );
        
        // affects tile?
        if( transRecord->targetID == tileID
            &&
            ( transRecord->targetStartState == tileState
              ||
              transRecord->targetStartState == -1 )
            &&
            transRecord->targetEndState != tileState ) {
            
            inMapStates[ inToolTargetIndex ] = transRecord->targetEndState;
            
            // return after first one found
            return;
            }
        // affects mobile?
        else if( transRecord->targetID == mobileID
            &&
            ( transRecord->targetStartState == mobileState
              ||
              transRecord->targetStartState == -1 )
            &&
            transRecord->targetEndState != mobileState ) {
            
            inMapMobileStates[ inToolTargetIndex ] = 
                transRecord->targetEndState;
            
            // return after first one found
            return;
            }

        }
    }


