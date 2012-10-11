int versionNumber = 1;



#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

//#define USE_MALLINFO

#ifdef USE_MALLINFO
#include <malloc.h>
#endif


#include "minorGems/graphics/Color.h"




#include "minorGems/util/SimpleVector.h"
#include "minorGems/util/stringUtils.h"
#include "minorGems/util/SettingsManager.h"
#include "minorGems/util/random/CustomRandomSource.h"


CustomRandomSource randSource;



#include "minorGems/util/log/AppLog.h"

#include "minorGems/system/Time.h"


#include "minorGems/game/game.h"
#include "minorGems/game/gameGraphics.h"
#include "minorGems/game/Font.h"
#include "minorGems/game/drawUtils.h"




#include "LoginPage.h"
#include "CheckoutHousePage.h"
#include "EditHousePage.h"
#include "LoadBackpackPage.h"
#include "AuctionPage.h"
#include "BuyAuctionPage.h"
#include "CheckinHousePage.h"
#include "MenuPage.h"
#include "RobCheckoutHousePage.h"
#include "RobHousePage.h"
#include "RobCheckinHousePage.h"
#include "RobberyReplayMenuPage.h"
#include "FetchRobberyReplayPage.h"
#include "ReplayRobHousePage.h"
#include "StaleHousePage.h"


#include "houseObjects.h"
#include "houseTransitions.h"
#include "tools.h"
#include "galleryObjects.h"


GamePage *currentGamePage = NULL;


LoginPage *loginPage;
CheckoutHousePage *checkoutHousePage;
EditHousePage *editHousePage;
LoadBackpackPage *loadBackpackPage;
AuctionPage *auctionPage;
BuyAuctionPage *buyAuctionPage;
RobHousePage *selfHouseTestPage;
CheckinHousePage *checkinHousePage;
MenuPage *menuPage;
RobCheckoutHousePage *robCheckoutHousePage;
RobHousePage *robHousePage;
RobCheckinHousePage *robCheckinHousePage;
RobberyReplayMenuPage *robberyReplayMenuPage;
FetchRobberyReplayPage *fetchRobberyReplayPage;
ReplayRobHousePage *replayRobHousePage;
StaleHousePage *staleHousePage;
StaleHousePage *staleHouseDeadPage;


// position of view in world
doublePair lastScreenViewCenter = {0, 0 };



// world with of one view
double viewWidth = 20;

// fraction of viewWidth visible vertically (aspect ratio)
double viewHeightFraction;

int screenW, screenH;

float mouseSpeed;




double frameRateFactor = 1;


char firstDrawFrameCalled = false;




const char *getWindowTitle() {
    return "Game 10";
    }


const char *getFontTGAFileName() {
    return "font_8_16.tga";
    }


char isDemoMode() {
    return false;
    }


const char *getDemoCodeSharedSecret() {
    return "fundamental_right";
    }


const char *getDemoCodeServerURL() {
    return "http://FIXME/demoServer/server.php";
    }



char gamePlayingBack = false;


Font *mainFont;
Font *mainFontFixed;

Font *tinyFont;


char *serverURL = NULL;


char *userEmail = NULL;
int userID = -1;
char *downloadCode = NULL;
// each new request to server must use next sequence number
int serverSequenceNumber = -1;



static char wasPaused = false;
static float pauseScreenFade = 0;

static char *currentUserTypedMessage = NULL;



// for delete key repeat during message typing
static int holdDeleteKeySteps = -1;
static int stepsBetweenDeleteRepeat;





#define SETTINGS_HASH_SALT "another_loss"


static const char *customDataFormatWriteString = 
    "version%d_mouseSpeed%f_downloadCode%s_email%s";

static const char *customDataFormatReadString = 
    "version%d_mouseSpeed%f_downloadCode%10s_email%99s";


char *getCustomRecordedGameData() {    
    
    float mouseSpeedSetting = 
        SettingsManager::getFloatSetting( "mouseSpeed", 1.0f );
    
    char *email =
        SettingsManager::getStringSetting( "email" );
    if( email == NULL ) {
        email = stringDuplicate( "*" );
        }
    else {
        // put bogus email in recording files, since we don't
        // need a email during playback anyway (not communicating with 
        // server during playback)
        
        // want people to be able to share playback files freely without
        // divulging their emails
        delete [] email;

        email = stringDuplicate( "redacted@redacted.com" );
        }
    
    
    char *code = SettingsManager::getStringSetting( "downloadCode" );
    
    if( code == NULL ) {
        code = stringDuplicate( "**********" );
        }
    else {
        // put bogus code in recording files, since we don't
        // need a valid code during playback anyway (not communicating with 
        // server during playback)
        
        // want people to be able to share playback files freely without
        // divulging their download codes
        delete [] code;

        code = stringDuplicate( "EMPTYDEEPS" );
        }
    


    char * result = autoSprintf(
        customDataFormatWriteString,
        versionNumber, mouseSpeedSetting, code, email );

    delete [] email;
    delete [] code;
    

    return result;
    }



char showMouseDuringPlayback() {
    // since we rely on the system mouse pointer during the game (and don't
    // draw our own pointer), we need to see the recorded pointer position
    // to make sense of game playback
    return true;
    }



