#include "GamePage.h"

#include "message.h"

#include "serialWebRequests.h"

#include "minorGems/util/stringUtils.h"
#include "minorGems/game/game.h"



int GamePage::sPageCount = 0;

SpriteHandle GamePage::sWaitingSprites[3] = { NULL, NULL, NULL };
SpriteHandle GamePage::sResponseWarningSprite = NULL;

int GamePage::sCurrentWaitingSprite = 0;
int GamePage::sLastWaitingSprite = -1;
int GamePage::sWaitingSpriteDirection = 1;
double GamePage::sCurrentWaitingSpriteFade = 0;

char GamePage::sResponseWarningShowing = false;
doublePair GamePage::sResponseWarningPosition = { 0, 0 };


double GamePage::sWaitingFade = 0;
char GamePage::sWaiting = false;
char GamePage::sShowWaitingWarningOnly = false;

char GamePage::sShutdownPendingWarning = false;


extern int currentActiveSerialWebRequest;



GamePage::GamePage()
        : PageComponent( 0, 0 ),
          mStatusError( false ),
          mStatusMessageKey( NULL ),
          mStatusMessage( NULL ),
          mTip( NULL ),
          mLastTip( NULL ),
          mLastTipFade( 1 ),
          mTipAtTopOfScreen( false ),
          mResponseWarningTipShowing( false ) {

    if( sWaitingSprites[0] == NULL ) {
        sWaitingSprites[0] = loadSprite( "loading.tga", true );
        sWaitingSprites[1] = loadSprite( "loading2.tga", true );
        sWaitingSprites[2] = loadSprite( "loading3.tga", true );

        sResponseWarningSprite = loadSprite( "responseWarning.tga", true );
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
    if( mLastTip != NULL ) {
        delete [] mLastTip;
        }
    
    sPageCount--;
    if( sPageCount == 0 ) {
        freeSprite( sWaitingSprites[0] );
        freeSprite( sWaitingSprites[1] );
        freeSprite( sWaitingSprites[2] );
        freeSprite( sResponseWarningSprite );
        
        sWaitingSprites[0] = NULL;
        sWaitingSprites[1] = NULL;
        sWaitingSprites[2] = NULL;
        sResponseWarningSprite = NULL;
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



char GamePage::isStatusShowing() {
    return ( mStatusMessage != NULL || mStatusMessageKey != NULL );
    }



void GamePage::setToolTip( const char *inTip ) {
    if( mTip != NULL && inTip == NULL ) {
        // tip disappearing, save it as mLastTip
        if( mLastTip != NULL ) {
            delete [] mLastTip;
            }
        mLastTip = mTip;
        mLastTipFade = 1.0;
        }
    else if( mTip != NULL ) {
        delete [] mTip;
        }
        
    if( inTip != NULL ) {
        mTip = stringDuplicate( inTip );
        }
    else {
        mTip = NULL;
        }

    mResponseWarningTipShowing = false;
    }



void GamePage::clearToolTip( const char *inTipToClear ) {
    if( mTip != NULL ) {
        if( strcmp( mTip, inTipToClear ) == 0 ) {

            // tip disappearing, save it as mLastTip
            if( mLastTip != NULL ) {
                delete [] mLastTip;
                }
            mLastTip = mTip;
            mLastTipFade = 1.0;
            
            mTip = NULL;
            }
        }
    }



void GamePage::setTipPosition( char inTop ) {
    mTipAtTopOfScreen = inTop;
    }




void GamePage::base_draw( doublePair inViewCenter, 
                          double inViewSize ){

    if( sShutdownPendingWarning ) {
        // skip drawing current page and draw warning instead

        doublePair labelPos = { 0, 0 };
        
        drawMessage( "shutdownPendingWarning", labelPos );
        
        return;
        }

    
    drawUnderComponents( inViewCenter, inViewSize );


    PageComponent::base_draw( inViewCenter, inViewSize );
    

    if( mStatusMessageKey != NULL ) {
        doublePair labelPos = { 0, -5 };
        
        drawMessage( mStatusMessageKey, labelPos, mStatusError );
        }
    else if( mStatusMessage != NULL ) {
        doublePair labelPos = { 0, -5 };
        
        drawMessage( mStatusMessage, labelPos, mStatusError );
        }


    doublePair tipPosition = { 0, -7 };

    if( mTipAtTopOfScreen ) {
        tipPosition.y *= -1;
        }
    

    if( mTip != NULL ) {
        drawMessage( mTip, tipPosition );
        }
    else if( mLastTip != NULL && mLastTipFade > 0 ) {
        drawMessage( mLastTip, tipPosition, false, mLastTipFade );
        }
    
    if( sWaitingFade > 0 ) {
        
        doublePair spritePos = { 9.25, 7 };
        
        float r = 1;
        float g = 1;
        float b = 1;

        char showWarningIcon = false;
        
        if( sWaiting && currentActiveSerialWebRequest != -1 ) {
            
            switch( getWebRequestRetryStatus( 
                        currentActiveSerialWebRequest ) ) {
                case 0:
                    break;
                case 1:
                    g = 0.4666;
                    b = 0;
                    break;
                default:
                    g = 0;
                    b = 0;
                    showWarningIcon = true;
                    break;
                }
            }
        

        setDrawColor( r, g, b, 
                      sWaitingFade * sCurrentWaitingSpriteFade );

        drawSprite( sWaitingSprites[sCurrentWaitingSprite], 
                    spritePos, 1/16.0 );

        if( sLastWaitingSprite != -1 ) {
            
            setDrawColor( r, g, b, 
                          sWaitingFade * ( 1 - sCurrentWaitingSpriteFade ) );
            
            drawSprite( sWaitingSprites[sLastWaitingSprite], 
                        spritePos, 1/16.0 );
            }


        if( showWarningIcon && ! makeWaitingIconSmall() ) {
            spritePos.y -= 0.5;
            
            setDrawColor( r, g, b, sWaitingFade );
            
            drawSprite( sResponseWarningSprite, spritePos, 1/16.0 );
            sResponseWarningShowing = true;
            sResponseWarningPosition = spritePos;
            }
        else if( showWarningIcon ) {
            // should show warning, but not enough room (small icon)
            
            // still show tool tip centered on small icon
            sResponseWarningShowing = true;
            sResponseWarningPosition = spritePos;
            }
        }
    
    draw( inViewCenter, inViewSize );
    }


extern double frameRateFactor;


void GamePage::base_step() {
    if( sShutdownPendingWarning ) {
        // skip stepping stuff so that game doesn't advance
        // while the user's view is obscured
        return;
        }
    
    PageComponent::base_step();


    mLastTipFade -= 0.025 * frameRateFactor;
    if( mLastTipFade < 0 ) {
        mLastTipFade = 0;
        }
    

    if( sWaiting 
        && 
        canShowWaitingIcon() 
        &&
        ( ! sShowWaitingWarningOnly 
          || 
          getWebRequestRetryStatus( currentActiveSerialWebRequest ) > 0 ) ) {
        
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
    
    // skip animation if not visible
    if( sWaitingFade > 0 ) {
        
        sCurrentWaitingSpriteFade += 0.025 * frameRateFactor;
        if( sCurrentWaitingSpriteFade > 1 ) {
            sCurrentWaitingSpriteFade = 0;

            switch( sCurrentWaitingSprite ) {
                case 0:
                    sCurrentWaitingSprite = 1;
                    sLastWaitingSprite = 0;
                    sWaitingSpriteDirection = 1;
                    break;
                case 1:
                    sCurrentWaitingSprite += sWaitingSpriteDirection;
                    sLastWaitingSprite = 1;
                    break;
                case 2:
                    sCurrentWaitingSprite = 1;
                    sLastWaitingSprite = 2;
                    sWaitingSpriteDirection = -1;
                    break;
                }
            }
        }
    else {
        // reset animation
        sCurrentWaitingSprite = 0;
        sLastWaitingSprite = -1;
        sCurrentWaitingSpriteFade = 0;
        sWaitingSpriteDirection = 1;
        }
        
    }



void GamePage::showShutdownPendingWarning() {
    sShutdownPendingWarning = true;
    setIgnoreEvents( true );
    }



void GamePage::base_keyDown( unsigned char inASCII ) {
    PageComponent::base_keyDown( inASCII );
    
    if( sShutdownPendingWarning && inASCII == ' ' ) {
        sShutdownPendingWarning = false;
        setIgnoreEvents( false );
        }
    }




void GamePage::base_makeActive( char inFresh ){
    if( inFresh ) {    
        for( int i=0; i<mComponents.size(); i++ ) {
            PageComponent *c = *( mComponents.getElement( i ) );
            
            c->base_clearState();
            }

        // don't show lingering tool tips from last time page was shown
        mLastTipFade = 0;
        if( mTip != NULL ) {
            delete [] mTip;
            mTip = NULL;
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




void GamePage::setWaiting( char inWaiting, char inWarningOnly ) {
    sWaiting = inWaiting;
    sShowWaitingWarningOnly = inWarningOnly;
    
    if( sWaiting == false && mResponseWarningTipShowing ) {
        setToolTip( NULL );
        }
    }




void GamePage::pointerMove( float inX, float inY ) {
    if( sResponseWarningShowing && currentActiveSerialWebRequest != -1 ) {
        
        if( fabs( inX - sResponseWarningPosition.x ) < 0.375 &&
            fabs( inY - sResponseWarningPosition.y ) < 0.375 ) {
            
            int status = 
                getWebRequestRetryStatus( currentActiveSerialWebRequest );
            
            if( status >= 2 ) {
                

                char *tipString =
                    autoSprintf( translate( "responseWarning" ),
                                 status - 1 );
                
            
                setToolTip( tipString );

                mResponseWarningTipShowing = true;

                delete [] tipString;
                }
            }
        else {
            setToolTip( NULL );
            mResponseWarningTipShowing = false;
            }
        }
    }

    




