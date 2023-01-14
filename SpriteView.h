#pragma once

#include "View.h"

class SpriteView : public View
{
public:
	SpriteView(CommandHandler *parentView, Rect box)
	: View(parentView, box), mFirstFrameID(128), mCurrentFrameID(128), mLastFrameID(135) {}
	
	virtual void Draw();
	
	virtual long IdleView(); // Sent to all views. Idle() only sent to focused view.
		
protected:
	short mFirstFrameID;
	short mLastFrameID;
	short mCurrentFrameID;
};
