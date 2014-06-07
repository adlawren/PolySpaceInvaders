/*
	The main template file/runtime file.

	TODO(high level stuffs):
	- need to fix the occasional problem where rows pretend like the fallen guy on the far side is still alive; in their translation :S
	- add missile functionality - and death mechanics
		-> need to get the aliens shooting back; and the player killed if hit
	- clean up fire-by-event process; it's a little finiky(?) if you press two buttons at once...
	- need to keep the fighter from going off the screen
	- add explosion animations for the aliens
	- add different types of aliens
	- add lives for the fighter
	- add the red ufo going accross the top
	- sound for when the space invaders move
		-> there also needs to be sound for the red ufo
*/

#include "PolycodeTemplateApp.h"

PolycodeTemplateApp::PolycodeTemplateApp(PolycodeView *view) : EventHandler() {
	// initialization
	core = new Win32Core(view, screen_width,screen_height,false, false, 0, 0, 60);
	CoreServices::getInstance()->getResourceManager()->addArchive("default.pak");
	CoreServices::getInstance()->getResourceManager()->addDirResource("default", false);
	
	// assign GUI parameters
	alien_width_1 = 168;
	alien_height_1 = 53;

	alien_width_2 = 154;
	alien_height_2 = 52;

	alien_width_2 = 132;
	alien_height_2 = 53;

	alien_sprite_xscale = 0.5;
	alien_sprite_yscale = 0.5;
	alien_sprite_scale = new Vector3( alien_sprite_xscale, alien_sprite_yscale, 0 );

	player_width = 85;
	player_height = 53;
	player_sprite_xscale = 0.5;
	player_sprite_yscale = 0.5;
	player_sprite_scale = new Vector3( player_sprite_xscale, player_sprite_yscale, 0 );

	pmissile_width = 3;
	pmissile_height = 15;
	player_missile_scale = new Vector3( pmissile_sprite_xscale, pmissile_sprite_yscale, 0 );

	player_xoffset = 50;
	player_yoffset = 50;

	// initialize timers/timer parameters
	timer = new Timer( false, 0 );
	weapon_cooldown = new Timer( false, 0 );

	duration = 500;

	// initialize player delta
	player_delta = Vector3( 0, 0, 0 );

	// Create screen
	main_screen = new PhysicsScreen( 10, 50 );

	// add background
	ScreenSprite *background = ScreenSprite::ScreenSpriteFromImageFile("Resources/background.png", Number(screen_width*2), Number(screen_height*2) ); //new ScreenSprite("Resources/background.png");

	main_screen->addChild( background );

	// initialize fighter/player entity
	ScreenSprite *player_sprite = ScreenSprite::ScreenSpriteFromImageFile("Resources/fighter_1.png", Number(player_width*2), Number(player_height*2) ); //new ScreenSprite("Resources/fighter_1.png");
	player_sprite->setScale( *player_sprite_scale );
	
	Vector3 *location = new Vector3( float( screen_width/2 ) - (player_sprite->getWidth() * player_sprite_xscale / 2), float( screen_height - ( (player_sprite->getHeight()/2) * player_sprite_yscale ) ) - player_yoffset, 0 );

	player = new SpaceInvadersEntity( player_sprite, location, initial_HP );
	main_screen->addCollisionChild( player->getSprite(), PhysicsScreenEntity::ENTITY_RECT );

	// assign current direction
	current_dir = direction::right;

	// spawn aliens and add to screen
	aliens = createAliens( Vector3( 100, 100, 0 ), 1, 50, 10, 50 );
	addAliensToScreen( aliens );

	// listen for input
	core->getInput()->addEventListener( this, InputEvent::EVENT_KEYDOWN );
	core->getInput()->addEventListener( this, InputEvent::EVENT_KEYUP );
	core->getInput()->addEventListener( this, InputEvent::EVENT_MOUSEDOWN );
	core->getInput()->addEventListener( this, InputEvent::EVENT_MOUSEUP );

	// listen for collisions
	main_screen->addEventListener( this, PhysicsScreenEvent::EVENT_NEW_SHAPE_COLLISION );
}
PolycodeTemplateApp::~PolycodeTemplateApp() {
    
}

