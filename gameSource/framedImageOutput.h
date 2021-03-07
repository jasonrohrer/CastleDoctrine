#include "minorGems/util/random/CustomRandomSource.h"


static CustomRandomSource tilingRandSource( 483915 );


static void writeShadowFramedFileOut( Image *inImage, int inID,
                                      char *inDescription ) {

    // doubleImage destroys argument
    Image *frameImage = doubleImage( readTGAFile( "frame.tga" ) );
    
    
    // frame image is 2x size of image, centered

    // paste into frame
    double *fr = frameImage->getChannel( 0 );
    double *fg = frameImage->getChannel( 1 );
    double *fb = frameImage->getChannel( 2 );
    
    double *r = inImage->getChannel( 0 );
    double *g = inImage->getChannel( 1 );
    double *b = inImage->getChannel( 2 );
    
    int w = inImage->getWidth();
    int h = inImage->getHeight();

    int fw = frameImage->getWidth();

    for( int y=0; y<h; y++ ) {
        int fy = y + h / 2;
        for( int x=0; x<w; x++ ) {
            int fx = x + w / 2;

            int i = y * w + x;
            int fi = fy * fw + fx;
            
            fr[ fi ] = r[i];
            fg[ fi ] = g[i];
            fb[ fi ] = b[i];
            }
        }
    
    
    Image *framedPainting = frameImage->getSubImage( 
        // back off to leave room for 2x wide frame (3 original pixels wid
        w / 2 - 6, h / 2 - 6, 
        w + 12, h + 12 );    

    
    // 1 through 155
    int fileID = inID - 1999;
    

    char *outFileName = 
        autoSprintf( "shadowedGallery/%03d.tga", fileID );
                        
    writeTGAFile( outFileName, framedPainting );

    delete [] outFileName;


    
    // now output in randomized order
    int randNum = tilingRandSource.getRandomBoundedInt( 0, 99999 );
    
    outFileName = 
        autoSprintf( "shadowedGallery/rand_%05d.tga", randNum );
                        
    writeTGAFile( outFileName, framedPainting );

    delete [] outFileName;



    delete framedPainting;
    delete frameImage;
    
    char *textFileName = autoSprintf( "shadowedGallery/%03d.txt", fileID );
    FILE *file = fopen( textFileName, "w" );
    
    fprintf( file, "%s", inDescription );
    
    fclose( file );
    delete [] textFileName;
    }
