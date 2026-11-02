#include <SDL3/SDL.h>
#include "SceneManager.h"
#include "VulkanRenderer.h"
#include "OpenGLRenderer.h"
#include "Timer.h"
#include "Scene0.h"
#include "Debug.h"

SceneManager::SceneManager(): 
	currentScene(nullptr), timer(nullptr),
	fps(60), isRunning(false), rendererType(RendererType::VULKAN),
	renderer(nullptr) {}

SceneManager::~SceneManager() {
	if (currentScene) {
		currentScene->OnDestroy();
		delete currentScene;
		currentScene = nullptr;
	}
	
	if (timer) {
		delete timer;
		timer = nullptr;
	}
	renderer->OnDestroy();
	delete renderer;
	Debug::Info("Deleting the GameSceneManager", __FILE__, __LINE__);

}

bool SceneManager::Initialize(std::string name_, int width_, int height_) {

	switch(rendererType){
	case RendererType::OPENGL:
		renderer = new OpenGLRenderer();
		renderer->setRendererType(RendererType::OPENGL);
		renderer->CreateWindow(name_, width_, height_);
		renderer->OnCreate();
			
		
		break;

	case RendererType::VULKAN:
		renderer = new VulkanRenderer();
		renderer->setRendererType(RendererType::VULKAN);
		renderer->CreateWindow(name_, width_, height_);
		renderer->OnCreate();
		break;

	case RendererType::DIRECTX11:
	case RendererType::DIRECTX12:
		Debug::FatalError("Renderer not yet supported", __FILE__, __LINE__);
		return false;
		break;
	}

	timer = new Timer();
	if (timer == nullptr) {
		Debug::FatalError("Failed to initialize Timer object", __FILE__, __LINE__);
		return false;
	}

	BuildScene(SCENE0);
	
	return true;
}

#include "Profiler.h"

static ImVec4 ColorForMs(float ms) {
	if (ms <= 0.8f) return ImVec4(0, 1, 0, 1); // Green
	if (ms < 1.0f)  return ImVec4(1, 1, 0, 1); // Orange
	return ImVec4(1, 0, 0, 1); // Red
}

void SceneManager::Run() {
	Profiler profilerMain;

	float timeSinceLastReport = 0.0f;
	unsigned int frameCount = 0;
	float measuredFps = 0;

	timer->Start();
	isRunning = true;
	while (isRunning) {
		timer->UpdateFrameTicks();
		frameCount++;

		// imgui
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplSDL3_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin("Scene Statistics", nullptr, ImGuiChildFlags_AlwaysAutoResize);
		
		ImGui::SetNextWindowSize(ImVec2(400, 400), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);

		ImGui::BeginChild("FPS", ImVec2(100, 60), ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_Borders);
		ImGui::SetWindowFontScale(1.5f);

		timeSinceLastReport += timer->GetDeltaTime();
		if (timeSinceLastReport >= 1.0f) {
			measuredFps = frameCount / timeSinceLastReport;
			frameCount = 0;
			timeSinceLastReport = 0.0f;		
		}

		ImGui::Text("FPS: %.0f", measuredFps);
		ImGui::EndChild();

		ImGui::SameLine();

		ImGui::BeginChild("Scene Timings", ImVec2(0,60), ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_Borders);
		ImGui::SetWindowFontScale(1.5f);

		float updateTime = profilerMain.GetElapsedTime("Update").count();
		float renderTime = profilerMain.GetElapsedTime("Render").count();

		ImGui::TextColored(ColorForMs(updateTime), "Update: %.2fms", updateTime);
		ImGui::TextColored(ColorForMs(renderTime), "Render: %.2fms", renderTime);
		ImGui::EndChild();

		profilerMain.reset();

		{
			ScopedTimer t(profilerMain, "Update");
			currentScene->Update(timer->GetDeltaTime());
		}
		{
			ScopedTimer t(profilerMain, "Render");
			currentScene->Render();
		}

		GetEvents();
		
		SDL_Delay(timer->GetSleepTime(fps));	
	}
}

void SceneManager::GetEvents() {
	SDL_Event sdlEvent;

	while (SDL_PollEvent(&sdlEvent)) {

		ImGui_ImplSDL3_ProcessEvent(&sdlEvent); // gui takes all inputs first

		if (sdlEvent.type == SDL_EventType::SDL_EVENT_QUIT) {
			isRunning = false;
			return;
		}
		else if (sdlEvent.type == SDL_EVENT_KEY_DOWN) {
			switch (sdlEvent.key.scancode) {
			case SDL_SCANCODE_ESCAPE:
			case SDL_SCANCODE_Q:
				isRunning = false;
				return;

			case SDL_SCANCODE_F1:
				///BuildScene(SCENE1);
				break;

			case SDL_SCANCODE_F2:
				///BuildScene(SCENE2);
				break;

			case SDL_SCANCODE_F3:
				///BuildScene(SCENE3);
				break;

			case SDL_SCANCODE_F4:
				///BuildScene(SCENE4);
				break;

			case SDL_SCANCODE_F5:
				///BuildScene(SCENE5);
				break;

			case SDL_SCANCODE_F6:
				///BuildScene(SCENE6);
				break;

			default:
				break;
			}
		}
		if (currentScene == nullptr) {
			Debug::FatalError("Failed to initialize Scene", __FILE__, __LINE__);
			isRunning = false;
			return;
		}
		
		currentScene->HandleEvents(sdlEvent);
	}
}

void SceneManager::BuildScene(SCENE_NUMBER scene) {
	bool status; 

	if (currentScene != nullptr) {
		delete currentScene;
		currentScene = nullptr;
	}

	switch (scene) {
	case SCENE0:  
		currentScene = new Scene0(renderer);
		status = currentScene->OnCreate();
		break;

	case SCENE1:
		///currentScene = new Scene1();
		status = currentScene->OnCreate();
		break;

	case SCENE2:
		///currentScene = new Scene2();
		status = currentScene->OnCreate();
		break;

	case SCENE3:
		///currentScene = new Scene3();
		status = currentScene->OnCreate();
		break;

	case SCENE4:
		///currentScene = new Scene4();
		//status = currentScene->OnCreate();
		break;

	case SCENE5:
		///currentScene = new Scene5();
		status = currentScene->OnCreate();
		break;

	case SCENE6:
		///currentScene = new Scene6();
		status = currentScene->OnCreate();
		break;

	default:
		Debug::Error("Incorrect scene number assigned in the manager", __FILE__, __LINE__);
		currentScene = nullptr;
		break;
	}	
}


