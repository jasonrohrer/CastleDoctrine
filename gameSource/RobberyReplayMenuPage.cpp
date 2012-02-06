#include "RobberyReplayMenuPage.h"

#include "minorGems/game/game.h"


extern Font *mainFont;





RobberyReplayMenuPage::RobberyReplayMenuPage() 
        : mPickList( 0, 0, true, this ),
          mMenuButton( mainFont, 4, -4, translate( "returnMenu" ) ),
          mReplayButton( mainFont, -4, -4, translate( "startReplay" ) ),
          mReturnToMenu( false ),
          mStartReplay( false ) {

    addComponent( &mMenuButton );
    addComponent( &mReplayButton );
    addComponent( &mPickList );

    mMenuButton.addActionListener( this );
    mReplayButton.addActionListener( this );

    mPickList.addActionListener( this );

    mReplayButton.setVisible( false );
    }


        
RobberyReplayMenuPage::~RobberyReplayMenuPage() {
    }



char RobberyReplayMenuPage::getReturnToMenu() {
    return mReturnToMenu;
    }



char RobberyReplayMenuPage::getStartReplay() {
    return mStartReplay;
    }



int RobberyReplayMenuPage::getLogID() {
    HouseRecord *record = mPickList.getSelectedHouse();

    if( record == NULL ) {
        return -1;
        }
    else {
        return record->uniqueID;
        }
    }



void RobberyReplayMenuPage::actionPerformed( GUIComponent *inTarget ) {
    if( inTarget == &mMenuButton ) {
        mReturnToMenu = true;
        }
    else if( inTarget == &mPickList ) {
        if( mPickList.getSelectedHouse() == NULL ) {
            mReplayButton.setVisible( false );
            }
        else {
            mReplayButton.setVisible( true );
            }
        }
    else if( inTarget == &mReplayButton ) {
        mStartReplay = true;
        }
    
    }



void RobberyReplayMenuPage::step() {
    }


        
void RobberyReplayMenuPage::draw( doublePair inViewCenter, 
                          double inViewSize ) {
    }


        
void RobberyReplayMenuPage::makeActive( char inFresh ) {
    if( !inFresh ) {
        return;
        }
    
    mPickList.refreshList();
    
    mReturnToMenu = false;
    mStartReplay = false;

    mStatusMessageKey = NULL;
    mStatusError = false;
    }


        