char *getHashSalt() {
    return stringDuplicate( SETTINGS_HASH_SALT );
    }








void initFrameDrawer( int inWidth, int inHeight, int inTargetFrameRate,
                      const char *inCustomRecordedGameData,
                      char inPlayingBack ) {

    gamePlayingBack = inPlayingBack;
    
    screenW = inWidth;
    screenH = inHeight;
    
    if( inTargetFrameRate != 60 ) {
        frameRateFactor = (double)60 / (double)inTargetFrameRate;
        }
    
    TextField::setDeleteRepeatDelays( (int)( 30 / frameRateFactor ),
                                      (int)( 2 / frameRateFactor ) );
    


    setViewCenterPosition( lastScreenViewCenter.x, lastScreenViewCenter.y );

    viewHeightFraction = inHeight / (double)inWidth;

    // monitors vary in width relative to height
    // keep visible vertical view span constant (15)
    // which is what it would be for a view width of 20 at a 4:3 aspect
    // ratio
    viewWidth = 15 * 1.0 / viewHeightFraction;
    
    
    setViewSize( viewWidth );


    
    

    

    setCursorVisible( true );
    grabInput( false );
    
    // world coordinates
    setMouseReportingMode( true );
    
    if( !inPlayingBack ) {    
        int x,y;
        warpMouseToCenter( &x, &y );
        }
    
    
    
    mainFont = new Font( getFontTGAFileName(), 1, 4, false );
    mainFontFixed = new Font( getFontTGAFileName(), 1, 4, true );
    
    tinyFont = new Font( "font_4_8.tga", 1, 2, false );


    float mouseSpeedSetting = 1.0f;

    userEmail = new char[100];
    downloadCode = new char[11];
    
    int readVersionNumber;
    
    int numRead = sscanf( inCustomRecordedGameData, 
                          customDataFormatReadString, 
                          &readVersionNumber,
                          &mouseSpeedSetting, 
                          downloadCode,
                          userEmail );
    if( numRead != 4 ) {
        // no recorded game?
        }
    else {

        if( readVersionNumber != versionNumber ) {
            AppLog::printOutNextMessage();
            AppLog::warningF( 
                "WARNING:  version number in playback file is %d "
                "but game version is %d...",
                readVersionNumber, versionNumber );
            }

        if( strcmp( downloadCode, "**********" ) == 0 ) {
            delete [] downloadCode;
            downloadCode = NULL;
            }
        if( strcmp( userEmail, "*" ) == 0 ) {
            delete [] userEmail;
            userEmail = NULL;
            }
        }
    
    if( !inPlayingBack ) {
        // read REAL email and download code from settings file

        delete [] userEmail;
        
        userEmail = SettingsManager::getStringSetting( "email" );    

        
        delete [] downloadCode;
        
        downloadCode = SettingsManager::getStringSetting( "downloadCode" );    
        }
    

    
    double mouseParam = 0.000976562;

    mouseParam *= mouseSpeedSetting;

    mouseSpeed = mouseParam * inWidth / viewWidth;



    serverURL = SettingsManager::getStringSetting( "serverURL" );

    if( serverURL == NULL ) {
        serverURL = 
            stringDuplicate( 
                "http://localhost/jcr13/castleServer/server.php" );
        }


    initHouseObjects();    
    initTools();
    initGalleryObjects();
    
    initHouseTransitions();
    

    loginPage = new LoginPage();
    checkoutHousePage = new CheckoutHousePage();
    editHousePage = new EditHousePage();
    loadBackpackPage = new LoadBackpackPage();
    auctionPage = new AuctionPage();
    buyAuctionPage = new BuyAuctionPage();
    
    selfHouseTestPage = new RobHousePage();
    selfHouseTestPage->showBackpack( false );
    
    checkinHousePage = new CheckinHousePage();
    menuPage = new MenuPage();
    robCheckoutHousePage = new RobCheckoutHousePage();
    robHousePage = new RobHousePage();
    robCheckinHousePage = new RobCheckinHousePage();
    robberyReplayMenuPage = new RobberyReplayMenuPage();
    fetchRobberyReplayPage = new FetchRobberyReplayPage();
    replayRobHousePage = new ReplayRobHousePage();
    staleHousePage = new StaleHousePage( false );
    staleHouseDeadPage = new StaleHousePage( true );

    currentGamePage = loginPage;

    currentGamePage->base_makeActive( true );
    }


void freeFrameDrawer() {
    delete mainFont;
    delete mainFontFixed;
    delete tinyFont;
    
    if( currentUserTypedMessage != NULL ) {
        delete [] currentUserTypedMessage;
        currentUserTypedMessage = NULL;
        }


    currentGamePage = NULL;
    delete loginPage;
    delete checkoutHousePage;
    delete editHousePage;
    delete loadBackpackPage;
    delete auctionPage;
    delete buyAuctionPage;
    delete selfHouseTestPage;
    delete checkinHousePage;
    delete menuPage;
    delete robCheckoutHousePage;
    delete robHousePage;
    delete robCheckinHousePage;
    delete robberyReplayMenuPage;
    delete fetchRobberyReplayPage;
    delete replayRobHousePage;
    delete staleHousePage;
    delete staleHouseDeadPage;


    freeHouseObjects();
    freeHouseTransitions();
    
    freeTools();
    freeGalleryObjects();

    if( serverURL != NULL ) {
        delete [] serverURL;
        serverURL = NULL;
        }

    if( downloadCode != NULL ) {
        delete [] downloadCode;
        downloadCode = NULL;
        }
    
    if( userEmail != NULL ) {
        delete [] userEmail;
        userEmail = NULL;
        }
    

    }





    


