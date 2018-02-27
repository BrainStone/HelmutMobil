#include "_CompileFlags.h"

#ifndef VGHelper_H
#define VGHelper_H

#include <cstdlib>
#include <cstring>
#include <string>

#include <VG/openvg.h>
#include <VG/vgu.h>
#include <fontinfo.h>
#include <shapes.h>

// Functions

void Text( VGfloat x, VGfloat y, const char* text, Fontinfo font, int fontSize );
void TextMid( VGfloat x, VGfloat y, const char* text, Fontinfo font, int fontSize );
void TextEnd( VGfloat x, VGfloat y, const char* text, Fontinfo font, int fontSize );

void Text( VGfloat x, VGfloat y, const std::string& text, Fontinfo font, int fontSize );
void TextMid( VGfloat x, VGfloat y, const std::string& text, Fontinfo font, int fontSize );
void TextEnd( VGfloat x, VGfloat y, const std::string& text, Fontinfo font, int fontSize );

#endif // #ifndef VGHelper_H