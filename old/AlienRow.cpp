#include "AlienRow.h"

AlienRow::AlienRow( const vector<Alien*> &initial_list ) : _aliens( initial_list )
{
	//

}

// alternative constructor - pass in a reference of the alien to clone
AlienRow::AlienRow( const Alien &clone, const Vector3 &initial_pos, unsigned num_aliens, unsigned spacing ) 
	: _num_aliens( num_aliens )
{
	//
	for ( int i = 0; i < num_aliens; ++i ) {
		//
		Alien * next = (Alien*) clone.Clone( true, false );

		// move to the initial position
		next->Translate( initial_pos - next->getPosition() );
		
		// shift by spacing amount
		next->Translate( Vector3( i * spacing, 0, 0 ) );

		_aliens.push_back( next );
	}
}

AlienRow::~AlienRow()
{
	//
	for ( int i = 0; i < _aliens.size(); ++i ) {
		//
		delete _aliens[i];
	}
}

void AlienRow::translate( int delta ) {
	//
	for ( unsigned i = 0; i < _num_aliens; ++i ) {
		//
		_aliens[i]->Translate( Vector3( delta, 0, 0 ) );

		// change the animation frame
		_aliens[i]->changeAnimationFrame();
	}
}

unsigned AlienRow::getNumAliens() const {
	//
	return _num_aliens;
}

void AlienRow::getAliens( vector<Alien*> &list ) const {
	//
	list = _aliens;
}

Alien * AlienRow::getLeftMostAlien() {
	//
	if ( _num_aliens == 0 ) return 0;

	//
	Alien * result = _aliens[0];
	for ( unsigned i = 1; i < _num_aliens; ++i ) {
		//
		if ( _aliens[i]->getPosition().x < result->getPosition().x ) {
			//
			result = _aliens[i];
		}
	}

	return result;
}

Alien * AlienRow::getRightMostAlien() {
	//
	if ( _num_aliens == 0 ) return 0;

	//
	Alien * result = _aliens[0];
	for ( unsigned i = 1; i < _num_aliens; ++i ) {
		//
		if ( _aliens[i]->getPosition().x > result->getPosition().x ) {
			//
			result = _aliens[i];
		}
	}

	return result;
}

bool AlienRow::containsAlien( ScreenEntity * to_remove ) const {
	//
	for ( unsigned i = 0; i < _num_aliens; ++i ) {
		//
		if ( _aliens[i] == to_remove ) {
			//
			return true;
		}
	}

	return false;
}

void AlienRow::killAlien( ScreenEntity * to_kill ) {
	//
	for ( unsigned i = 0; i < _num_aliens; ++i ) {
		//
		if ( _aliens[i] == to_kill ) {
			//
			_aliens[i]->kill();
			
			return;
		}
	}
}

vector<Alien*> AlienRow::getDeadAliens() {
	//
	vector<Alien*> result;
	for ( unsigned i = 0; i < _num_aliens; ++i ) {
		//
		if ( _aliens[i]->getState() == SpaceInvadersEntity::EntityState::dead ) {
			//
			result.push_back( _aliens[i] );
		}
	}

	return result;
}

void AlienRow::removeAlien( ScreenEntity * to_remove ) {
	//
	for ( unsigned i = 0; i < _num_aliens; ++i ) {
		//
		if ( _aliens[i] == to_remove ) {
			//
			_aliens.erase( _aliens.begin() + i );
			--_num_aliens;

			return;
		}
	}
}