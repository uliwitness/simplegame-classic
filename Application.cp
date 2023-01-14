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


Application::Application() : CommandHandler(NULL) {
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
	MenuBarHandle menuBar = GetNewMBar(128);
	SetMenuBar(menuBar);
	DrawMenuBar();
	
	MenuHandle mh = GetMenuHandle(kAppleMenuID);
	AppendResMenu(mh, 'DRVR');
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
		for (int y = 1; y <= numItems; ++y) {
			if (firstHandler->UpdateMenuItem(menuID, y)) {
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
		Alert(128, NULL);
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
								firstHandler->MenuItemSelected(HiWord(menuChoice), LoWord(menuChoice));
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
							firstHandler->MenuItemSelected(HiWord(menuChoice), LoWord(menuChoice));
						}
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
				
				case nullEvent: {
					WindowPtr frontWindow = FrontWindow();
					CommandHandler * firstHandler = frontWindow ? (CommandHandler*) GetWRefCon(frontWindow) : this;
					idleTime = firstHandler->Idle();
					break;
				}
			}
		} catch(std::exception& err) {
			paramtext(err.what(), "", "", "");
			Alert(128, NULL);
		}
	}
}

void Application::MenuItemSelected(short menuID, short itemIndex) {
	if (menuID == kFileMenuID && itemIndex == kQuitMenuItem) {
		Quit();
	} else if (menuID == kAppleMenuID && itemIndex == kAboutMenuItem) {
		Window * aboutWindow = new Window(this, 128);
		aboutWindow->InitWindow();
		aboutWindow->SetWindowPICT(128);
		aboutWindow->Center();
		aboutWindow->GetContentView()->SetAction(new Action<Window>(aboutWindow, &Window::CloseBoxClicked));
	} else if (menuID == kAppleMenuID && itemIndex > kAboutMenuItem) {
		Str255 itemText = {0};
		MenuHandle appleMenu = GetMenuHandle(menuID);
		GetMenuItemText(appleMenu, itemIndex, itemText);
		OpenDeskAcc(itemText);
		HiliteMenu(0);
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
	} else if (menuID == kAppleMenuID && itemIndex > kAboutMenuItem) {
		return true;
	} else {
		return CommandHandler::UpdateMenuItem(menuID, itemIndex);
	}
}
