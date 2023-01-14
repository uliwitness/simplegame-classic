#include <Dialogs.h>
#include <Fonts.h>
#include <MacWindows.h>
#include <Menus.h>
#include <QuickDraw.h>
#include <TextEdit.h>
#include "Application.h"
#include "Window.h"
#include "CommandHandler.h"
#include "MenuItems.h"


Application *Application::sApplication = NULL;


Application::Application()
: CommandHandler(NULL), mOldGrayRgn(NULL), mOldMBarHeight(0), mBarWasHiddenBeforeSuspend(false) {
	mQuit = false;
	sApplication = this;
}

void Application::Initialize() {
	InitGraf(&qd.thePort);
	InitFonts();
	InitWindows();
	InitMenus();
	TEInit();
	InitDialogs(nil);
	InitCursor();
}

void Application::SetUp() {
	// Load base menu bar:
	MenuBarHandle menuBar = GetNewMBar(128);
	SetMenuBar(menuBar);
	DrawMenuBar();
	
	// Set up system-provided Apple menu items:
	MenuHandle mh = GetMenuHandle(kAppleMenuID);
	AppendResMenu(mh, 'DRVR');
	
	// Either insert our help menu in the menu bar, or append its items to the system help menu:
	MenuHandle help = GetMenu(kHelpMenuID);
	MenuHandle systemHelp = NULL;
	if (noErr != HMGetHelpMenuHandle(&systemHelp)) {
		InsertMenu(help, 0);
		mSystemHelpMItemCount = 0;
		mSystemHelpMenuID = kHelpMenuID;
	} else {
		short numItems = CountMItems(help);
		mSystemHelpMItemCount = CountMItems(systemHelp);
		mSystemHelpMenuID = (**systemHelp).menuID;
		for (int x = 1; x <= numItems; ++x) {
			Str255 itemName = {0};
			CharParameter commandChar = 0;
			GetMenuItemText(help, x, itemName);
			GetItemCmd(help, x, &commandChar);
			AppendMenu(systemHelp, "\pxxx");
			SetMenuItemText(systemHelp, mSystemHelpMItemCount + x, itemName);
			SetItemCmd(systemHelp, mSystemHelpMItemCount + x, commandChar);
		}
	}
}

void Application::UpdateMenuItems() {
	WindowPtr frontWindow = FrontWindow();
	CommandHandler * firstHandler = frontWindow ? (CommandHandler*) GetWRefCon(frontWindow) : this;
	for (short menuID = kAppleMenuID; true; ++menuID) {
		MenuHandle mh = GetMenuHandle(menuID);
		if (!mh) {
			break;
		}
		short numItems = CountMItems(mh);
		short itemIndexOffset = (menuID == mSystemHelpMenuID) ? -mSystemHelpMItemCount : 0;
		short menuIDToReport = (menuID == mSystemHelpMenuID) ? kHelpMenuID : menuID;
		for (int y = 1; y <= numItems; ++y) {
			if (firstHandler->UpdateMenuItem(menuIDToReport, y + itemIndexOffset)) {
				EnableItem(mh, y);
			} else  {
				DisableItem(mh, y);
			}
		}
	}
}