/*
	TODO:
	- translation functionality for the alien row
		- need to incorporate a current direction
		- awareness of the left-most/right-most entity for consistency in movement
		- if the left-most/right-most entity is at it's respective edge; reverse direction
*/
bool PolycodeTemplateApp::Update() {
	// translate the aliens - if the necessary time has elapsed
	if ( (timer->getElapsedf() * 1000) >= duration ) {
		translateAliens( aliens );
		timer->Reset();
	}

	// process translation input
	processPlayerInput();

	// update player missiles
	updatePlayerMissles( player_missiles, player_missile_speed );

	return core->updateAndRender();
}

/*
	TODO:
	- add missile firing functionality for fighter
*/
void PolycodeTemplateApp::handleEvent( Event *e ) {
	//
	
	if ( e->getDispatcher() == core->getInput() ) {
		//
		InputEvent *ie = (InputEvent*) e;
		switch ( e->getEventCode() )
		{
		case InputEvent::EVENT_KEYDOWN:
			switch( ie->keyCode() ) {
			/*case KEY_a:
				if ( player->getPosition().x >= player_xoffset ) {
					player_delta.x = -player_delta_x;
				} else {
					player_delta.x = 0;
				}
				break;
			case KEY_d:
				if ( player->getPosition().x <= (screen_width/2 - player_xoffset) ) {
					player_delta.x = player_delta_x;
				} else {
					player_delta.x = 0;
				}
				break;*/
			case KEY_SPACE:
				if ( (weapon_cooldown->getElapsedf() * 1000) >= weapon_cooldown_time ) {
					playerFireMissile();
					weapon_cooldown->Reset();
				}
				break;
			}
			break;
		case InputEvent::EVENT_KEYUP:
			switch ( ie->keyCode() ) {
			case KEY_a:
				player_delta.x = 0;
				break;
			case KEY_d:
				player_delta.x = 0;
				break;
			}
			break;
		case InputEvent::EVENT_MOUSEDOWN:
			main_screen->removeChild( player->getSprite() );
			break;
		case InputEvent::EVENT_MOUSEUP:
			main_screen->addChild( player->getSprite() );
			break;
		}
	} else if ( e->getDispatcher() == main_screen ) {
		//
		PhysicsScreenEvent * pe = (PhysicsScreenEvent*) e;
		switch ( pe->getEventCode() ) {
		case PhysicsScreenEvent::EVENT_NEW_SHAPE_COLLISION:
			if ( isPlayerMissile( pe->entity1 ) ) {
				removePlayerMissile( pe->entity1 );
			} else if ( isPlayerMissile( pe->entity2 ) ) {
				removePlayerMissile( pe->entity2 );
			}

			if ( isAlien( pe->entity1 ) ) {
				removeAlien( pe->entity1 );
			} else if ( isAlien( pe->entity2 ) ) {
				removeAlien( pe->entity2 );
			}
			break; 
		}
	}
}

void PolycodeTemplateApp::processPlayerInput() {
	//
	bool key_a = core->getInput()->getKeyState( KEY_a );
	bool key_d = core->getInput()->getKeyState( KEY_d );

	if ( key_a ) {
		//
		if ( player->getPosition().x >= (player_xoffset) ) player->translate( Vector3( -player_delta_x, 0, 0 ) );
	}
	if ( key_d ) {
		if ( player->getPosition().x <= (screen_width - player_xoffset) ) player->translate( Vector3( player_delta_x, 0, 0 ) );
	}
}

