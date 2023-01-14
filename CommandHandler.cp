#include "CommandHandler.h"

void CommandHandler::MenuItemSelected(short menuID, short itemNumber) {
	if (mNextHandler) {
		mNextHandler->MenuItemSelected(menuID, itemNumber);
	}
}
	
bool CommandHandler::UpdateMenuItem(short menuID, short itemNumber) {
	if (mNextHandler) {
		return mNextHandler->UpdateMenuItem(menuID, itemNumber);
	} else {
		return false;
	}
}

void CommandHandler::HandleKeyDown(EventRecord *event) {
	if (mNextHandler) {
		return mNextHandler->HandleKeyDown(event);
	}
}

void CommandHandler::HandleKeyUp(EventRecord *event) {
	if (mNextHandler) {
		return mNextHandler->HandleKeyUp(event);
	}
}

long CommandHandler::Idle() {
	if (mNextHandler) {
		return mNextHandler->Idle();
	}
	return 6;
}
