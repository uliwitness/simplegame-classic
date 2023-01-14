#include "Window.h"
#include "Application.h"
#include "MenuItems.h"
#include "QDOffscreen.h"
#include "Exception.h"

Window::Window(CommandHandler *nextHandler, short windID, WindowFlags flags)
: mWindow(NULL), CommandHandler(nextHandler), mBuffer(NULL), mFlags(flags) {
	mWindow = GetNewCWindow(windID,
							NULL,
							(WindowPtr) -1);
	if (!mWindow) {
		mWindow = GetNewWindow(windID,
								NULL,
								(WindowPtr) -1);
	}
	SetWRefCon(mWindow, (long)this);
	if ((flags & kWindowFlagBufferedIfPossible) || (flags & kWindowFlagBuffered)) {
		Rect gwBox = { 0, 0,
					mWindow->portRect.bottom - mWindow->portRect.top,
					mWindow->portRect.right - mWindow->portRect.left };
		QDErr err = NewGWorld(&mBuffer, 0, &gwBox, NULL, NULL, 0);
		if (err != noErr && (flags & kWindowFlagBuffered)) {
			throw Exception(err);
		}
	}
}

Window::~Window() {
	if (mBuffer) {
		DisposeGWorld(mBuffer);
		mBuffer = NULL;
	}
}

void Window::InitWindow() {
   mContentView = std::shared_ptr<View>(new View(this, mWindow->portRect));
}

void Window::CloseBoxClicked(CommandHandler *) {
	DisposeWindow(mWindow);
	delete this;
	HiliteMenu(0);
}

void Window::SizeWindow(short width, short height) {
	::SizeWindow(mWindow, width, height, true);
	Rect contentBox = { 0, 0, height, width };
	if (mBuffer) {
		if (gwFlagErr & UpdateGWorld(&mBuffer, 0, &contentBox, NULL, NULL, 0)) {
        	throw Exception(gwFlagErr);
        }
	}
	if (mContentView.get()) {
		mContentView->SetFrame(contentBox);
	}
}

void Window::MenuItemSelected(short menuID, short itemIndex) {
	if (menuID == kFileMenuID && itemIndex == kCloseMenuItem
		&& (mFlags & kWindowFlagNotUserCloseable) == 0) {
		CloseBoxClicked();
	} else {
		CommandHandler::MenuItemSelected(menuID, itemIndex);
	}
}

bool Window::UpdateMenuItem(short menuID, short itemIndex) {
	if (menuID == kFileMenuID && itemIndex == kCloseMenuItem
		&& (mFlags & kWindowFlagNotUserCloseable) == 0) {
		return true;
	} else {
		return CommandHandler::UpdateMenuItem(menuID, itemIndex);
	}
}

void Window::Draw() {
	GWorldPtr oldGW = NULL;
	GDHandle oldDevice = NULL;
	if (mBuffer) {
		GetGWorld(&oldGW, &oldDevice);
		SetGWorld(mBuffer, NULL);
		LockPixels(GetGWorldPixMap(mBuffer));
	}
	Point zeroPos = { 0, 0 };
	SetOrigin(0,0);
	mContentView->DrawWithParentViewOffset(zeroPos);
	SetOrigin(0,0);
	if (mBuffer) {
		SetGWorld(oldGW, oldDevice);
	}
	Rect redrawBox = (**((GrafPtr)oldGW)->visRgn).rgnBBox;
	CopyBits(&((GrafPtr)mBuffer)->portBits,
			&((GrafPtr)oldGW)->portBits,
			&redrawBox,
			&redrawBox,
			srcCopy, NULL);
	if (mBuffer) {
		UnlockPixels(GetGWorldPixMap(mBuffer));
	}
}

void Window::SetWindowPICT(short resID) {
	PicHandle pic = GetPicture(resID);
	DetachResource((Handle)pic);
	::SizeWindow(mWindow, (**pic).picFrame.right - (**pic).picFrame.left,
							(**pic).picFrame.bottom - (**pic).picFrame.top,
							true);
	SetWindowPic(mWindow, pic);
}

void Window::HandleClick(EventRecord *event) {
	EventRecord localEvt = *event;
	GrafPtr oldPort = NULL;
	GetPort(&oldPort);
	SetPort(GetMacWindow());
	GlobalToLocal(&localEvt.where);
	mContentView->HandleClick(&localEvt);
	SetPort(oldPort);
}


void Window::Center() {
   short left = qd.screenBits.bounds.right - qd.screenBits.bounds.left;
   short top = qd.screenBits.bounds.bottom - qd.screenBits.bounds.top - GetMBarHeight();
   left -= mWindow->portRect.right - mWindow->portRect.left;
   top -= mWindow->portRect.bottom - mWindow->portRect.top;
   left /= 2;
   top /= 2;
   ::MoveWindow(mWindow, left, GetMBarHeight() + top, true);
}

long Window::Idle() {
	long desiredTime = CommandHandler::Idle();
	long viewDesiredTime = mContentView->IdleView();
	if (desiredTime > viewDesiredTime) {
		return viewDesiredTime;
	} else {
		return desiredTime;
	}
}