/*
	Memory leak here? not deleting the pointers?... of the sprites?...
*/
SpaceInvadersEntity * PolycodeTemplateApp::createAlienOne() {
	// only take half the width; the other half is the second anim. frame
	ScreenSprite * alien_sprite = ScreenSprite::ScreenSpriteFromImageFile( "Resources/Alien_1.png", Number(alien_width_1/2), Number(alien_height_1) );
	alien_sprite->setScale( *alien_sprite_scale );

	// add animations
	alien_sprite->addAnimation( "frame_1", "0", 1 );
	alien_sprite->addAnimation( "frame_2", "1", 1 );
	alien_sprite->playAnimation( "frame_1", 0, false );

	return new SpaceInvadersEntity( alien_sprite, new Vector3(0, 0, 0), initial_HP );
}

SpaceInvadersEntity * PolycodeTemplateApp::createPlayerMissile() {
	//
	ScreenSprite * player_missile_sprite = ScreenSprite::ScreenSpriteFromImageFile( "Resources/alien_1.png", pmissile_width*2, pmissile_height*2 );
	player_missile_sprite->setScale( *player_missile_scale );

	return new SpaceInvadersEntity( player_missile_sprite, new Vector3(0, 0, 0), initial_HP );
}

AlienRow * PolycodeTemplateApp::createAlienRow( Vector3 &start_pos, unsigned num_aliens, unsigned spacing ) {
	//
	return new AlienRow( *( createAlienOne() ), start_pos, num_aliens, spacing );
}

vector<AlienRow*> PolycodeTemplateApp::createAliens( Vector3 &start_pos, unsigned num_rows, unsigned row_spacing, unsigned num_aliens_per_row, unsigned sprite_spacing ) {
	//
	vector<AlienRow*> result;
	Vector3 row_offset( start_pos );
	for ( unsigned i = 0; i < num_rows; ++i ) {
		//
		result.push_back( createAlienRow( row_offset, num_aliens_per_row, sprite_spacing ) );

		row_offset += Vector3( 0, row_spacing, 0 );
	}

	return result;
}

void PolycodeTemplateApp::addAlienRowToScreen( AlienRow * row ) {
	//
	const unsigned num_aliens = row->getNumAliens();
	vector<SpaceInvadersEntity*> aliens;
	row->getAliens( aliens );
	for ( unsigned i = 0; i < num_aliens; ++i ) {
		//
		main_screen->addCollisionChild( aliens[i]->getSprite(), PhysicsScreenEntity::ENTITY_RECT );
		//main_screen->addChild( aliens[i]->getSprite() );
	}
}

void PolycodeTemplateApp::addAliensToScreen( vector<AlienRow*> aliens ) {
	//
	const unsigned num_rows = aliens.size();
	for ( unsigned i = 0; i < num_rows; ++i ) {
		//
		addAlienRowToScreen( aliens[i] );
	}
}

void PolycodeTemplateApp::translateAlienRow( AlienRow *row ) {
	// if there are now aliens in the row; return
	if ( row->getNumAliens() == 0 ) return;
	
	// variable to reverse the delta direction if need be
	int reverse = 1;

	// list for retrieval
	vector<SpaceInvadersEntity*> alien_list;
	row->getAliens( alien_list );

	// a pointer to the entity at the front of the list
	// with respect to the direction the row is moving
	SpaceInvadersEntity *front_entity;

	// alter parameters based on direction
	switch( current_dir ) {
	default:
		break;
	case PolycodeTemplateApp::direction::left:
		reverse *= -1;
		front_entity = alien_list.at(0);
		break;
	case PolycodeTemplateApp::direction::right:
		front_entity = alien_list.at( alien_list.size() - 1 );
		break;
	}

	// delta vector
	Vector3 *delta_vec = new Vector3( delta * reverse, 0, 0 );

	// check if the front element will escape the bound(s) if incremented
	// by the offset, if so, reverse the direction
	Vector3 next_position = front_entity->getPosition() + *delta_vec;
	if ( ( next_position.x > (screen_width - offset) ) || ( next_position.x < offset ) ) {
		reverse *= -1;

		switch( current_dir ) {
		default:
			break;
		case PolycodeTemplateApp::direction::right:
			current_dir = left;
			front_entity = alien_list.at(0);
			break;
		case PolycodeTemplateApp::direction::left:
			current_dir = right;
			front_entity = alien_list.at( alien_list.size() - 1 );
			break;
		}
	}

	// translate
	row->translate( Vector3( delta * reverse, 0, 0 ) );
}

