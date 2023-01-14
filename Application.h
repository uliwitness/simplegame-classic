#pragma once

#include "CommandHandler.h"

class Application : public CommandHandler
{
public:
	Application();
	
	void Run();
	void Quit() { mQuit = true; }
	
	virtual void MenuItemSelected(short menuID, short itemIndex);
	virtual bool UpdateMenuItem(short menuID, short itemIndex);
	
	static Application *Shared() { return sApplication; }
	
	void ToggleMenuBar() { if (mOldGrayRgn) ShowMenuBar(); else HideMenuBar(); }
	void HideMenuBar();
	void ShowMenuBar();
	
protected:
	virtual void Initialize();
	virtual void SetUp();
	void UpdateMenuItems();
	
	bool mQuit;
	short mSystemHelpMenuID;
	short mSystemHelpMItemCount;
	RgnHandle mOldGrayRgn;
	short mOldMBarHeight;
	bool mBarWasHiddenBeforeSuspend;

	static Application * sApplication;
};
