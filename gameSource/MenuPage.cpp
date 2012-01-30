#include "MenuPage.h"

#include "minorGems/game/game.h"


extern Font *mainFont;





MenuPage::MenuPage() 
        : mPickList( 0, 0, this ),
          mEditHouseButton( mainFont, 4, -4, translate( "startEditHouse" ) ),
          mRobHouseButton( mainFont, -4, -4, translate( "startRobHouse" ) ),
          mStartEditHouse( false ),
          mStartRobHouse( false ) {

    addComponent( &mEditHouseButton );
    addComponent( &mRobHouseButton );
    addComponent( &mPickList );

    mEditHouseButton.addActionListener( this );
    mRobHouseButton.addActionListener( this );

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

    mStatusMessageKey = NULL;
    mStatusError = false;
    }


        

