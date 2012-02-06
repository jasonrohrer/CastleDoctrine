#include "MenuPage.h"

#include "minorGems/game/game.h"


extern Font *mainFont;





MenuPage::MenuPage() 
        : mPickList( 0, 0, false, this ),
          mEditHouseButton( mainFont, 4, -4, translate( "startEditHouse" ) ),
          mRobHouseButton( mainFont, -4, -4, translate( "startRobHouse" ) ),
          mShowReplayListButton( mainFont, -4, -6, 
                                 translate( "listRobberyReplays" ) ),
          mStartEditHouse( false ),
          mStartRobHouse( false ),
          mShowReplayList( false ) {

    addComponent( &mEditHouseButton );
    addComponent( &mRobHouseButton );
    addComponent( &mShowReplayListButton );
    addComponent( &mPickList );

    mEditHouseButton.addActionListener( this );
    mRobHouseButton.addActionListener( this );
    mShowReplayListButton.addActionListener( this );

    mPickList.addActionListener( this );

    mRobHouseButton.setVisible( false );
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
            }
        else {
            mRobHouseButton.setVisible( true );
            }
        }
    else if( inTarget == &mRobHouseButton ) {
        mStartRobHouse = true;
        }
    else if( inTarget == &mShowReplayListButton ) {
        mShowReplayList = true;
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
    
    mStatusMessageKey = NULL;
    mStatusError = false;
    }


        

