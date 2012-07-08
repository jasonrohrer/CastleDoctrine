#include "AuctionPage.h"

#include "minorGems/game/game.h"
#include "minorGems/util/stringUtils.h"
#include "minorGems/game/Font.h"

#include "message.h"
#include "ticketHash.h"


#include <time.h>


extern Font *mainFont;

extern char *serverURL;

extern int userID;




AuctionPage::AuctionPage() 
        : mDoneButton( mainFont, 8, -5, translate( "doneEdit" ) ),
          mUpdateButton( mainFont, 0, -5, translate( "auctionUpdateButton" ) ),
          mWebRequest( -1 ),
          mSecondsUntilUpdate( -1 ),
          mBaseTimestamp( -1 ),
          mDone( false ) {

    addComponent( &mDoneButton );
    addComponent( &mUpdateButton );
    
    mDoneButton.addActionListener( this );
    mUpdateButton.addActionListener( this );
    
    mDoneButton.setMouseOverTip( "" );
    mUpdateButton.setMouseOverTip( translate( "auctionUpdateButtonTip" ) );
    
    mUpdateButton.setVisible( false );
    }


        
AuctionPage::~AuctionPage() {
    if( mWebRequest != -1 ) {
        clearWebRequest( mWebRequest );
        }

    for( int i=0; i<mAuctionItems.size(); i++ ) {
        delete [] *( mAuctionItems.getElement( i ) );
        }
    mAuctionItems.deleteAll();
    }



char AuctionPage::getDone() {
    return mDone;
    }



void AuctionPage::actionPerformed( GUIComponent *inTarget ) {
    if( inTarget == &mDoneButton ) {
        mDone = true;
        }    
    else if( inTarget == &mUpdateButton ) {
        mUpdateButton.setVisible( false );
        setToolTipDirect( "" );
        refreshPrices();
        
        // keep house checked out as long as user is still actively
        // watching the auctions with the Update button
        actionHappened();
        }
    }



void AuctionPage::step() {
    if( mWebRequest != -1 ) {
            
        int stepResult = stepWebRequest( mWebRequest );
    
        switch( stepResult ) {
            case 0:
                break;
            case -1:
                setStatus( "err_webRequest", true );
                clearWebRequest( mWebRequest );
                mWebRequest = -1;
                break;
            case 1: {
                char *result = getWebResult( mWebRequest );
                clearWebRequest( mWebRequest );
                mWebRequest = -1;
                     
                printf( "Web result = %s\n", result );
   
                if( strstr( result, "DENIED" ) != NULL ) {
                    setStatus( "auctionListFetchFailed", true );
                    }
                else {
                    // got auction list!

                    setStatus( NULL, false );

                    // parse result
                    SimpleVector<char *> *lines = 
                        tokenizeString( result );
                    
                    
                    char badParse = false;

                    if( lines->size() < 2 ) {
                        // list could be empty, but it still must have 
                        // seconds_until_price_drop and 
                        // OK at the end
                        badParse = true;
                        }
                    else {
                        char *string = *( lines->getElement( 0 ) );
                        
                        // first line should be seconds until update
                        sscanf( string, "%d", &mSecondsUntilUpdate );
                        delete [] string;

                        if( mSecondsUntilUpdate > 0 ) {
                            mBaseTimestamp = time( NULL );
                            }
                        }
                        
                    // last line should be OK
                    for( int i=1; i<lines->size() - 1; i++ ) {
                        char *line = *( lines->getElement( i ) );
                        
                        mAuctionItems.push_back( line );
                        
                        /*
                        int numParts;
                        char **parts = split( line, "#", &numParts );
                        
                        if( numParts != 5 ) {
                            printf( "Unexpected number of parts on house "
                                    "list line: %d\n", numParts );
                            badParse = true;
                            }
                        else {
                            HouseRecord r;
                            
                            r.selected = false;
                            r.draw = false;
                            r.position.x = 0;
                            r.position.y = topOffset - i * lineHeight;
                            
                            sscanf( parts[0], "%d", &( r.uniqueID ) );
                            
                            
                            char found;
                            r.characterName = 
                                trimName(
                                    replaceAll( parts[1], "_", " ", &found ),
                                    lineWidthLeft - 0.125 );
                            
                            r.lastRobberName = 
                                trimName (
                                    replaceAll( parts[2], "_", " ", &found ),
                                    lineWidthLeft - 0.125 );
                            

                            sscanf( parts[3], "%d", &( r.lootValue ) );
                            sscanf( parts[4], "%d", &( r.robAttempts ) );

                            mHouseList.push_back( r );
                            }
                        
                        for( int j=0; j<numParts; j++ ) {
                            delete [] parts[j];
                            }
                        delete [] parts;

                        
                        delete [] line;
                        */
                        }
                    
                    if( lines->size() > 0 ) {
                        
                        char *line = 
                            *( lines->getElement( lines->size() -1  ) );
                    
                        if( strcmp( line, "OK" ) != 0 ) {
                            badParse = true;
                            }
                        
                        delete [] line;
                        }
                    
                    

                    delete lines;

                    
                    if( badParse ) {
                        for( int i=0; i<mAuctionItems.size(); i++ ) {
                            delete [] *( mAuctionItems.getElement( i ) );
                            }
                        mAuctionItems.deleteAll();
                        setStatus( "auctionListFetchFailed", true );
                        }
                    }
                        
                        
                delete [] result;
                }
                break;
            }
        return;
        }

    
    // else no web request

    if( ( time(NULL) - mBaseTimestamp ) >= mSecondsUntilUpdate ) {
        // expired
        mUpdateButton.setVisible( true );
        }
    }


        
