#pragma once

#include "CommandHandler.h"
#include "Action.h"
#include "shared_ptr.h"
#include <vector>

class Window;

class View : public CommandHandler
{
public:
	View(CommandHandler *parentView, Rect box)
	: CommandHandler(parentView), mFrame(box) {}
	virtual ~View() {}
	
	void SetAction(ActionBase* action) { mAction = std::auto_ptr<ActionBase>(action); }
	
	virtual void HandleClick(EventRecord *);
	virtual void SetFrame(Rect box);
	virtual Rect GetFrame() { return mFrame; }
	virtual Rect GetFrameInWindow();
	virtual Rect GetBounds() {
		Rect box = { 0, 0, mFrame.bottom - mFrame.top, mFrame.right - mFrame.left };
		return box;
	}
	void Center();
	
	void NeedsRedraw();
	
	virtual void Draw();
	
	void DrawWithParentViewOffset(Point offset);
	
	View* GetSuperview();
	Window* GetWindow();
	
	void AddSubview(std::shared_ptr<View> subview) { mSubviews.push_back(subview); subview->SetSuperview(this); }
	
	virtual long IdleView(); // Sent to all views. Idle() only sent to focused view.
		
protected:
	void SetSuperview(View *parent) { mNextHandler = parent; }
	
	std::vector<std::shared_ptr<View> > mSubviews;
	Rect mFrame;
	std::auto_ptr<ActionBase> mAction;
};
