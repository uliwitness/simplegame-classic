#include "View.h"
#include "Window.h"
#include <Quickdraw.h>

inline static bool IntersectRect(const Rect *boxA, const Rect *boxB) {
	Rect sectBox;
	SectRect(boxA, boxB, &sectBox);
	return !EmptyRect(&sectBox);
}

void View::DrawWithParentViewOffset(Point offset) {
	Point myOffset = { offset.v + mFrame.top, offset.h + mFrame.left };
	SetOrigin(-myOffset.h, -myOffset.v);
	
	GrafPtr port = NULL;
	GetPort(&port);
	Rect visBox = (**port->visRgn).rgnBBox;
	Draw();
	
	std::vector<std::shared_ptr<View> >::iterator subviewItty;
	for (subviewItty = mSubviews.begin(); subviewItty != mSubviews.end(); ++subviewItty) {
		Rect frame = (*subviewItty)->GetFrame();
		if (IntersectRect(&visBox, &frame)) {
			(*subviewItty)->DrawWithParentViewOffset(myOffset);
		}
	}
}

View* View::GetSuperview() {
	return dynamic_cast<View*>(mNextHandler);
}

Window* View::GetWindow() {
	Window *parent = dynamic_cast<Window*>(mNextHandler);
	if (!parent) {
		View *superview = GetSuperview();
		if (superview) {
			parent = superview->GetWindow();
		}
	}
	return parent;
}

void View::Draw() {
	Rect box = GetBounds();
	EraseRect(&box);
}

long View::IdleView() {
	long minTime = LONG_MAX;
	std::vector<std::shared_ptr<View> >::iterator subviewItty;
	for (subviewItty = mSubviews.begin(); subviewItty != mSubviews.end(); ++subviewItty) {
		long subviewTime = (*subviewItty)->IdleView();
		if (subviewTime < minTime) {
			minTime = subviewTime;
		}
	}
	return minTime;
}

void View::SetFrame(Rect box) {
	NeedsRedraw();
	mFrame = box;
	NeedsRedraw();
}

Rect View::GetFrameInWindow() {
	Rect box = mFrame;
	View *vw = GetSuperview();
	while (vw) {
		Rect currBox = vw->GetFrame();
		OffsetRect(&box, currBox.left, currBox.top);
		vw = vw->GetSuperview();
	}
	return box;
}

void View::NeedsRedraw() {
	Window *wd = GetWindow();
	if (!wd) return;
	GrafPtr macWd = wd->GetMacWindow();
	if (!macWd) return;
	GrafPtr oldPort = NULL;
	GetPort(&oldPort);
	SetPort(macWd);
	Rect box = GetFrameInWindow();
	InvalRect(&box);
	SetPort(oldPort);
}

void View::Center() {
	View *sv = GetSuperview();
	if (!sv) return;
	
	Rect frame = mFrame;
	frame.right -= frame.left;
	frame.bottom -= frame.top;
	frame.left = frame.top = 0;

	Rect superBox = sv->GetFrame();
	short left = superBox.right - superBox.left;
	short top = superBox.bottom - superBox.top;
	left -= frame.right;
	top -= frame.bottom;
	left /= 2;
	top /= 2;
	OffsetRect(&frame, left, top);
	SetFrame(frame);
}

void View::HandleClick(EventRecord *event) {
	std::vector<std::shared_ptr<View> >::iterator subviewItty;
	for (subviewItty = mSubviews.begin(); subviewItty != mSubviews.end(); ++subviewItty) {
		Rect box = (*subviewItty)->GetFrame();
		if (PtInRect(event->where, &box)) {
			EventRecord localEvt = *event;
			localEvt.where.h -= mFrame.left;
			localEvt.where.v -= mFrame.top;
			(*subviewItty)->HandleClick(&localEvt);
			return;
		}
	}
	
	if (mAction.get() != NULL) {
		(*mAction)(this);
	}
}
