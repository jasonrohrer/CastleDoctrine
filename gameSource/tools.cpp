#include "tools.h"


#include "gameElements.h"



typedef struct toolRecord {
        int id;
        
        char *name;
        
        char *description;
        
        SpriteHandle sprite;

        int reach;

    } toolRecord;



static SimpleVector<toolRecord> tools;

static int idSpaceSize = 0;

// some may be -1
static int *idToIndexMap = NULL;



void initTools() {
    File elementsDir( NULL, "gameElements" );

    if( !elementsDir.exists() || !elementsDir.isDirectory() ) {
        return;
        }
    
    File *toolsDir = elementsDir.getChildFile( "tools" );
    
    if( toolsDir == NULL ) {
        return;
        }
    else if( !toolsDir->exists() || !toolsDir->isDirectory() ) {
        delete toolsDir;
        return;
        }
    
    int numTools;
    File **toolNameDirs = toolsDir->getChildFiles( &numTools );

    delete toolsDir;
    
    if( toolNameDirs == NULL ) {
        return;
        }
    
    
    for( int i=0; i<numTools; i++ ) {
        
        File *f = toolNameDirs[i];
        

        if( f->exists() && f->isDirectory() ) {
            
            char completeRecord = true;

            toolRecord r;
            
            r.name = f->getFileName();
            r.description = NULL;
            r.sprite = NULL;
            
            File *infoFile = f->getChildFile( "info.txt" );
            
            completeRecord = readInfoFile( infoFile, 
                                           &( r.id ), &( r.description ) );
            delete infoFile;



            if( completeRecord ) {

                // read reach, if present (if not, default to 1)
                r.reach = 1;

                File *reachFile = f->getChildFile( "reach.txt" );
            
                if( reachFile->exists() ) {
                    
                    char *reach = reachFile->readFileContents();
                
                    sscanf( reach, "%d", &( r.reach ) );

                    delete [] reach;
                    }
                
                delete reachFile;
                
                
                // look for sprite TGA
                int numChildFiles;
                File **childFiles = f->getChildFiles( &numChildFiles );
    
                char *tgaPath = NULL;

                for( int j=0; j<numChildFiles; j++ ) {
        
                    File *f = childFiles[j];
        
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


                if( tgaPath != NULL ) {
                    r.sprite = loadSpriteBase( tgaPath );
                    delete [] tgaPath;
                    }
                else {
                    completeRecord = false;
                    }
                }
            
            if( completeRecord ) {
                if( r.id >= idSpaceSize ) {
                    idSpaceSize = r.id + 1;
                    }
                tools.push_back( r );
                }
            else {
                delete [] r.name;
                if( r.description != NULL ) {
                    delete [] r.description;
                    }
                if( r.sprite != NULL ) {
                    freeSprite( r.sprite );
                    }
                }
            }
        delete f;
        }

    delete [] toolNameDirs;


    // build map
    idToIndexMap = new int[idSpaceSize];
    for( int i=0; i<idSpaceSize; i++ ) {
        idToIndexMap[i] = -1;
        }

    for( int i=0; i<tools.size(); i++ ) {
        toolRecord r = *( tools.getElement( i ) );
        
        idToIndexMap[r.id] = i;
        }
    }



void freeTools() {
    for( int i=0; i<tools.size(); i++ ) {
        toolRecord r = *( tools.getElement( i ) );
        
        
        delete [] r.name;
        delete [] r.description;
        
        freeSprite( r.sprite );
        }

    tools.deleteAll();
    
    if( idToIndexMap != NULL ) {
        delete [] idToIndexMap;
        
        idToIndexMap = NULL;
        }
    }



static toolRecord *getToolRecord( int inObjectID ) {

    int index = idToIndexMap[inObjectID];

    return tools.getElement( index );
    }



SpriteHandle getToolSprite( int inObjectID ) {
    return getToolRecord( inObjectID )->sprite;
    }



int getToolReach( int inObjectID ) {
    return getToolRecord( inObjectID )->reach;
    }



const char *getToolName( int inObjectID ) {
    return getToolRecord( inObjectID )->name;
    }


const char *getToolDescription( int inObjectID  ) {
    return getToolRecord( inObjectID )->description;
    }



int getToolID( const char *inName ) {
    for( int i=0; i<tools.size(); i++ ) {
        toolRecord *r = tools.getElement( i );
        
        if( strcmp( r->name, inName ) == 0 ) {
            return r->id;
            }
        } 

    return -1;
    }



int *getFullToolIDList( int *outNumIDs ) {
    *outNumIDs = tools.size();
    
    int *returnList = new int[ *outNumIDs ];
    
    
    for( int i=0; i<*outNumIDs; i++ ) {
        toolRecord *r = tools.getElement( i );
    
        returnList[i] = r->id;
        }
    
    return returnList;
    }