void Application::Run()
{
	RgnHandle mouseRgn = NULL;
	long idleTime = 6;
	
	Initialize();
	try {
		SetUp();
	} catch(std::exception& err) {
		paramtext(err.what(), "", "", "");
		Alert(kErrorAlertID, NULL);
		return;
	}
	
	while (!mQuit) {
		EventRecord event = {0};
		if (!WaitNextEvent(everyEvent, &event, 10, mouseRgn)) {
			WindowPtr frontWindow = FrontWindow();
			CommandHandler * firstHandler = frontWindow ? (CommandHandler*) GetWRefCon(frontWindow) : this;
			idleTime = firstHandler->Idle();
			continue;
		}
		
		try {
			switch( event.what ) {
				case mouseDown: {
					WindowPtr currentWindow = NULL;
					WindowPartCode part = FindWindow(event.where, &currentWindow);
					Window *windowObject = currentWindow ? (Window*) GetWRefCon(currentWindow) : NULL;
					switch( part ) {
						case inMenuBar: {
							WindowPtr frontWindow = FrontWindow();
							CommandHandler * firstHandler = frontWindow ? (CommandHandler*) GetWRefCon(frontWindow) : this;
							UpdateMenuItems();
							
							long menuChoice = MenuSelect(event.where);
							if (menuChoice) {
								short menuID = HiWord(menuChoice);
								short itemIndex = LoWord(menuChoice);
								if (itemIndex > 0) {
									short itemIndexOffset = (menuID == mSystemHelpMenuID) ? -mSystemHelpMItemCount : 0;
									short menuIDToReport = (menuID == mSystemHelpMenuID) ? kHelpMenuID : menuID;
									firstHandler->MenuItemSelected(menuIDToReport, itemIndex + itemIndexOffset);
								}
							}
							break;
						}
						
						case inSysWindow:
							SystemClick(&event, currentWindow);
							break;
						
						case inGoAway:
							if( TrackGoAway(currentWindow, event.where) ) {
								windowObject->CloseBoxClicked();
							}
							break;
						
						case inContent:
							windowObject->HandleClick(&event);
							break;
						
						case inDrag:
							DragWindow(currentWindow, event.where, &(**GetGrayRgn()).rgnBBox);
							break;
							
						case inGrow:
						{
							Rect limits = windowObject->GetGrowLimitRect();
							long newSize = GrowWindow(currentWindow, event.where, &limits);
							if( newSize != 0 ) {
								windowObject->SizeWindow(LoWord(newSize), HiWord(newSize));
							}
							break;
						}
					}
					break;
				}
				
				case keyDown: {
					WindowPtr frontWindow = FrontWindow();
					CommandHandler * firstHandler = frontWindow ? (CommandHandler*) GetWRefCon(frontWindow) : this;
					if (event.modifiers & cmdKey) {
						UpdateMenuItems();
						long menuChoice = MenuKey(event.message & charCodeMask);
						if (menuChoice) {
							short menuID = HiWord(menuChoice);
							short itemIndexOffset = (menuID == mSystemHelpMenuID) ? -mSystemHelpMItemCount : 0;
							short menuIDToReport = (menuID == mSystemHelpMenuID) ? kHelpMenuID : menuID;
							firstHandler->MenuItemSelected(menuIDToReport, LoWord(menuChoice) + itemIndexOffset);
						}
					} else {
						firstHandler->HandleKeyDown(&event);
					}
					break;
				}
				
				case autoKey: {
					WindowPtr frontWindow = FrontWindow();
					CommandHandler * firstHandler = frontWindow ? (CommandHandler*) GetWRefCon(frontWindow) : this;
					if ((event.modifiers & cmdKey) == 0) {
						firstHandler->HandleKeyDown(&event);
					}
					break;
				}
				
				case keyUp: {
					WindowPtr frontWindow = FrontWindow();
					CommandHandler * firstHandler = frontWindow ? (CommandHandler*) GetWRefCon(frontWindow) : this;
					if ((event.modifiers & cmdKey) == 0) {
						firstHandler->HandleKeyUp(&event);
					}
					break;
				}
				
				case updateEvt: {
					GrafPtr oldPort = NULL;
					GetPort( &oldPort );
					WindowPtr currentWindow = (WindowPtr)event.message;
					Window *windowObject = currentWindow ? (Window*) GetWRefCon(currentWindow) : NULL;
					SetPort(currentWindow);
					BeginUpdate(currentWindow);
					windowObject->Draw();
					EndUpdate(currentWindow);
					ValidRect(&currentWindow->portRect);
					SetPort(oldPort);
					break;
				}
				
				case osEvt: {
					switch ((event.message << 8) & 0xff) {
						case mouseMovedMessage:
							break;
						
						case suspendResumeMessage:
							if (event.message & resumeFlag) { // resume
								//if (mBarWasHiddenBeforeSuspend) {
								//	HideMenuBar();
								//}
							} else { // suspend
								//mBarWasHiddenBeforeSuspend = mOldGrayRgn != NULL;
								//ShowMenuBar();
							}
							break;
					}
					break;
				}
				
				case nullEvent: {
					WindowPtr frontWindow = FrontWindow();
					CommandHandler * firstHandler = frontWindow ? (CommandHandler*) GetWRefCon(frontWindow) : this;
					idleTime = firstHandler->Idle();
					break;
				}
			}
		} catch(std::exception& err) {
			paramtext(err.what(), "", "", "");
			Alert(kErrorAlertID, NULL);
		}
	}
	
	ShowMenuBar();
}