// draw code separated from updates
// some updates are still embedded in draw code, so pass a switch to 
// turn them off
static void drawFrameNoUpdate( char inUpdate );




static void drawPauseScreen() {

    double viewHeight = viewHeightFraction * viewWidth;

    setDrawColor( 1, 1, 1, 0.5 * pauseScreenFade );
        
    drawSquare( lastScreenViewCenter, 1.05 * ( viewHeight / 3 ) );
        

    setDrawColor( 0.2, 0.2, 0.2, 0.85 * pauseScreenFade  );
        
    drawSquare( lastScreenViewCenter, viewHeight / 3 );
        

    setDrawColor( 1, 1, 1, pauseScreenFade );

    doublePair messagePos = lastScreenViewCenter;

    messagePos.y += 4.5;

    mainFont->drawString( translate( "pauseMessage1" ), 
                           messagePos, alignCenter );
        
    messagePos.y -= 1.25 * (viewHeight / 15);
    mainFont->drawString( translate( "pauseMessage2" ), 
                           messagePos, alignCenter );

    if( currentUserTypedMessage != NULL ) {
            
        messagePos.y -= 1.25 * (viewHeight / 15);
            
        double maxWidth = 0.95 * ( viewHeight / 1.5 );
            
        int maxLines = 9;

        SimpleVector<char *> *tokens = 
            tokenizeString( currentUserTypedMessage );


        // collect all lines before drawing them
        SimpleVector<char *> lines;
        
            
        while( tokens->size() > 0 ) {

            // build up a a line

            // always take at least first token, even if it is too long
            char *currentLineString = 
                stringDuplicate( *( tokens->getElement( 0 ) ) );
                
            delete [] *( tokens->getElement( 0 ) );
            tokens->deleteElement( 0 );
            
            

            

            
            char nextTokenIsFileSeparator = false;
                
            char *nextLongerString = NULL;
                
            if( tokens->size() > 0 ) {

                char *nextToken = *( tokens->getElement( 0 ) );
                
                if( nextToken[0] == 28 ) {
                    nextTokenIsFileSeparator = true;
                    }
                else {
                    nextLongerString =
                        autoSprintf( "%s %s ",
                                     currentLineString,
                                     *( tokens->getElement( 0 ) ) );
                    }
                
                }
                
            while( !nextTokenIsFileSeparator 
                   &&
                   nextLongerString != NULL 
                   && 
                   mainFont->measureString( nextLongerString ) 
                   < maxWidth 
                   &&
                   tokens->size() > 0 ) {
                    
                delete [] currentLineString;
                    
                currentLineString = nextLongerString;
                    
                nextLongerString = NULL;
                    
                // token consumed
                delete [] *( tokens->getElement( 0 ) );
                tokens->deleteElement( 0 );
                    
                if( tokens->size() > 0 ) {
                    
                    char *nextToken = *( tokens->getElement( 0 ) );
                
                    if( nextToken[0] == 28 ) {
                        nextTokenIsFileSeparator = true;
                        }
                    else {
                        nextLongerString =
                            autoSprintf( "%s%s ",
                                         currentLineString,
                                         *( tokens->getElement( 0 ) ) );
                        }
                    }
                }
                
            if( nextLongerString != NULL ) {    
                delete [] nextLongerString;
                }
                
            while( mainFont->measureString( currentLineString ) > 
                   maxWidth ) {
                    
                // single token that is too long by itself
                // simply trim it and discard part of it 
                // (user typing nonsense anyway)
                    
                currentLineString[ strlen( currentLineString ) - 1 ] =
                    '\0';
                }
                
            if( currentLineString[ strlen( currentLineString ) - 1 ] 
                == ' ' ) {
                // trim last bit of whitespace
                currentLineString[ strlen( currentLineString ) - 1 ] = 
                    '\0';
                }

                
            lines.push_back( currentLineString );

            
            if( nextTokenIsFileSeparator ) {
                // file separator

                // put a paragraph separator in
                lines.push_back( stringDuplicate( "---" ) );

                // token consumed
                delete [] *( tokens->getElement( 0 ) );
                tokens->deleteElement( 0 );
                }
            }   


        // all tokens deleted above
        delete tokens;


        double messageLineSpacing = 0.625 * (viewHeight / 15);
        
        int numLinesToSkip = lines.size() - maxLines;

        if( numLinesToSkip < 0 ) {
            numLinesToSkip = 0;
            }
        
        
        for( int i=0; i<numLinesToSkip-1; i++ ) {
            char *currentLineString = *( lines.getElement( i ) );
            delete [] currentLineString;
            }
        
        int lastSkipLine = numLinesToSkip - 1;

        if( lastSkipLine >= 0 ) {
            
            char *currentLineString = *( lines.getElement( lastSkipLine ) );

            // draw above and faded out somewhat

            doublePair lastSkipLinePos = messagePos;
            
            lastSkipLinePos.y += messageLineSpacing;

            setDrawColor( 1, 1, 0.5, 0.125 * pauseScreenFade );

            mainFont->drawString( currentLineString, 
                                   lastSkipLinePos, alignCenter );

            
            delete [] currentLineString;
            }
        

        setDrawColor( 1, 1, 0.5, pauseScreenFade );

        for( int i=numLinesToSkip; i<lines.size(); i++ ) {
            char *currentLineString = *( lines.getElement( i ) );
            
            if( false && lastSkipLine >= 0 ) {
            
                if( i == numLinesToSkip ) {
                    // next to last
                    setDrawColor( 1, 1, 0.5, 0.25 * pauseScreenFade );
                    }
                else if( i == numLinesToSkip + 1 ) {
                    // next after that
                    setDrawColor( 1, 1, 0.5, 0.5 * pauseScreenFade );
                    }
                else if( i == numLinesToSkip + 2 ) {
                    // rest are full fade
                    setDrawColor( 1, 1, 0.5, pauseScreenFade );
                    }
                }
            
            mainFont->drawString( currentLineString, 
                                   messagePos, alignCenter );

            delete [] currentLineString;
                
            messagePos.y -= messageLineSpacing;
            }
        }
        
        

    setDrawColor( 1, 1, 1, pauseScreenFade );

    messagePos = lastScreenViewCenter;

    messagePos.y -= 3.75 * ( viewHeight / 15 );
    //mainFont->drawString( translate( "pauseMessage3" ), 
    //                      messagePos, alignCenter );

    messagePos.y -= 0.625 * (viewHeight / 15);
    mainFont->drawString( translate( "pauseMessage3" ), 
                           messagePos, alignCenter );

    }



