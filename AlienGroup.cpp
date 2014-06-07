#include "AlienGroup.h"

AlienGroup::AlienGroup( const Vector3 &start_pos, const unsigned num_rows, const unsigned row_spacing, const unsigned num_row_aliens, const unsigned sprite_spacing, const int alien_speed )
	: _num_rows( num_rows ), _speed( alien_speed )
{
	//
	Vector3 next_pos( start_pos );
	for ( unsigned r = 0; r < num_rows; ++r ) {
		//
		_aliens.push_back( new AlienRow( *( new AlienOne() ), next_pos, num_row_aliens, sprite_spacing ) );

		next_pos += Vector3( 0, row_spacing, 0 );
	}

	//
	_current_dir = right;
}


AlienGroup::~AlienGroup(  )
{
	//
}

unsigned AlienGroup::getNumberOfRows() const {
	//
	return _num_rows;
}

void AlienGroup::getAliens( vector<AlienRow*> &alien_list ) const {
	//
	alien_list = _aliens;
}

void AlienGroup::translate() {
	//
	switch( _current_dir ) {
	case left:
		// translate every alien by delta
		for ( int i = 0; i < _num_rows; ++i ) {
			//
			_aliens[i]->translate( -_speed );
		}
		break;
	case right:
		// translate every alien by delta
		for ( int i = 0; i < _num_rows; ++i ) {
			//
			_aliens[i]->translate( _speed );
		}
		break;
	}
}

AlienGroup::Direction AlienGroup::getCurrentDirection() const {
	//
	return _current_dir;
}

void AlienGroup::reverseDirection() {
	//
	switch ( _current_dir ) {
	case left:
		_current_dir = right;
		break;
	case right:
		_current_dir = left;
		break;
	}
}

Alien * AlienGroup::getLeftMostAlien() {
	//
	return 0;
}

Alien * AlienGroup::getRightMostAlien() {
	//
	return 0;
}