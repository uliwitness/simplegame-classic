#pragma once

#include "CommandHandler.h"
#include "View.h"
#include <Windows.h>


class Window : public CommandHandler
{
public:
	enum {
		kWindowFlagBuffered = 1 << 0,
		kWindowFlagBufferedIfPossible = 1 << 1,
		kWindowFlagNotUserCloseable = 1 << 2
	};
	typedef UInt32 WindowFlags;

	Window(CommandHandler *nextHandler, short windID, WindowFlags flags = 0);
    virtual ~Window();
    
    virtual void InitWindow();
    
    std::shared_ptr<View> GetContentView() { return mContentView; }
    void SetContentView(std::shared_ptr<View> vw) { mContentView = vw; }
	
	virtual void CloseBoxClicked(CommandHandler *sender = NULL);
	virtual void HandleClick(EventRecord *event);
	
	virtual Rect GetGrowLimitRect() {
		Rect limits = { 100, 100, 32767, 32767 };
		return limits;
	}
	virtual void SizeWindow(short width, short height);
	virtual void MenuItemSelected(short menuID, short itemNumber);
	virtual bool UpdateMenuItem(short menuID, short itemNumber);
	virtual void Draw();
	
	void SetWindowPICT(short resID);
	
	void Center();

	virtual long Idle();
	
	WindowPtr GetMacWindow() { return mWindow; }

protected:
	GWorldPtr mBuffer;
	WindowPtr mWindow;
	std::shared_ptr<View> mContentView;
	WindowFlags mFlags;
};
