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

long CommandHandler::Idle() {
	if (mNextHandler) {
		return mNextHandler->Idle();
	}
	return 6;
}
