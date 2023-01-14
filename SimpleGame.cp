#include "SimpleGame.h"
#include "FullscreenWindow.h"
#include "SpriteView.h"


void GameApplication::SetUp() {
	Application::SetUp();
	
	Window * backdropWindow = new FullscreenWindow(this, 128);
	backdropWindow->InitWindow();
	Window * gameWindow = new Window(this, 128, Window::kWindowFlagBuffered | Window::kWindowFlagNotUserCloseable);
	gameWindow->InitWindow();
	Rect wdRect = { 0, 0, 480, 640 };
	gameWindow->SizeWindow(wdRect.right - wdRect.left, wdRect.bottom - wdRect.top);
	std::shared_ptr<FilledBoxView> stageView = std::shared_ptr<FilledBoxView>(new FilledBoxView(gameWindow, wdRect));
	RGBColor stageColor = { 0xffff, 0xaaaa, 0xaaaa };
	stageView->SetColor(&stageColor);
	gameWindow->SetContentView(std::static_pointer_cast<View>(stageView));
	gameWindow->Center();
	Rect spriteBox = { 10, 10, 73, 73 };
	stageView->AddSubview(std::shared_ptr<View>(new SpriteView(stageView.get(), spriteBox)));
	Rect spriteBox2 = { 50, 40, 113, 103 };
	stageView->AddSubview(std::shared_ptr<View>(new SpriteView(stageView.get(), spriteBox2)));
}


void main(void)
{
	GameApplication theApplication;
	
	theApplication.Run();
}