/*
	Translate all the alien rows. Continue until the left-most/right-most element is at the edge of the screen.
	
	TODO:
	- there currently isn't functionality for all rows(of different lengths) moving in sync
*/
void PolycodeTemplateApp::translateAliens( vector<AlienRow*> &aliens ) {
	//
	const unsigned num_rows = aliens.size();
	for ( unsigned i = 0; i < num_rows; ++i ) {
		//
		translateAlienRow( aliens[i] );
	}
}

/*
	fire a missile from above the players location; create a missile at the players location and add it to the player-missile array

	Issue: adding the sprite as a collision child shifts it's position slightly; in a manner not see when adding as a normal child
*/
void PolycodeTemplateApp::playerFireMissile() {
	//
	SpaceInvadersEntity *new_missile = createPlayerMissile();
	new_missile->translate( player->getPosition() + Vector3( -(new_missile->getSprite()->getWidth() * pmissile_sprite_xscale / 2), -( ((player->getSprite()->getHeight()/2) * player_sprite_yscale) + (new_missile->getSprite()->getHeight() * pmissile_sprite_yscale) ), 0 ) );

	main_screen->addCollisionChild( new_missile->getSprite(), PhysicsScreenEntity::ENTITY_RECT );
	
	// see issue above
	//main_screen->addChild( new_missile->getSprite() );

	player_missiles.push_back( new_missile );
}

/*
	translate each missile; as long as it is still within the screen - otherwise... DESTROY IT!!!
*/
void PolycodeTemplateApp::updatePlayerMissles( vector<SpaceInvadersEntity*> player_missles, int player_missile_speed ) {
	//
	unsigned num_missiles = player_missles.size();
	for ( unsigned i = 0; i < num_missiles; ++i ) {
		//
		player_missles[i]->translate( Vector3( 0, -player_missile_speed, 0 ) );

		if ( player_missiles[i]->getSprite()->getPosition().y < 0 ) {
			// remove from the screen
			main_screen->removeChild( player_missiles[i]->getSprite() );

			player_missiles.erase( player_missiles.begin() + i );
			--num_missiles;
		}
	}
}

bool PolycodeTemplateApp::isPlayerMissile( ScreenEntity * entity  ) {
	//
	const unsigned num_missiles = player_missiles.size();
	for ( unsigned i = 0; i < num_missiles; ++i ) {
		//
		if ( player_missiles[i]->getSprite() == entity  ) {
			return true;
		}
	}

	return false;
}

/*
	if they're collision entities, do you need to use remove physics child when removing from the screen?

	Note: INEFFICIENT
	- currently search through the list for the appropriate missile; then delete
*/
void PolycodeTemplateApp::removePlayerMissile( ScreenEntity * to_remove ) {
	//
	main_screen->removeChild( to_remove );

	const unsigned num_missiles = player_missiles.size();
	for ( unsigned i = 0; i < num_missiles; ++i ) {
		//
		if ( to_remove == player_missiles[i]->getSprite() ) {
			player_missiles.erase( player_missiles.begin() + i );
			break;
		}
	}
}

bool PolycodeTemplateApp::isAlien( ScreenEntity * entity ) {
	//
	const unsigned num_rows = aliens.size();
	for ( unsigned i = 0; i < num_rows; ++i ) {
		//
		if ( aliens[i]->containsAlien( entity ) ) return true;
	}

	return false;
}

void PolycodeTemplateApp::removeAlien( ScreenEntity * to_remove ) {
	//
	assert( isAlien( to_remove ) );

	main_screen->removeChild( to_remove );

	const unsigned num_rows = aliens.size();
	for ( unsigned i = 0; i < num_rows; ++i ) {
		//
		if ( aliens[i]->containsAlien( to_remove ) ) {
			aliens[i]->removeAlien( to_remove );
			break;
		}
	}
}