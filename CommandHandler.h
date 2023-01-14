#pragma once

class CommandHandler
{
public:
	CommandHandler(CommandHandler *nextHandler) : mNextHandler(nextHandler) {}

	virtual void MenuItemSelected(short menuID, short itemNumber);
	virtual bool UpdateMenuItem(short menuID, short itemNumber);
	
	virtual void HandleKeyDown(EventRecord *event);
	virtual void HandleKeyUp(EventRecord *event);
	
	virtual long Idle();
	
protected:
	CommandHandler *mNextHandler;
};