void Application::MenuItemSelected(short menuID, short itemIndex) {
	if (menuID == kFileMenuID && itemIndex == kQuitMenuItem) {
		Quit();
	} else if (menuID == kAppleMenuID && itemIndex == kAboutMenuItem) {
		Window * aboutWindow = new Window(this, kAboutWindowID);
		aboutWindow->InitWindow();
		aboutWindow->SetWindowPICT(kAboutWindowPICTID);
		aboutWindow->Center();
		aboutWindow->GetContentView()->SetAction(new Action<Window>(aboutWindow, &Window::CloseBoxClicked));
	} else if (menuID == kAppleMenuID && itemIndex > kAboutMenuItem) {
		Str255 itemText = {0};
		MenuHandle appleMenu = GetMenuHandle(menuID);
		GetMenuItemText(appleMenu, itemIndex, itemText);
		OpenDeskAcc(itemText);
		HiliteMenu(0);
	} else if (menuID == kViewMenuID && itemIndex == kMenuBarMenuItem) {
		ToggleMenuBar();
	} else if (menuID == kHelpMenuID && itemIndex == kHelpMenuItem) {
		paramtext("No Help Available.", "", "", "");
		Alert(kErrorAlertID, NULL);
	} else {
		Delay(6, NULL);
		HiliteMenu(0);
	}
}

bool Application::UpdateMenuItem(short menuID, short itemIndex) {
	if (menuID == kFileMenuID && itemIndex == kQuitMenuItem) {
		return true;
	} else if (menuID == kAppleMenuID && itemIndex == kAboutMenuItem) {
		return true;
	} else if (menuID == kAppleMenuID && itemIndex > kAboutMenuLastSeparator) {
		return true;
	} else if (menuID == kViewMenuID && itemIndex == kMenuBarMenuItem) {
		SetItemMark(GetMenuHandle(menuID), itemIndex, (mOldGrayRgn == NULL) ? checkMark : noMark);
		return true;
	} else if (menuID == kHelpMenuID && itemIndex == kHelpMenuItem) {
		return true;
	} else {
		return CommandHandler::UpdateMenuItem(menuID, itemIndex);
	}
}

void Application::HideMenuBar() {
	if (mOldGrayRgn == NULL) {
		mOldMBarHeight = LMGetMBarHeight();
		mOldGrayRgn = NewRgn();
		LMSetMBarHeight(0);
		RgnHandle realGrayRgn = LMGetGrayRgn();
		CopyRgn(realGrayRgn, mOldGrayRgn);
		RgnHandle mbarRgn = NewRgn();
		SetRectRgn(mbarRgn,
					0, 0,
					(**realGrayRgn).rgnBBox.right - (**realGrayRgn).rgnBBox.left, mOldMBarHeight);
		UnionRgn(realGrayRgn, mbarRgn, realGrayRgn);

		WindowPtr	fw = FrontWindow();
		if (fw)
		{
			PaintBehind(fw, mbarRgn);
			CalcVisBehind(fw,  mbarRgn);
		}
		DisposeRgn(mbarRgn);
	}
}

void Application::ShowMenuBar() {
	if (mOldGrayRgn != NULL) {
		LMSetMBarHeight(mOldMBarHeight);
		RgnHandle realGrayRgn = LMGetGrayRgn();
		CopyRgn(mOldGrayRgn, realGrayRgn);
		DisposeRgn(mOldGrayRgn);
		mOldGrayRgn = NULL;

		RgnHandle mbarRgn = NewRgn();
		SetRectRgn(mbarRgn,
					0, 0,
					(**realGrayRgn).rgnBBox.right - (**realGrayRgn).rgnBBox.left, mOldMBarHeight);
		WindowPtr	fw = FrontWindow();
		if (fw)
		{
			PaintBehind(fw, mbarRgn);
			CalcVisBehind(fw,  mbarRgn);
		}
		DisposeRgn(mbarRgn);
		DrawMenuBar();
		mOldMBarHeight = 0;
	}
}

