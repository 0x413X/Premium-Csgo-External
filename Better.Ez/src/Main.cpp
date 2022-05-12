#include "Framework.h"

int __stdcall wWinMain(
	HINSTANCE instance,
	HINSTANCE previousInstance,
	PWSTR arguments,
	int commandShow
)
{
	//init memory and cheats

	//create gui
	Menu::CreateHWindow("Cool Menu", "Menu Class");
	Menu::CreateDevice();
	Menu::CreateImGui();

	//main loop
	while (Menu::bRunning)
	{
		Menu::BeginRender();
		Menu::Render();
		Menu::EndRender();

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	Menu::DestroyImGui();
	Menu::DestroyDevice();
	Menu::DestroyHWindow();

	return EXIT_SUCCESS;
}
