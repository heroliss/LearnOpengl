#include "GUI.h"

#include <filesystem>
#include <iostream>
#ifdef _WIN32
#define NOMINMAX
#include <Windows.h>
#endif

GUI::GUI(const char* glsl_version, GLFWwindow* window)
	: glsl_version(glsl_version)
	, window(nullptr)
	, io(nullptr)
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();

	ImGui::CreateContext();

	// Mutable settings belong outside the extracted application directory.
	std::filesystem::path settings = std::filesystem::temp_directory_path() / "Liss/LearnOpenGL";
#ifdef _WIN32
	wchar_t localAppData[32768];
	const DWORD length = GetEnvironmentVariableW(L"LOCALAPPDATA", localAppData, 32768);
	if (length > 0 && length < 32768) settings = std::filesystem::path(localAppData) / "Liss/LearnOpenGL";
#endif
	std::filesystem::create_directories(settings);
	const auto ini = (settings / "imgui.ini").u8string();
	const auto log = (settings / "imgui_log.txt").u8string();
	iniPath.assign(ini.begin(), ini.end());
	logPath.assign(log.begin(), log.end());
	ImGui::GetIO().IniFilename = iniPath.c_str();
	ImGui::GetIO().LogFilename = logPath.c_str();

	io = &ImGui::GetIO();
	io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Prefer an installed CJK font, falling back to an installed Latin or built-in font.
#ifdef _WIN32
	wchar_t windowsDirectory[32768];
	const UINT windowsLength = GetWindowsDirectoryW(windowsDirectory, 32768);
	if (windowsLength > 0 && windowsLength < 32768) {
		for (const auto* name : { L"msyh.ttc", L"msyh.ttf", L"segoeui.ttf" }) {
			const auto path = std::filesystem::path(windowsDirectory) / "Fonts" / name;
			if (!std::filesystem::is_regular_file(path)) continue;
			const auto encoded = path.u8string();
			const std::string fontPath(encoded.begin(), encoded.end());
			if (io->Fonts->AddFontFromFileTTF(fontPath.c_str(), 16.0f, nullptr, io->Fonts->GetGlyphRangesChineseFull())) break;
		}
	}
#endif
	if (io->Fonts->Fonts.empty()) io->Fonts->AddFontDefault();

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);
}

GUI::~GUI()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void GUI::BeginFrame()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void GUI::EndFrame()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}




//Demo Window
bool show_demo_window = false;
bool show_another_window = false;
void GUI::ShowDemoWindow() {
	// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	if (show_demo_window)
		ImGui::ShowDemoWindow(&show_demo_window);

	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
	{
		static int counter = 0;

		ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

		ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
		ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
		ImGui::Checkbox("Another Window", &show_another_window);

		//ImGui::SliderFloat3("Translation", &translation.x, 0.0f, screenWidth);
		//ResetMVP();

		//ImGui::ColorEdit3("clear color", clear_color); // Edit 3 floats representing a color

		if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			counter++;
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		//ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
		ImGui::End();
	}

	// 3. Show another simple window.
	if (show_another_window)
	{
		ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		ImGui::Text("Hello from another window!");
		if (ImGui::Button("Close Me"))
			show_another_window = false;
		ImGui::End();
	}
}

