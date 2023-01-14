#include "SpriteView.h"


void SpriteView::Draw() {
	Rect box = GetBounds();
	CIconHandle icn = GetCIcon(mCurrentFrameID);
	if (icn) {
		PlotCIcon(&box, icn);
		DisposeCIcon(icn);
	}
}

long SpriteView::IdleView() {
	if (mCurrentFrameID >= mLastFrameID) {
		mCurrentFrameID = mFirstFrameID;
	} else {
		++mCurrentFrameID;
	}
	NeedsRedraw();
	return 2;
}