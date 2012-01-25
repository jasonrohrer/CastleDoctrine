#include "MenuPage.h"

#include "minorGems/game/game.h"


extern Font *mainFont;





MenuPage::MenuPage() 
        : mPickList( 0, 0, this ),
          mEditHouseButton( mainFont, 4, -4, translate( "startEditHouse" ) ),
          mUpButton( "up.tga", -4, 4, 1/16.0),
          mStartEditHouse( false ) {

    addComponent( &mEditHouseButton );
    addComponent( &mUpButton );
    addComponent( &mPickList );
    mEditHouseButton.addActionListener( this );
    }


        
MenuPage::~MenuPage() {
    }



char MenuPage::getStartEditHouse() {
    return mStartEditHouse;
    }



void MenuPage::actionPerformed( GUIComponent *inTarget ) {
    if( inTarget == &mEditHouseButton ) {
        mStartEditHouse = true;
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
    

    mStatusMessageKey = NULL;
    mStatusError = false;
    }


        