void deleteCharFromUserTypedMessage() {
    if( currentUserTypedMessage != NULL ) {
                    
        int length = strlen( currentUserTypedMessage );
        
        char fileSeparatorDeleted = false;
        if( length > 2 ) {
            if( currentUserTypedMessage[ length - 2 ] == 28 ) {
                // file separator with spaces around it
                // delete whole thing with one keypress
                currentUserTypedMessage[ length - 3 ] = '\0';
                fileSeparatorDeleted = true;
                }
            }
        if( !fileSeparatorDeleted && length > 0 ) {
            currentUserTypedMessage[ length - 1 ] = '\0';
            }
        }
    }




void drawFrame( char inUpdate ) {
    

    // disable keyboard mapping while paused and on login page
    if( isPaused() || currentGamePage == loginPage ) {
        toggleKeyMapping( false );
        }
    else {
        toggleKeyMapping( true );
        }
    


    if( !inUpdate ) {
        drawFrameNoUpdate( false );
            
        drawPauseScreen();
        
        if( !wasPaused ) {
            if( currentGamePage != NULL ) {
                currentGamePage->base_makeNotActive();
                }
            }
        wasPaused = true;

        // handle delete key repeat
        if( holdDeleteKeySteps > -1 ) {
            holdDeleteKeySteps ++;
            
            if( holdDeleteKeySteps > stepsBetweenDeleteRepeat ) {        
                // delete repeat

                // platform layer doesn't receive event for key held down
                // tell it we are still active so that it doesn't
                // reduce the framerate during long, held deletes
                wakeUpPauseFrameRate();
                


                // subtract from messsage
                deleteCharFromUserTypedMessage();
                
                            

                // shorter delay for subsequent repeats
                stepsBetweenDeleteRepeat = (int)( 2/ frameRateFactor );
                holdDeleteKeySteps = 0;
                }
            }

        // fade in pause screen
        if( pauseScreenFade < 1 ) {
            pauseScreenFade += ( 1.0 / 30 ) * frameRateFactor;
        
            if( pauseScreenFade > 1 ) {
                pauseScreenFade = 1;
                }
            }
        

        return;
        }


    // not paused


    // fade pause screen out
    if( pauseScreenFade > 0 ) {
        pauseScreenFade -= ( 1.0 / 30 ) * frameRateFactor;
        
        if( pauseScreenFade < 0 ) {
            pauseScreenFade = 0;

            if( currentUserTypedMessage != NULL ) {

                // make sure it doesn't already end with a file separator
                // (never insert two in a row, even when player closes
                //  pause screen without typing anything)
                int lengthCurrent = strlen( currentUserTypedMessage );

                if( lengthCurrent < 2 ||
                    currentUserTypedMessage[ lengthCurrent - 2 ] != 28 ) {
                         
                        
                    // insert at file separator (ascii 28)
                    
                    char *oldMessage = currentUserTypedMessage;
                    
                    currentUserTypedMessage = autoSprintf( "%s %c ", 
                                                           oldMessage,
                                                           28 );
                    delete [] oldMessage;
                    }
                }
            }
        }    
    
    

    if( !firstDrawFrameCalled ) {
        
        // do final init step... stuff that shouldn't be done until
        // we have control of screen
        
        char *moveKeyMapping = 
            SettingsManager::getStringSetting( "upLeftDownRightKeys" );
    
        if( moveKeyMapping != NULL ) {
            char *temp = stringToLowerCase( moveKeyMapping );
            delete [] moveKeyMapping;
            moveKeyMapping = temp;
        
            if( strlen( moveKeyMapping ) == 4 &&
                strcmp( moveKeyMapping, "wasd" ) != 0 ) {
                // different assignment

                mapKey( moveKeyMapping[0], 'w' );
                mapKey( moveKeyMapping[1], 'a' );
                mapKey( moveKeyMapping[2], 's' );
                mapKey( moveKeyMapping[3], 'd' );
                }
            delete [] moveKeyMapping;
            }


        firstDrawFrameCalled = true;
        }

    
    if( currentGamePage != NULL ) {
        
        if( wasPaused ) {
            currentGamePage->base_makeActive( false );
            wasPaused = false;
            }

        currentGamePage->base_step();

        if( currentGamePage == loginPage ) {
            if( loginPage->getLoginDone() ) {
                
                currentGamePage = checkoutHousePage;
                currentGamePage->base_makeActive( true );
                }
            }
        else if( currentGamePage == checkoutHousePage ) {
            if( checkoutHousePage->getReturnToMenu() ) {
                currentGamePage = menuPage;
                currentGamePage->base_makeActive( true );
                }
            else {
                char *houseMap = checkoutHousePage->getHouseMap();
                char *vaultContents = checkoutHousePage->getVaultContents();
                char *backpackContents = 
                    checkoutHousePage->getBackpackContents();
                char *galleryContents = 
                    checkoutHousePage->getGalleryContents();
                char *priceList = checkoutHousePage->getPriceList();
                int lootValue = checkoutHousePage->getLootValue();
                char mustSelfTest = checkoutHousePage->getMustSelfTest();
                
                if( houseMap != NULL && 
                    vaultContents != NULL &&
                    backpackContents != NULL && 
                    galleryContents != NULL && 
                    priceList != NULL ) {
                    
                    editHousePage->setHouseMap( houseMap );
                    editHousePage->setVaultContents( vaultContents );
                    editHousePage->setBackpackContents( backpackContents );
                    editHousePage->setGalleryContents( galleryContents );
                    editHousePage->setPriceList( priceList );
                    editHousePage->setLootValue( lootValue );
                    editHousePage->setMustSelfTest( mustSelfTest );
                    
                    // empty, because house just checked out an nothing
                    // bought yet
                    editHousePage->setPurchaseList( "#" );
                    

                    delete [] houseMap;
                    delete [] vaultContents;
                    delete [] backpackContents;
                    delete [] galleryContents;
                    delete [] priceList;

                    currentGamePage = editHousePage;
                    currentGamePage->base_makeActive( true );
                    }
                else {
                    if( houseMap != NULL ) {
                        delete [] houseMap;
                        }
                    if( vaultContents != NULL ) {
                        delete [] vaultContents;
                        }
                    if( backpackContents != NULL ) {
                        delete [] backpackContents;
                        }
                    if( galleryContents != NULL ) {
                        delete [] galleryContents;
                        }
                    if( priceList != NULL ) {
                        delete [] priceList;
                        }
                    }
                }
            }
        else if( currentGamePage == editHousePage ) {
            if( editHousePage->isStale() ) {
                currentGamePage = staleHousePage;
                currentGamePage->base_makeActive( true );
                }
            else if( editHousePage->showLoadBackpack() ) {
                char *vaultContents = editHousePage->getVaultContents();
                char *backpackContents = editHousePage->getBackpackContents();
                char *purchaseList = editHousePage->getPurchaseList();
                char *priceList = editHousePage->getPriceList();
                
                loadBackpackPage->setLootValue( 
                    editHousePage->getLootValue() );
                loadBackpackPage->setVaultContents( vaultContents );
                loadBackpackPage->setBackpackContents( backpackContents );
                loadBackpackPage->setPurchaseList( purchaseList );
                loadBackpackPage->setPriceList( priceList );
                
                currentGamePage = loadBackpackPage;
                currentGamePage->base_makeActive( true );

                delete [] vaultContents;
                delete [] backpackContents;
                delete [] purchaseList;
                delete [] priceList;
                }
            else if( editHousePage->showAuctions() ) {
                // FIXME
                // char *galleryContents = editHousePage->getGalleryContents();
                
                // auctionPage->setGalleryContents( galleryContents );
                // delete [] galleryContents;

                auctionPage->setLootValue( editHousePage->getLootValue() );
                                
                currentGamePage = auctionPage;
                currentGamePage->base_makeActive( true );
                }
            else if( editHousePage->getDone() ) {

                char *houseMap = editHousePage->getHouseMap();
                char *vaultContents = editHousePage->getVaultContents();
                char *backpackContents = editHousePage->getBackpackContents();
                char *galleryContents = editHousePage->getGalleryContents();
                char *editList = editHousePage->getEditList();
                char *purchaseList = editHousePage->getPurchaseList();
                char *priceList = editHousePage->getPriceList();
                
                if( editHousePage->houseMapChanged() ) {
                    // force player to test own house first
                    selfHouseTestPage->setHouseMap( houseMap );
                    
                    selfHouseTestPage->setGalleryContents( galleryContents );
                    
                    selfHouseTestPage->setDescription( 
                        translate( "selfTestDescription" ) );

                    currentGamePage = selfHouseTestPage;
                    currentGamePage->base_makeActive( true );
                    selfHouseTestPage->startSelfTest();
                    }
                else {
                    // not changed, check it right in
                
                    checkinHousePage->setHouseMap( houseMap );
                    checkinHousePage->setVaultContents( vaultContents );
                    checkinHousePage->setBackpackContents( backpackContents );
                    checkinHousePage->setGalleryContents( galleryContents );
                    checkinHousePage->setEditList( editList );
                    checkinHousePage->setPurchaseList( purchaseList );
                    checkinHousePage->setPriceList( priceList );
                    
                    currentGamePage = checkinHousePage;
                    currentGamePage->base_makeActive( true );
                    }

                delete [] houseMap;
                delete [] vaultContents;
                delete [] backpackContents;
                delete [] galleryContents;
                delete [] editList;
                delete [] purchaseList;
                delete [] priceList;
                }
            }
        else if( currentGamePage == staleHousePage ) {
            if( staleHousePage->getDone() ) {
                // house edit became stale
                
                // return to own house
                currentGamePage = checkoutHousePage;
                currentGamePage->base_makeActive( true );
                }
            }
        else if( currentGamePage == staleHouseDeadPage ) {
            if( staleHouseDeadPage->getDone() ) {
                // either robbery or house self-test became stale
                
                // return to own house in either case
                currentGamePage = checkoutHousePage;
                currentGamePage->base_makeActive( true );
                }
            }
        else if( currentGamePage == loadBackpackPage ) {
            if( loadBackpackPage->getDone() ) {
                // done changing backpack, back to editor
                
                char *vaultContents = loadBackpackPage->getVaultContents();
                char *backpackContents = 
                    loadBackpackPage->getBackpackContents();
                char *purchaseList = loadBackpackPage->getPurchaseList();
                
                editHousePage->setVaultContents( vaultContents );
                editHousePage->setBackpackContents( backpackContents );
                editHousePage->setPurchaseList( purchaseList );
                
                editHousePage->setLootValue( 
                    loadBackpackPage->getLootValue() );


                delete [] vaultContents;
                delete [] backpackContents;
                delete [] purchaseList;
                
                // back to editing
                currentGamePage = editHousePage;
                currentGamePage->base_makeActive( true );
                }
            }
        else if( currentGamePage == auctionPage ) {
            if( auctionPage->getDone() ) {
                // done with auctions, back to editor
                currentGamePage = editHousePage;
                currentGamePage->base_makeActive( true );
                }
            else if( auctionPage->getBuy() ) {
                buyAuctionPage->setObject( auctionPage->getBoughtObject() );
                buyAuctionPage->setLootValue( editHousePage->getLootValue() );

                char *galleryContents = editHousePage->getGalleryContents();
                buyAuctionPage->setGalleryContents( galleryContents );
                delete [] galleryContents;

                currentGamePage = buyAuctionPage;
                currentGamePage->base_makeActive( true );
                }
            }
        else if( currentGamePage == buyAuctionPage ) {
            if( buyAuctionPage->getReturnToHome() ) {                
                char *galleryContents = buyAuctionPage->getGalleryContents();
                editHousePage->setGalleryContents( galleryContents );
                delete [] galleryContents;
                                
                editHousePage->setLootValue( buyAuctionPage->getLootValue() );

                currentGamePage = editHousePage;
                currentGamePage->base_makeActive( true );
                }
            }
        else if( currentGamePage == selfHouseTestPage ) {
            if( selfHouseTestPage->isStale() ) {
                currentGamePage = staleHouseDeadPage;
                currentGamePage->base_makeActive( true );
                }
            else if( selfHouseTestPage->getDone() ) {
                char *houseMap = editHousePage->getHouseMap();
                char *vaultContents = editHousePage->getVaultContents();
                char *backpackContents = editHousePage->getBackpackContents();
                char *galleryContents = editHousePage->getGalleryContents();
                char *editList = editHousePage->getEditList();
                char *purchaseList = editHousePage->getPurchaseList();
                char *priceList = editHousePage->getPriceList();
                
                int testResult = selfHouseTestPage->getSuccess();

                if( testResult == 1 ) {
                    
                    // house passed by owner (reached vault), okay to check in
                    checkinHousePage->setHouseMap( houseMap );
                    checkinHousePage->setVaultContents( vaultContents );
                    checkinHousePage->setBackpackContents( backpackContents );
                    checkinHousePage->setGalleryContents( galleryContents );
                    checkinHousePage->setEditList( editList );
                    checkinHousePage->setPurchaseList( purchaseList );
                    checkinHousePage->setPriceList( priceList );
                    checkinHousePage->setDied( 0 );
                    
                    currentGamePage = checkinHousePage;
                    currentGamePage->base_makeActive( true );
                    }
                else if( testResult == 0 ) {
                    // died while testing
                    // check in
                    checkinHousePage->setHouseMap( houseMap );
                    checkinHousePage->setVaultContents( vaultContents );
                    checkinHousePage->setBackpackContents( backpackContents );
                    checkinHousePage->setGalleryContents( galleryContents );
                    checkinHousePage->setEditList( editList );
                    checkinHousePage->setPurchaseList( purchaseList );
                    checkinHousePage->setPriceList( priceList );
                    checkinHousePage->setDied( 1 );
                    
                    currentGamePage = checkinHousePage;
                    currentGamePage->base_makeActive( true );
                    }
                else {
                    // back to editing it
                    
                    // don't SET it again, because we want to keep
                    // track of the fact that it has changed since the
                    // last valid checkin
                    // editHousePage->setHouseMap( houseMap );
                    
                    currentGamePage = editHousePage;
                    currentGamePage->base_makeActive( true );
                    editHousePage->endSelfTest();
                    }
                delete [] houseMap;
                delete [] vaultContents;
                delete [] backpackContents;
                delete [] galleryContents;
                delete [] editList;
                delete [] purchaseList;
                delete [] priceList;
                }
            }
        else if( currentGamePage == checkinHousePage ) {
            if( checkinHousePage->getReturnToMenu() ) {
                currentGamePage = menuPage;
                currentGamePage->base_makeActive( true );
                }
            else if( checkinHousePage->getStartOver() ) {
                // fresh start, back to editing empty house
                currentGamePage = checkoutHousePage;
                currentGamePage->base_makeActive( true );
                }
            }
        else if( currentGamePage == menuPage ) {
            if( menuPage->getStartEditHouse() ) {
                currentGamePage = checkoutHousePage;
                currentGamePage->base_makeActive( true );
                }
            if( menuPage->getShowReplayList() ) {
                currentGamePage = robberyReplayMenuPage;
                currentGamePage->base_makeActive( true );
                }
            else if( menuPage->getStartRobHouse() ) {
                HouseRecord *r = menuPage->getSelectedHouse();
                
                if( r != NULL ) {
                    currentGamePage = robCheckoutHousePage;
                    robCheckoutHousePage->setToRobUserID( r->uniqueID );
                    currentGamePage->base_makeActive( true );
                    }
                }
            }
        else if( currentGamePage == robCheckoutHousePage ) {
            if( robCheckoutHousePage->getReturnToMenu() ) {
                currentGamePage = menuPage;
                currentGamePage->base_makeActive( true );
                }
            else {
                char *houseMap = robCheckoutHousePage->getHouseMap();
                
                if( houseMap != NULL ) {
                    
                    robHousePage->setHouseMap( houseMap );
                    delete [] houseMap;

                    char *backpackContents = 
                        robCheckoutHousePage->getBackpackContents();
                    
                    robHousePage->setBackpackContents( backpackContents );
                    
                    delete [] backpackContents;

                    char *galleryContents = 
                        robCheckoutHousePage->getGalleryContents();
                    
                    robHousePage->setGalleryContents( galleryContents );
                    
                    delete [] galleryContents;


                    char *ownerName = robCheckoutHousePage->getOwnerName();
                    
                    // handle names that end with "s" properly
                    char *ownerNamePossessive;

                    if( ownerName[ strlen( ownerName ) - 1 ] == 's' ) {
                        ownerNamePossessive = 
                            autoSprintf( "%s'", ownerName );
                        }
                    else {
                        ownerNamePossessive = 
                            autoSprintf( "%s's", ownerName );
                        }
                    delete [] ownerName;

                    char *description = 
                        autoSprintf( translate( "robDescription" ),
                                     ownerNamePossessive );

                    delete [] ownerNamePossessive;


                    robHousePage->setDescription( description );
                    delete [] description;

                    currentGamePage = robHousePage;
                    currentGamePage->base_makeActive( true );
                    }
                }
            }
        else if( currentGamePage == robHousePage ) {
            if( robHousePage->isStale() ) {
                currentGamePage = staleHouseDeadPage;
                currentGamePage->base_makeActive( true );
                }
            else if( robHousePage->getDone() ) {
                char *houseMap = robHousePage->getHouseMap();
                char *moveList = robHousePage->getMoveList();
                char *backpackContents = robHousePage->getBackpackContents();
                
                robCheckinHousePage->setBackpackContents( backpackContents );
                robCheckinHousePage->setHouseMap( houseMap );
                robCheckinHousePage->setSuccess( robHousePage->getSuccess() );
                                
                robCheckinHousePage->setMoveList( moveList );

                delete [] backpackContents;
                delete [] moveList;
                delete [] houseMap;
                
                currentGamePage = robCheckinHousePage;
                currentGamePage->base_makeActive( true );
                }
            }
        else if( currentGamePage == robCheckinHousePage ) {
            if( robCheckinHousePage->getReturnToHome() ) {
                currentGamePage = checkoutHousePage;
                currentGamePage->base_makeActive( true );
                }
            else if( robCheckinHousePage->getStartOver() ) {
                // fresh start, back to editing empty house
                currentGamePage = checkoutHousePage;
                currentGamePage->base_makeActive( true );
                }
            }
        else if( currentGamePage == robberyReplayMenuPage ) {
            if( robberyReplayMenuPage->getReturnToMenu() ) {
                currentGamePage = menuPage;
                currentGamePage->base_makeActive( true );
                }
            else if( robberyReplayMenuPage->getStartReplay() ) {
                int id = robberyReplayMenuPage->getLogID();
                
                if( id != -1  ) {
                    currentGamePage = fetchRobberyReplayPage;
                    fetchRobberyReplayPage->setLogID( id );
                    currentGamePage->base_makeActive( true );
                    }
                }
            }
        else if( currentGamePage == fetchRobberyReplayPage ) {
            if( fetchRobberyReplayPage->getReturnToMenu() ) {
                currentGamePage = menuPage;
                currentGamePage->base_makeActive( true );
                }
            else if( fetchRobberyReplayPage->getRecordReady() ) {
                RobberyLog log = fetchRobberyReplayPage->getLogRecord();
                
                replayRobHousePage->setLog( log );
                
                currentGamePage = replayRobHousePage;
                currentGamePage->base_makeActive( true );
                }
            }
        else if( currentGamePage == replayRobHousePage ) {
            if( replayRobHousePage->getDone() ) {
                // nothing to check in (just a read-only replay)

                // back to menu right away
                currentGamePage = menuPage;
                currentGamePage->base_makeActive( true );
                }
            }

        }


    // now draw stuff AFTER all updates
    drawFrameNoUpdate( true );



    // draw tail end of pause screen, if it is still visible
    if( pauseScreenFade > 0 ) {
        drawPauseScreen();
        }
    }



