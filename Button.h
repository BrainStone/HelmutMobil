#include "_CompileFlags.h"

#ifndef Button_H
#define Button_H

// Includes
#include <list>
#include <string>
#include <string.h>

#include "VG/openvg.h"
#include "VG/vgu.h"

#include "fontinfo.h"
#include "GraphicsHelper.h"
#include "shapes.h"
#include "TouchScreen.h"

// Class
class Button {
public:
	struct Color {
		unsigned int red, green, blue;
		VGfloat alpha;

		Color() = delete;
		Color( unsigned int red, unsigned int green, unsigned int blue, VGfloat alpha = 1.0 )
			: red( red ), green( green ), blue( blue ), alpha( alpha ) {}
	};

	static const int defaultFontsize = 30;
	static const Color defaultBackgroundUp;
	static const Color defaultBackgroundDown;
	static const Color defaultFont;

private:
	char* text;
	Color backgroundUp;
	Color backgroundDown;
	Color font;

protected:
	bool down;
	bool active;
	VGfloat x, y;
	int fontsize;
	VGfloat width, height;
	std::list<void( *)()> eventCallbacks;

	void setUp();

	virtual bool mouseOnButton( double x, double y ) const;

	static std::list<Button*> Buttons;
	static bool initialized;

public:
	Button() = delete;
	Button( const Button& ) = delete;

	Button( const std::string& text,
			VGfloat x,
			VGfloat y,
			int width,
			int height,
			int fontsize = defaultFontsize,
			Color backgroundUp = defaultBackgroundUp,
			Color backgroundDown = defaultBackgroundDown,
			Color font = defaultFont );
	Button( const std::string& text,
			VGfloat x,
			VGfloat y,
			int fontsize = defaultFontsize,
			Color backgroundUp = defaultBackgroundUp,
			Color backgroundDown = defaultBackgroundDown,
			Color font = defaultFont );
	Button( const char* text,
			VGfloat x,
			VGfloat y,
			int width,
			int height,
			int fontsize = defaultFontsize,
			Color backgroundUp = defaultBackgroundUp,
			Color backgroundDown = defaultBackgroundDown,
			Color font = defaultFont );
	Button( const char* text,
			VGfloat x,
			VGfloat y,
			int fontsize = defaultFontsize,
			Color backgroundUp = defaultBackgroundUp,
			Color backgroundDown = defaultBackgroundDown,
			Color font = defaultFont );

	~Button();

	virtual void render();

	virtual void mouseEvent( bool down );
	virtual void moveEvent( double x, double y );

	void setActive( bool active );

	void addEventCallback( void( *callback )() );

	static void handleMouseEvent( bool down );
	static void handleMoveEvent( double x, double y );
};

class ButtonCustomRender : public Button {
private:
	void( *customRender )(bool, VGfloat, VGfloat, int, int);

public:
	ButtonCustomRender( const std::string&, VGfloat, VGfloat, int, int, int, Color, Color, Color ) = delete;
	ButtonCustomRender( const std::string&, VGfloat, VGfloat, int, Color, Color, Color ) = delete;
	ButtonCustomRender( const char*, VGfloat, VGfloat, int, int, int, Color, Color, Color ) = delete;
	ButtonCustomRender( const char*, VGfloat, VGfloat, int, Color, Color, Color ) = delete;

	ButtonCustomRender( void( *customRender )(bool, VGfloat, VGfloat, int, int),
						VGfloat x,
						VGfloat y,
						int width,
						int height );

	virtual void render();
};

#endif // #ifndef Button_H