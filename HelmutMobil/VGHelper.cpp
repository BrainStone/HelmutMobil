#include "VGHelper.h"

using namespace std;

template<void( *TextFunc )(VGfloat, VGfloat, const char*, Fontinfo, int)>
void TextString( VGfloat x, VGfloat y, const string& text, Fontinfo font, int fontSize ) {
	TextFunc( x, y, text.c_str(), font, fontSize );
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