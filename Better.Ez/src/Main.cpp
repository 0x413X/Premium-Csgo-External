#include "Framework.h"

int __stdcall wWinMain(
	HINSTANCE instance,
	HINSTANCE previousInstance,
	PWSTR arguments,
	int commandShow
)
{
	//create console

	//init memory
	Memory mem{ "csgo.exe" };
	Globals::clientModule = mem.GetModuleAddress("client.dll");
	Globals::engineModule = mem.GetModuleAddress("engine.dll");

	//creating cheat threads
	std::cout << "Running Threads -> ";
	std::thread(Esp::Run, mem).detach();
	std::thread(Visuals::Run, mem).detach();
	std::cout << "SUCCESS" << std::endl;

	//create gui
	std::cout << "Creating Gui -> ";
	Menu::CreateHWindow("Cool Menu", "Menu Class");
	Menu::CreateDevice();
	Menu::CreateImGui();
	std::cout << "SUCCESS" << std::endl;

	//main loop
	while (Globals::bRunning)
	{
		Menu::BeginRender();
		Menu::Render();
		Menu::EndRender();

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	
	std::cout << "Destroying Gui -> ";
	Menu::DestroyImGui();
	Menu::DestroyDevice();
	Menu::DestroyHWindow();
	std::cout << "SUCCESS" << std::endl;

	//destroy console

	//destroy memory
	mem.~Memory();

	return EXIT_SUCCESS;
}