void drawFrameNoUpdate( char inUpdate ) {

    if( currentGamePage != NULL ) {
        currentGamePage->base_draw( lastScreenViewCenter, viewWidth );
        }

    }



void pointerMove( float inX, float inY ) {
    if( isPaused() ) {
        return;
        }
    
    if( currentGamePage != NULL ) {
        currentGamePage->base_pointerMove( inX, inY );
        }
    }



void pointerDown( float inX, float inY ) {
    if( isPaused() ) {
        return;
        }
    
    if( currentGamePage != NULL ) {
        currentGamePage->base_pointerDown( inX, inY );
        }
    }



void pointerDrag( float inX, float inY ) {
    if( isPaused() ) {
        return;
        }
    
    if( currentGamePage != NULL ) {
        currentGamePage->base_pointerDrag( inX, inY );
        }
    }



void pointerUp( float inX, float inY ) {
    if( isPaused() ) {
        return;
        }
    
    if( currentGamePage != NULL ) {
        currentGamePage->base_pointerUp( inX, inY );
        }
    }







void keyDown( unsigned char inASCII ) {

    // taking screen shot is ALWAYS possible
    if( inASCII == '=' ) {    
        saveScreenShot( "screen" );
        }
    

    
    if( isPaused() ) {
        // block general keyboard control during pause


        switch( inASCII ) {
            case 13:  // enter
                // unpause
                pauseGame();
                break;
            }
        
        
        if( inASCII == 127 || inASCII == 8 ) {
            // subtract from it

            deleteCharFromUserTypedMessage();

            holdDeleteKeySteps = 0;
            // start with long delay until first repeat
            stepsBetweenDeleteRepeat = (int)( 30 / frameRateFactor );
            }
        else if( inASCII >= 32 ) {
            // add to it
            if( currentUserTypedMessage != NULL ) {
                
                char *oldMessage = currentUserTypedMessage;

                currentUserTypedMessage = autoSprintf( "%s%c", 
                                                       oldMessage, inASCII );
                delete [] oldMessage;
                }
            else {
                currentUserTypedMessage = autoSprintf( "%c", inASCII );
                }
            }
        
        return;
        }
    
    
    if( currentGamePage != NULL ) {
        currentGamePage->base_keyDown( inASCII );
        }

    
    switch( inASCII ) {
        case 'm':
        case 'M': {
#ifdef USE_MALLINFO
            struct mallinfo meminfo = mallinfo();
            printf( "Mem alloc: %d\n",
                    meminfo.uordblks / 1024 );
#endif
            }
            break;
        }
    }