void AuctionPage::draw( doublePair inViewCenter, 
                          double inViewSize ) {
    
    doublePair labelPos = { 0, 6.75 };
    
    drawMessage( "auctionDescription", labelPos, false );

    labelPos.y = 0;

    for( int i=0; i<mAuctionItems.size(); i++ ) {
        char *string = *( mAuctionItems.getElement( i ) );
        
        drawMessage( string, labelPos, false );
        labelPos.y -= 0.5;
        }
    
    if( mSecondsUntilUpdate > 0 ) {
        
        int currentTime = time( NULL );
        
        int timeLeft = mSecondsUntilUpdate - ( currentTime - mBaseTimestamp );
        
        
        // draw time where Update button will eventually go
        labelPos = mUpdateButton.getCenter();

        if( timeLeft > 0 ) {
            
            int minutesLeft = timeLeft / 60;
            int secondsLeft = timeLeft - minutesLeft * 60;
            
            

            char *timeMessage = autoSprintf( "%d:%02d",
                                             minutesLeft, secondsLeft );
            
            mainFont->drawString( timeMessage, labelPos, alignLeft );
        
            delete [] timeMessage;
            }
        

        labelPos.x -= 2;

        if( timeLeft > 0 ) {
            mainFont->drawString( translate( "auctionTimeString" ),
                                  labelPos, alignRight );
            }
        else {
            mainFont->drawString( translate( "auctionPricesStale" ), 
                                  labelPos, alignRight );
            }
        }
    }


        
void AuctionPage::makeActive( char inFresh ) {
    LiveHousePage::makeActive( inFresh );
    
    if( !inFresh ) {
        return;
        }

    mDone = false;

    if( mWebRequest == -1 ) {
        if( mSecondsUntilUpdate <= 0 ||
            ( time(NULL) - mBaseTimestamp ) >= mSecondsUntilUpdate ) {
            
            refreshPrices();
            }
        // else keep existing prices (not stale)
        }
    // else go with prices that will be fetched by current web request
    }



void AuctionPage::refreshPrices() {
    

    if( mWebRequest != -1 ) {
        clearWebRequest( mWebRequest );
        }

    for( int i=0; i<mAuctionItems.size(); i++ ) {
        delete [] *( mAuctionItems.getElement( i ) );
        }
    mAuctionItems.deleteAll();
    
    mSecondsUntilUpdate = -1;
    mBaseTimestamp = -1;

    // request house list from server
    char *ticketHash = getTicketHash();
    
    char *actionString = autoSprintf( "action=list_auctions&user_id=%d&%s", 
                                      userID, ticketHash );
    delete [] ticketHash;
            
    
    mWebRequest = startWebRequest( "POST", 
                                   serverURL, 
                                   actionString );
    printf( "Web POST action string = %s\n", actionString );


    delete [] actionString;
    }


        

