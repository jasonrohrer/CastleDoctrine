#include "houseObjects.h"

#include "minorGems/util/SimpleVector.h"

#include "minorGems/io/file/File.h"



typedef struct houseObjectState {
        // can be NULL if state not present
        SpriteHandle stateSprite;
    } houseObjectState;



typedef struct houseObjectRecord {
        int id;
        
        char *name;
        
        char *description;
        
        int numStates;
        
        
        // sprites inside can be NULL for certain entries if state IDs 
        // are sparsely distributed
        //
        // MUST have at least 0 defined
        houseObjectState *states;
        

    } houseObjectRecord;



static SimpleVector<houseObjectRecord> objects;

int idSpaceSize = 0;

// some may be -1
static int *idToIndexMap = NULL;



static houseObjectState readState( File *inStateDir ) {
    
    int numChildFiles;
    File **childFiles = inStateDir->getChildFiles( &numChildFiles );
    
    char *tgaPath = NULL;
    char transCorner = true;

    for( int i=0; i<numChildFiles; i++ ) {
        
        File *f = childFiles[i];
        
        char *name = f->getFileName();
        
        if( strstr( name, ".tga" ) != NULL ) {
            
            if( tgaPath != NULL ) {
                delete [] tgaPath;
                }
            tgaPath = f->getFullFileName();
            }
        delete [] name;

        delete childFiles[i];
        }
    delete [] childFiles;

    
    houseObjectState state;
    
    printf( "Trying to read tga from %s\n", tgaPath );
    
    state.stateSprite = loadSpriteBase( tgaPath, transCorner );
    
    delete [] tgaPath;
    
    return state;
    }




void initHouseObjects() {
    File elementsDir( NULL, "gameElements" );

    if( !elementsDir.exists() || !elementsDir.isDirectory() ) {
        return;
        }
    
    File *objectsDir = elementsDir.getChildFile( "houseObjects" );
    
    if( objectsDir == NULL ) {
        return;
        }
    else if( !objectsDir->exists() || !objectsDir->isDirectory() ) {
        delete objectsDir;
        return;
        }
    
    int numObjects;
    File **objectNameDirs = objectsDir->getChildFiles( &numObjects );

    delete objectsDir;
    
    if( objectNameDirs == NULL ) {
        return;
        }
    
    
    for( int i=0; i<numObjects; i++ ) {
        
        File *f = objectNameDirs[i];
        

        if( f->exists() && f->isDirectory() ) {
            
            char completeRecord = true;

            houseObjectRecord r;
            
            r.name = f->getFileName();
            r.description = NULL;

            File *infoFile = f->getChildFile( "info.txt" );
            
            if( infoFile->exists() ) {
                
                char *info = infoFile->readFileContents();
                
                char *infoStart = info;

                int numRead = sscanf( info, "%d", &( r.id ) );
                
                if( numRead != 1 ) {
                    completeRecord = false;
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
                    r.description = stringDuplicate( descriptionString );
                    }
                else {
                    r.description = stringDuplicate( r.name );
                    }
                delete [] descriptionString;
                }
            else {
                completeRecord = false;
                }


            if( completeRecord ) {
                
                // read states

                int numStates;
                File **stateDirs = f->getChildFiles( &numStates );
                
                int dirCount = 0;
                for( int c=0; c<numStates; c++ ) {
                    if( stateDirs[c]->isDirectory() ) {
                        dirCount++;
                        }
                    }
                
                
                r.numStates = dirCount;
                

                r.states = new houseObjectState[ r.numStates ];
                
                
                int s = 0;
                for( int c=0; c<numStates; c++ ) {
                    if( stateDirs[c]->isDirectory() ) {
                        
                        r.states[s] = readState( stateDirs[c] );
                        
                        s++;
                        }
                    delete stateDirs[c];
                    }
                delete [] stateDirs;

                
                if( r.id >= idSpaceSize ) {
                    idSpaceSize = r.id + 1;
                    }

                objects.push_back( r );
                }
            else {
                delete [] r.name;
                if( r.description != NULL ) {
                    delete [] r.description;
                    }
                }
            
            delete infoFile;
            }

        delete f;
        }

    delete [] objectNameDirs;


    // build map
    idToIndexMap = new int[idSpaceSize];
    for( int i=0; i<idSpaceSize; i++ ) {
        idToIndexMap[i] = -1;
        }

    for( int i=0; i<objects.size(); i++ ) {
        houseObjectRecord r = *( objects.getElement( i ) );
        
        idToIndexMap[r.id] = i;
        }
    
    }



void freeHouseObjects() {
    for( int i=0; i<objects.size(); i++ ) {
        houseObjectRecord r = *( objects.getElement( i ) );
        
        
        delete [] r.name;
        delete [] r.description;
        
        for( int s=0; s<r.numStates; s++ ) {
            
            if( r.states[s].stateSprite != NULL ) {
                freeSprite( r.states[s].stateSprite );
                }
            }
        delete [] r.states;
        }

    objects.deleteAll();
    
    if( idToIndexMap != NULL ) {
        delete [] idToIndexMap;
        
        idToIndexMap = NULL;
        }
    }



SpriteHandle getObjectSprite( int inID, int inState ) {
    

    int index = idToIndexMap[inID];
    
    houseObjectRecord *r = objects.getElement( index );

    if( inState >= r->numStates ) {
        // switch to default state
        inState = 0;
        }

    SpriteHandle h = r->states[inState].stateSprite;

    if( h == NULL ) {
        return r->states[0].stateSprite;
        }

    return h;
    }

