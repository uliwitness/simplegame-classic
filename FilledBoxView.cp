#include "FilledBoxView.h"

void FilledBoxView::Draw() {
	Rect box = GetBounds();
	RGBColor oldColor;
	GetForeColor(&oldColor);
	RGBForeColor(&mColor);
	PaintRect(&box);
	RGBForeColor(&oldColor);
}
