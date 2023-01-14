#pragma once

#include "View.h"

class FilledBoxView : public View
{
public:
	FilledBoxView(CommandHandler *parentView, Rect box)
	: View(parentView, box) { mColor.red = mColor.green = mColor.blue = 0x0000; }
	
	virtual void Draw();
	
	void SetColor(RGBColor *color) { mColor = *color; NeedsRedraw(); }
	void GetColor(RGBColor *outColor) { *outColor = mColor; }
	
protected:
	RGBColor mColor;
};