void keyUp( unsigned char inASCII ) {
    if( inASCII == 127 || inASCII == 8 ) {
        // delete no longer held
        // even if pause screen no longer up, pay attention to this
        holdDeleteKeySteps = -1;
        }

    if( ! isPaused() ) {
        if( currentGamePage != NULL ) {
            currentGamePage->base_keyUp( inASCII );
            }
        }

    }







void specialKeyDown( int inKey ) {
    if( isPaused() ) {
        return;
        }
    

    if( currentGamePage != NULL ) {
        currentGamePage->base_specialKeyDown( inKey );
        }

	}



void specialKeyUp( int inKey ) {
    if( isPaused() ) {
        return;
        }
    

    if( currentGamePage != NULL ) {
        currentGamePage->base_specialKeyUp( inKey );
        }
	} 




char getUsesSound() {
    return false;
    }


void getSoundSamples( Uint8 *inBuffer, int inLengthToFillInBytes ) {
    // do nothing, no sound
    }







void drawString( const char *inString ) {
    
    setDrawColor( 1, 1, 1, 0.75 );

    doublePair messagePos = lastScreenViewCenter;

    messagePos.x -= viewWidth / 2;
        
    messagePos.x +=  0.25;
    

    
    messagePos.y += (viewWidth * viewHeightFraction) /  2;
    
    messagePos.y -= 0.4375;
    messagePos.y -= 0.5;
    

    int numLines;
    
    char **lines = split( inString, "\n", &numLines );
    
    for( int i=0; i<numLines; i++ ) {
        

        mainFont->drawString( lines[i], messagePos, alignLeft );
        messagePos.y -= 0.75;
        
        delete [] lines[i];
        }
    delete [] lines;
    }




