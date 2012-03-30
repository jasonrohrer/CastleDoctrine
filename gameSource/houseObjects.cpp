#include "houseObjects.h"

#include "minorGems/util/SimpleVector.h"

#include "minorGems/io/file/File.h"




typedef struct houseObjectState {

        // can be 0 if state not present
        int numOrientations;
        
        SpriteHandle stateSprite[ MAX_ORIENTATIONS ];

    } houseObjectState;



typedef struct houseObjectRecord {
        int id;
        
        char *name;
        
        char *description;
        
        int numStates;
        
        
        // sprites can be absent for certain entries if state IDs 
        // are sparsely distributed
        // (absent sprites have 0 orientations)
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

    state.numOrientations = 0;
    

    if( tgaPath == NULL ) {
        return state;
        }
    
    
    printf( "Trying to read tga from %s\n", tgaPath );

    Image *image = readTGAFileBase( tgaPath );

    if( image == NULL ) {
        delete [] tgaPath;
        
        return state;
        }
    

    int fullH = image->getHeight();
    int fullW = image->getWidth();

    int tileH = fullW;

    state.numOrientations = fullH / tileH;
    
    printf( "  Reading %d orientations\n", state.numOrientations );

    for( int o=0; o<state.numOrientations; o++ ) {
        
        Image *subImage = image->getSubImage( 0, tileH * o,
                                              fullW, tileH );
        
        state.stateSprite[o] = fillSprite( subImage, transCorner );
        
        delete subImage;
        }

    delete image;
    
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
                    else {
                        
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

            for( int o=0; o<r.states[s].numOrientations; o++ ) {
                freeSprite( r.states[s].stateSprite[o] );
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



SpriteHandle getObjectSprite( int inID, int inOrientation, int inState ) {
    

    int index = idToIndexMap[inID];
    
    houseObjectRecord *r = objects.getElement( index );

    if( inState >= r->numStates ) {
        // switch to default state
        inState = 0;
        }

    if( r->states[inState].numOrientations == 0 ) {
        return r->states[0].stateSprite[0];
        }
    

    return r->states[inState].stateSprite[inOrientation];
    }



int getNumOrientations( int inID, int inState ) {
    
    int index = idToIndexMap[inID];
    
    houseObjectRecord *r = objects.getElement( index );

    if( inState >= r->numStates ) {
        // switch to default state
        inState = 0;
        }

    return r->states[inState].numOrientations;
    }

