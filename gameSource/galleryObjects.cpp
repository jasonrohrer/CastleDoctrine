#include "galleryObjects.h"


#include "gameElements.h"



typedef struct galleryObjectRecord {
        int id;
        
        char *name;
        
        char *description;
        
        SpriteHandle sprite;

    } galleryObjectRecord;



static SimpleVector<galleryObjectRecord> galleryObjects;

static int idSpaceSize = 0;

// some may be -1
static int *idToIndexMap = NULL;



void initGalleryObjects() {
    File elementsDir( NULL, "gameElements" );

    if( !elementsDir.exists() || !elementsDir.isDirectory() ) {
        return;
        }
    
    File *galleryObjectsDir = elementsDir.getChildFile( "gallery" );
    
    if( galleryObjectsDir == NULL ) {
        return;
        }
    else if( !galleryObjectsDir->exists() || 
             !galleryObjectsDir->isDirectory() ) {
        delete galleryObjectsDir;
        return;
        }
    
    int numGalleryObjects;
    File **galleryObjectNameDirs =
        galleryObjectsDir->getChildFiles( &numGalleryObjects );

    delete galleryObjectsDir;
    
    if( galleryObjectNameDirs == NULL ) {
        return;
        }
    
    
    for( int i=0; i<numGalleryObjects; i++ ) {
        
        File *f = galleryObjectNameDirs[i];
        

        if( f->exists() && f->isDirectory() ) {
            
            char completeRecord = true;

            galleryObjectRecord r;
            
            r.name = f->getFileName();
            r.description = NULL;
            r.sprite = NULL;
            
            File *infoFile = f->getChildFile( "info.txt" );
            
            completeRecord = readInfoFile( infoFile, 
                                           &( r.id ), &( r.description ) );
            delete infoFile;



            if( completeRecord ) {                
                
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

                    delete childFiles[j];
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
                galleryObjects.push_back( r );
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

    delete [] galleryObjectNameDirs;


    // build map
    idToIndexMap = new int[idSpaceSize];
    for( int i=0; i<idSpaceSize; i++ ) {
        idToIndexMap[i] = -1;
        }

    for( int i=0; i<galleryObjects.size(); i++ ) {
        galleryObjectRecord r = *( galleryObjects.getElement( i ) );
        
        idToIndexMap[r.id] = i;
        }
    }



void freeGalleryObjects() {
    for( int i=0; i<galleryObjects.size(); i++ ) {
        galleryObjectRecord r = *( galleryObjects.getElement( i ) );
        
        
        delete [] r.name;
        delete [] r.description;
        
        freeSprite( r.sprite );
        }

    galleryObjects.deleteAll();
    
    if( idToIndexMap != NULL ) {
        delete [] idToIndexMap;
        
        idToIndexMap = NULL;
        }
    }



static galleryObjectRecord *getGalleryObjectRecord( int inObjectID ) {

    int index = idToIndexMap[inObjectID];

    return galleryObjects.getElement( index );
    }



SpriteHandle getGalleryObjectSprite( int inObjectID ) {
    return getGalleryObjectRecord( inObjectID )->sprite;
    }


const char *getGalleryObjectName( int inObjectID ) {
    return getGalleryObjectRecord( inObjectID )->name;
    }


const char *getGalleryObjectDescription( int inObjectID  ) {
    return getGalleryObjectRecord( inObjectID )->description;
    }



int getGalleryObjectID( const char *inName ) {
    for( int i=0; i<galleryObjects.size(); i++ ) {
        galleryObjectRecord *r = galleryObjects.getElement( i );
        
        if( strcmp( r->name, inName ) == 0 ) {
            return r->id;
            }
        } 

    return -1;
    }
