#include "MenuPage.h"

#include "minorGems/game/game.h"


extern Font *mainFont;

extern int playerIsAdmin;




MenuPage::MenuPage() 
        : mPickList( 0, 0, false, this ),
          mEditHouseButton( mainFont, 4, -6, translate( "returnHome" ) ),
          mRobHouseButton( mainFont, -4, -4, translate( "startRobHouse" ) ),
          mShowReplayListButton( mainFont, -4, -6, 
                                 translate( "listRobberyReplays" ) ),
          mProofReplayButton( mainFont, 4, -4, 
                              translate( "startProofReplay" ) ),
          mStartEditHouse( false ),
          mStartRobHouse( false ),
          mShowReplayList( false ),
          mStartSelfTestReplay( false ) {

    addComponent( &mEditHouseButton );
    addComponent( &mRobHouseButton );
    addComponent( &mShowReplayListButton );
    addComponent( &mProofReplayButton );
    addComponent( &mPickList );

    mEditHouseButton.addActionListener( this );
    mRobHouseButton.addActionListener( this );
    mShowReplayListButton.addActionListener( this );
    mProofReplayButton.addActionListener( this );

    mPickList.addActionListener( this );

    mRobHouseButton.setVisible( false );
    mProofReplayButton.setVisible( false );
    }


        
MenuPage::~MenuPage() {
    }



char MenuPage::getStartEditHouse() {
    return mStartEditHouse;
    }



char MenuPage::getStartRobHouse() {
    return mStartRobHouse;
    }


char MenuPage::getShowReplayList() {
    return mShowReplayList;
    }


char MenuPage::getStartSelfTestReplay() {
    return mStartSelfTestReplay;
    }


HouseRecord *MenuPage::getSelectedHouse() {
    return mPickList.getSelectedHouse();
    }



void MenuPage::actionPerformed( GUIComponent *inTarget ) {
    if( inTarget == &mEditHouseButton ) {
        mStartEditHouse = true;
        }
    else if( inTarget == &mPickList ) {
        if( mPickList.getSelectedHouse() == NULL ) {
            mRobHouseButton.setVisible( false );
            mProofReplayButton.setVisible( false );
            }
        else {
            mRobHouseButton.setVisible( true );
            mProofReplayButton.setVisible( playerIsAdmin );
            }
        }
    else if( inTarget == &mRobHouseButton ) {
        mStartRobHouse = true;
        }
    else if( inTarget == &mShowReplayListButton ) {
        mShowReplayList = true;
        }
    else if( inTarget == &mProofReplayButton ) {
        mStartSelfTestReplay = true;
        }
    }



void MenuPage::step() {
    }


        
void MenuPage::draw( doublePair inViewCenter, 
                          double inViewSize ) {
    }


        
void MenuPage::makeActive( char inFresh ) {
    if( !inFresh ) {
        return;
        }
    
    mPickList.refreshList();
    
    mStartEditHouse = false;
    mStartRobHouse = false;
    mShowReplayList = false;
    mStartSelfTestReplay = false;

    mStatusMessageKey = NULL;
    mStatusError = false;
    }


        

