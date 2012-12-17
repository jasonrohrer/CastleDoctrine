#include "GamePage.h"

#include "message.h"


#include "minorGems/util/stringUtils.h"



int GamePage::sPageCount = 0;
SpriteHandle GamePage::sWaitingSprite = NULL;
double GamePage::sWaitingFade = 0;
char GamePage::sWaiting = false;



GamePage::GamePage()
        : PageComponent( 0, 0 ),
          mStatusError( false ),
          mStatusMessageKey( NULL ),
          mStatusMessage( NULL ),
          mTipKey( NULL ),
          mTip( NULL ) {

    if( sWaitingSprite == NULL ) {
        sWaitingSprite = loadSprite( "loading.tga", true );
        }
    sPageCount++;
    }




GamePage::~GamePage() {
    if( mStatusMessage != NULL ) {
        delete [] mStatusMessage;
        }
    if( mTip != NULL ) {
        delete [] mTip;
        }
    sPageCount--;
    if( sPageCount == 0 ) {
        freeSprite( sWaitingSprite );
        sWaitingSprite = NULL;
        }
    }



void GamePage::setStatus( const char *inStatusMessageKey, char inError ) {
    mStatusMessageKey = inStatusMessageKey;
    mStatusError = inError;

    if( mStatusMessage != NULL ) {
        delete [] mStatusMessage;
        mStatusMessage = NULL;
        }
    }



void GamePage::setStatusDirect( char *inStatusMessage, char inError ) {
    if( mStatusMessage != NULL ) {
        delete [] mStatusMessage;
        mStatusMessage = NULL;
        }
    
    if( inStatusMessage != NULL ) {
        mStatusMessage = stringDuplicate( inStatusMessage );
        
        mStatusMessageKey = NULL;
        }
    
    mStatusError = inError;
    }



void GamePage::setToolTip( const char *inTipKey ) {
    mTipKey = inTipKey;

    if( mTip != NULL ) {
        delete [] mTip;
        mTip = NULL;
        }
    }



void GamePage::setToolTipDirect( const char *inTip ) {
    if( mTip != NULL ) {
        delete [] mTip;
        mTip = NULL;
        }
    
    if( inTip != NULL ) {
        mTip = stringDuplicate( inTip );
        
        mTipKey = NULL;
        }
    }



void GamePage::base_draw( doublePair inViewCenter, 
                          double inViewSize ){
    
    PageComponent::base_draw( inViewCenter, inViewSize );
    

    if( mStatusMessageKey != NULL ) {
        doublePair labelPos = { 0, -5 };
        
        drawMessage( mStatusMessageKey, labelPos, mStatusError );
        }
    else if( mStatusMessage != NULL ) {
        doublePair labelPos = { 0, -5 };
        
        drawMessage( mStatusMessage, labelPos, mStatusError );
        }


    if( mTipKey != NULL ) {
        doublePair labelPos = { 0, -7 };
        
        drawMessage( mTipKey, labelPos );
        }
    else if( mTip != NULL ) {
        doublePair labelPos = { 0, -7 };
        
        drawMessage( mTip, labelPos );
        }

    if( sWaitingFade > 0 ) {
        setDrawColor( 1, 1, 1, sWaitingFade );
        
        doublePair spritePos = { -8, 6 };
        
        drawSprite( sWaitingSprite, spritePos, 1/16.0 );
        }
    
    draw( inViewCenter, inViewSize );
    }


extern double frameRateFactor;


void GamePage::base_step() {
    PageComponent::base_step();

    if( sWaiting ) {
        sWaitingFade += 0.05 * frameRateFactor;
    
        if( sWaitingFade > 1 ) {
            sWaitingFade = 1;
            }
        }
    else {
        sWaitingFade -= 0.05 * frameRateFactor;
        if( sWaitingFade < 0 ) {
            sWaitingFade = 0;
            }
        }
    }




void GamePage::base_makeActive( char inFresh ){
    if( inFresh ) {    
        for( int i=0; i<mComponents.size(); i++ ) {
            PageComponent *c = *( mComponents.getElement( i ) );
            
            c->base_clearState();
            }
        }
    

    makeActive( inFresh );
    }



void GamePage::base_makeNotActive(){
    for( int i=0; i<mComponents.size(); i++ ) {
        PageComponent *c = *( mComponents.getElement( i ) );
        
        c->base_clearState();
        }
    
    makeNotActive();
    }




void GamePage::setWaiting( char inWaiting ) {
    sWaiting = inWaiting;
    }
    




