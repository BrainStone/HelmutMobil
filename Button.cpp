#include "Button.h"

using namespace std;

const Button::Color Button::defaultBackgroundUp( 0xDD, 0x00, 0x00 );
const Button::Color Button::defaultBackgroundDown( 0x00, 0xDD, 0x00 );
const Button::Color Button::defaultFont( 0xFF, 0xFF, 0xFF );

list<Button*> Button::Buttons;
bool Button::initialized = false;

void Button::setUp() {
	Buttons.push_back( this );

	if ( !initialized ) {
		initialized = true;

		registerMouseEvent( handleMouseEvent );
		registerMoveEvent( handleMoveEvent );
	}
}

bool Button::mouseOnButton( double x, double y ) const {
	return (x >= this->x) && (x <= (this->x + width)) && (y >= this->y) && (y <= (this->y + height));
}

Button::Button( const string& text, VGfloat x, VGfloat y, int width, int height, int fontsize,
				Color backgroundUp, Color backgroundDown, Color font )
				: x( x ), y( y ), width( width ), height( height ), fontsize( fontsize ),
				backgroundUp( backgroundUp ), backgroundDown( backgroundDown ),
				font( font ), down( false ), active( true ) {
	this->text = new char[text.size() + 1];
	strcpy( this->text, text.c_str() );

	setUp();
}

Button::Button( const string& text, VGfloat x, VGfloat y, int fontsize,
				Color backgroundUp, Color backgroundDown, Color font )
				: x( x ), y( y ), fontsize( fontsize ),
				backgroundUp( backgroundUp ), backgroundDown( backgroundDown ),
				font( font ), down( false ), active( true ) {
	this->text = new char[text.size() + 1];
	strcpy( this->text, text.c_str() );

	width = TextWidth( this->text, SansTypeface, fontsize ) + 2 * fontsize;
	height = fontsize + 2 * fontsize;

	setUp();
}

Button::Button( const char* text, VGfloat x, VGfloat y, int width, int height, int fontsize,
				Color backgroundUp, Color backgroundDown, Color font )
				: x( x ), y( y ), width( width ), height( height ), fontsize( fontsize ),
				backgroundUp( backgroundUp ), backgroundDown( backgroundDown ),
				font( font ), down( false ), active( true ) {
	this->text = new char[strlen( text ) + 1];
	strcpy( this->text, text );

	setUp();
}

Button::Button( const char* text, VGfloat x, VGfloat y, int fontsize,
				Color backgroundUp, Color backgroundDown, Color font )
				: x( x ), y( y ), fontsize( fontsize ),
				backgroundUp( backgroundUp ), backgroundDown( backgroundDown ),
				font( font ), down( false ), active(true) {
	this->text = new char[strlen( text ) + 1];
	strcpy( this->text, text );

	width = TextWidth( this->text, SansTypeface, fontsize ) + 2 * fontsize;
	height = fontsize + 2 * fontsize;

	setUp();
}

Button::~Button() {
	Buttons.remove( this );
	delete[] text;
}

void Button::render() {
	if ( down )
		Fill( backgroundDown.red, backgroundDown.green, backgroundDown.blue, backgroundDown.alpha );
	else
		Fill( backgroundUp.red, backgroundUp.green, backgroundUp.blue, backgroundUp.alpha );

	Rect( x, y, width, height );

	Fill( font.red, font.green, font.blue, font.alpha );

	TextMid( x + width / 2.0, y + (height - fontsize) / 2.0, text, SansTypeface, fontsize );
}

void Button::mouseEvent( bool down ) {
	if ( active ) {
		if ( mouseOnButton( mouseX, mouseY ) ) {
			if ( this->down && !down )
				for ( list<void( *)()>::iterator it = eventCallbacks.begin(); it != eventCallbacks.end(); ++it )
					(*it)();

			this->down = down;
		}
	} else {
		this->down = false;
	}
}

void Button::moveEvent( double x, double y ) {
	down = down && mouseOnButton( x, y );
}

void Button::setActive( bool active ) {
	this->active = active;
}

void Button::addEventCallback( void( *callback )() ) {
	eventCallbacks.push_back( callback );
}

void Button::handleMouseEvent( bool down ) {
	for ( list<Button*>::iterator it = Buttons.begin(); it != Buttons.end(); ++it )
		(*it)->mouseEvent( down );
}

void Button::handleMoveEvent( double x, double y ) {
	for ( list<Button*>::iterator it = Buttons.begin(); it != Buttons.end(); ++it )
		(*it)->moveEvent( x, y );
}

// ButtonCustomRender

ButtonCustomRender::ButtonCustomRender( void( *customRender )(bool, VGfloat, VGfloat, int, int), VGfloat x, VGfloat y, int width, int height )
	: Button( "", x, y, width, height ), customRender( customRender ) {}

void ButtonCustomRender::render() {
	customRender( down, x, y, width, height );
}