#pragma once

#include "Window.h"
#include "FilledBoxView.h"


class FullscreenWindow : public Window
{
public:
	FullscreenWindow(CommandHandler *nextHandler, short windID, WindowFlags flags = 0);
	
    virtual void InitWindow();
	
	virtual void MenuItemSelected(short menuID, short itemNumber);
	virtual bool UpdateMenuItem(short menuID, short itemNumber);
};
