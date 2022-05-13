#include "../Framework.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(
	HWND window,
	UINT message,
	WPARAM wideParameter,
	LPARAM longParameter
);

long __stdcall WindowProcess(
	HWND window,
	UINT message,
	WPARAM wideParameter,
	LPARAM longParameter)
{
	if (ImGui_ImplWin32_WndProcHandler(window, message, wideParameter, longParameter))
		return true;

	switch (message)
	{
	case WM_SIZE: {
		if (Menu::device && wideParameter != SIZE_MINIMIZED)
		{
			Menu::presentParameters.BackBufferWidth = LOWORD(longParameter);
			Menu::presentParameters.BackBufferHeight = HIWORD(longParameter);
			Menu::ResetDevice();
		}
	}return 0;

	case WM_SYSCOMMAND: {
		if ((wideParameter & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
	}break;

	case WM_DESTROY: {
		PostQuitMessage(0);
	}return 0;

	case WM_LBUTTONDOWN: {
		Menu::position = MAKEPOINTS(longParameter); // set click points
	}return 0;

	case WM_MOUSEMOVE: {
		if (wideParameter == MK_LBUTTON)
		{
			const auto points = MAKEPOINTS(longParameter);
			auto rect = ::RECT{ };

			GetWindowRect(Menu::window, &rect);

			rect.left += points.x - Menu::position.x;
			rect.top += points.y - Menu::position.y;

			if (Menu::position.x >= 0 &&
				Menu::position.x <= Menu::WIDTH &&
				Menu::position.y >= 0 && Menu::position.y <= 19)
				SetWindowPos(
					Menu::window,
					HWND_TOPMOST,
					rect.left,
					rect.top,
					0, 0,
					SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOZORDER
				);
		}

	}return 0;

	}

	return DefWindowProc(window, message, wideParameter, longParameter);
}

void Menu::CreateHWindow(const char* windowName, const char* className) noexcept
{
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_CLASSDC;
	windowClass.lpfnWndProc = WindowProcess;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = GetModuleHandleA(0);
	windowClass.hIcon = 0;
	windowClass.hCursor = 0;
	windowClass.hbrBackground = 0;
	windowClass.lpszMenuName = 0;
	windowClass.lpszClassName = className;
	windowClass.hIconSm = 0;

	RegisterClassEx(&windowClass);

	window = CreateWindowEx(
		0,
		className,
		windowName,
		WS_POPUP,
		100,
		100,
		WIDTH,
		HEIGHT,
		0,
		0,
		windowClass.hInstance,
		0
	);

	ShowWindow(window, SW_SHOWDEFAULT);
	UpdateWindow(window);
}

void Menu::DestroyHWindow() noexcept
{
	DestroyWindow(window);
	UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
}

bool Menu::CreateDevice() noexcept
{
	d3d = Direct3DCreate9(D3D_SDK_VERSION);

	if (!d3d)
		return false;

	ZeroMemory(&presentParameters, sizeof(presentParameters));

	presentParameters.Windowed = TRUE;
	presentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
	presentParameters.BackBufferFormat = D3DFMT_UNKNOWN;
	presentParameters.EnableAutoDepthStencil = TRUE;
	presentParameters.AutoDepthStencilFormat = D3DFMT_D16;
	presentParameters.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

	if (d3d->CreateDevice(
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		window,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&presentParameters,
		&device) < 0)
		return false;

	return true;
}

void Menu::ResetDevice() noexcept
{
	ImGui_ImplDX9_InvalidateDeviceObjects();

	const auto result = device->Reset(&presentParameters);

	if (result == D3DERR_INVALIDCALL)
		IM_ASSERT(0);

	ImGui_ImplDX9_CreateDeviceObjects();
}

void Menu::DestroyDevice() noexcept
{
	if (device)
	{
		device->Release();
		device = nullptr;
	}

	if (d3d)
	{
		d3d->Release();
		d3d = nullptr;
	}
}
ImFont* fontDefault;
ImFont* fontHeader;
ImFont* fontTitle;

void Menu::CreateImGui() noexcept
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ::ImGui::GetIO();

	io.IniFilename = NULL;

	//ImGui::StyleColorsDark();
	CreateCustomStyle();

	fontDefault = ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Ruda-Bold.ttf", 12);
	fontHeader = ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Ruda-Bold.ttf", 14);
	fontTitle = ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Ruda-Bold.ttf", 22);

	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX9_Init(device);
}

void Menu::DestroyImGui() noexcept
{
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void Menu::BeginRender() noexcept
{
	MSG message;
	while (PeekMessage(&message, 0, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&message);
		DispatchMessage(&message);

		if (message.message == WM_QUIT)
		{
			Globals::bRunning = !Globals::bRunning;
			return;
		}
	}

	// Start the Dear ImGui frame
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void Menu::EndRender() noexcept
{
	ImGui::EndFrame();

	device->SetRenderState(D3DRS_ZENABLE, FALSE);
	device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	device->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);

	device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(0, 0, 0, 255), 1.0f, 0);

	if (device->BeginScene() >= 0)
	{
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
		device->EndScene();
	}

	const auto result = device->Present(0, 0, 0, 0);

	// Handle loss of D3D9 device
	if (result == D3DERR_DEVICELOST && device->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
		ResetDevice();
}

void Menu::Render() noexcept
{
	ImGui::SetNextWindowPos({ 0, 0 });
	ImGui::SetNextWindowSize({ WIDTH, HEIGHT });
	ImGui::Begin(
		"Better.Ez",
		&Globals::bRunning,
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoMove
	);

	//menu
	//ImGui::ShowDemoWindow();

	ImGui::PushFont(fontTitle);
	ImGui::TextColored({ 1.f, 0.74f, 0.f, 1.f }, "Better.Ez (Ext)");
	ImGui::SameLine();
	ImGui::TextColored({ 0.22f, 0.22f, 0.27f, 1.f}, "v0.1");
	ImGui::PopFont();

	ImGui::PushFont(fontHeader);
	if (ImGui::CollapsingHeader("Aim"))
	{
		ImGui::PopFont();
		ImGui::PushFont(fontDefault);

		//majic
		if (ImGui::TreeNode("Aimbot"))
		{
			ImGui::Checkbox("Enabled", &Settings::aim.aimbotEnabled);
			ImGui::SliderFloat("Fov", &Settings::aim.aimbotFov, 0.f, 90.f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
			ImGui::SliderFloat("Smoothing", &Settings::aim.aimbotSmooth, 0.f, 90.f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
			ImGui::Checkbox("Target Teammates", &Settings::aim.aimbotTeammates);

			ImGui::TreePop();
		}
		if (ImGui::TreeNode("RCS"))
		{
			ImGui::Checkbox("Enabled", &Settings::aim.rcsEnabled);
			ImGui::SliderInt("Horizontal \%", &Settings::aim.rcsHorizontal, 0, 100, "%d", ImGuiSliderFlags_AlwaysClamp);
			ImGui::SliderInt("Vertical \%", &Settings::aim.rcsVertical, 0, 100, "%d", ImGuiSliderFlags_AlwaysClamp);
			
			ImGui::TreePop();
		}

		ImGui::PopFont();
	}

	ImGui::PushFont(fontHeader);
	if(ImGui::CollapsingHeader("Esp"))
	{
		ImGui::PopFont();
		ImGui::PushFont(fontDefault);

		//majic
		if (ImGui::TreeNode("Enemy Esp"))
		{
			ImGui::Checkbox("Enabled", &Settings::esp.glowEnemy);
			ImGui::ColorPicker4("Color", Settings::esp.glowEnemyColor);

			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Team Esp"))
		{
			ImGui::Checkbox("Enabled", &Settings::esp.glowTeam);
			ImGui::ColorPicker4("Color", Settings::esp.glowTeamColor);

			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Enemy Chams"))
		{
			ImGui::Checkbox("Enabled", &Settings::esp.chamsEnemy);
			ImGui::SliderFloat("Brightness", &Settings::esp.chamsEnemyBright, 0.f, 5.f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
			ImGui::ColorPicker3("Color", Settings::esp.chamsEnemyColor);

			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Team Chams"))
		{
			ImGui::Checkbox("Enabled", &Settings::esp.chamsTeam);
			ImGui::SliderFloat("Brightness", &Settings::esp.chamsTeamBright, 0.f, 5.f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
			ImGui::ColorPicker3("Color", Settings::esp.chamsTeamColor);

			ImGui::TreePop();
		}

		ImGui::PopFont();
	}

	ImGui::PushFont(fontHeader);
	if (ImGui::CollapsingHeader("Visuals"))
	{
		ImGui::PopFont();
		ImGui::PushFont(fontDefault);

		//majic
		ImGui::Checkbox("No Hands", &Settings::vis.noHands);

		ImGui::PopFont();
	}

	ImGui::PushFont(fontHeader);
	if (ImGui::CollapsingHeader("Misc"))
	{
		ImGui::PopFont();
		ImGui::PushFont(fontDefault);

		//majic
		ImGui::Checkbox("Radar", &Settings::misc.radar);
		ImGui::Spacing();
		ImGui::Checkbox("B-Hop", &Settings::misc.bhop);
		ImGui::SliderInt("B-Hop \%", &Settings::misc.bhopPercentage, 0, 100, "%d", ImGuiSliderFlags_AlwaysClamp);

		ImGui::PopFont();
	}

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();
	ImGui::PushFont(fontHeader);
	if (ImGui::CollapsingHeader("Entity List"))
	{
		ImGui::PopFont();
		ImGui::PushFont(fontDefault);

		//get all entities, sort by team, show stats

		ImGui::PopFont();
	}



	ImGui::End();
}

void Menu::CreateCustomStyle() noexcept
{
	ImGuiStyle* style = &ImGui::GetStyle();

	style->WindowPadding = ImVec2(15, 15);
	style->WindowRounding = 5.0f;
	style->FramePadding = ImVec2(5, 5);
	style->FrameRounding = 4.0f;
	style->ItemSpacing = ImVec2(12, 8);
	style->ItemInnerSpacing = ImVec2(8, 6);
	style->IndentSpacing = 25.0f;
	style->ScrollbarSize = 15.0f;
	style->ScrollbarRounding = 9.0f;
	style->GrabMinSize = 5.0f;
	style->GrabRounding = 3.0f;

	style->Colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
	style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_ChildBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style->Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style->Colors[ImGuiCol_Border] = ImVec4(0.80f, 0.80f, 0.83f, 0.88f);
	style->Colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
	style->Colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
	style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	//style->Colors[ImGuiCol_ComboBg] = ImVec4(0.19f, 0.18f, 0.21f, 1.00f);
	style->Colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_Button] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	//style->Colors[ImGuiCol_Column] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	//style->Colors[ImGuiCol_ColumnHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	//style->Colors[ImGuiCol_ColumnActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	//style->Colors[ImGuiCol_CloseButton] = ImVec4(0.40f, 0.39f, 0.38f, 0.16f);
	//style->Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.40f, 0.39f, 0.38f, 0.39f);
	//style->Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.40f, 0.39f, 0.38f, 1.00f);
	style->Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
	style->Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);
}