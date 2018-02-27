#include "VGHelper.h"

using namespace std;

template< void( *TextFunc )(VGfloat, VGfloat, char*, Fontinfo, int) >
void TextString( VGfloat x, VGfloat y, const char* text, Fontinfo font, int fontSize ) {
	char* newText = new char[strlen( text ) + 1];
	strcpy( newText, text );

	TextFunc( x, y, newText, font, fontSize );

	delete[] newText;
}

template<void( *TextFunc )(VGfloat, VGfloat, char*, Fontinfo, int)>
void TextString( VGfloat x, VGfloat y, const string& text, Fontinfo font, int fontSize ) {
	char* newText = new char[text.size() + 1];
	strcpy( newText, text.c_str() );

	TextFunc( x, y, newText, font, fontSize );

	delete[] newText;
}

void Text( VGfloat x, VGfloat y, const char* text, Fontinfo font, int fontSize ) {
	TextString<Text>( x, y, text, font, fontSize );
}
void TextMid( VGfloat x, VGfloat y, const char* text, Fontinfo font, int fontSize ) {
	TextString<TextMid>( x, y, text, font, fontSize );
}
void TextEnd( VGfloat x, VGfloat y, const char* text, Fontinfo font, int fontSize ) {
	TextString<TextEnd>( x, y, text, font, fontSize );
}

void Text( VGfloat x, VGfloat y, const string& text, Fontinfo font, int fontSize ) {
	TextString<Text>( x, y, text, font, fontSize );
}
void TextMid( VGfloat x, VGfloat y, const string& text, Fontinfo font, int fontSize ) {
	TextString<TextMid>( x, y, text, font, fontSize );
}
void TextEnd( VGfloat x, VGfloat y, const string& text, Fontinfo font, int fontSize ) {
	TextString<TextEnd>( x, y, text, font, fontSize );
}