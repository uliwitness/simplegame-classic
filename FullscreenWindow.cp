#include "FullscreenWindow.h"
#include "MenuItems.h"

FullscreenWindow::FullscreenWindow(CommandHandler *nextHandler, short windID, WindowFlags flags)
: Window(nextHandler, windID, flags) {
   short w = qd.screenBits.bounds.right - qd.screenBits.bounds.left;
   short h = qd.screenBits.bounds.bottom - qd.screenBits.bounds.top;
   ::MoveWindow(mWindow, 0, 0, true);
   SizeWindow(w, h);
}

void FullscreenWindow::InitWindow() {
   mContentView = std::shared_ptr<View>(new FilledBoxView(this, mWindow->portRect));
}

void FullscreenWindow::MenuItemSelected(short menuID, short itemIndex) {
	if (menuID == kFileMenuID && itemIndex == kCloseMenuItem) {
		;
	} else {
		Window::MenuItemSelected(menuID, itemIndex);
	}
}

bool FullscreenWindow::UpdateMenuItem(short menuID, short itemIndex) {
	if (menuID == kFileMenuID && itemIndex == kCloseMenuItem) {
		return false;
	} else {
		return Window::UpdateMenuItem(menuID, itemIndex);
	}
}
