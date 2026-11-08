#include <glew.h>
#include <iostream>
#include <cstdlib>
#include "Debug.h"
#include "Scene0.h"
#include <MMath.h>
#include "VulkanRenderer.h"
#include "OpenGLRenderer.h"
#include "Profiler.h"



Scene0::Scene0(Renderer *renderer_, Audio& audioSystem_):
	Scene(renderer_, audioSystem_),renderer(renderer_),
	camera(nullptr), actorPool(5), audioSystem(audioSystem_),
	selectedActor(0), stickLX(0), stickRX(0), stickLY(0), stickRY(0)
{
	Debug::Info("Created Scene0: ", __FILE__, __LINE__);
}

Scene0::~Scene0() {
}

bool Scene0::OnCreate() {

	int width = 0, height = 0;
	float aspectRatio;

	switch (renderer->getRendererType()) {
	case RendererType::VULKAN:
	{

		VulkanRenderer* vRenderer;
		vRenderer = dynamic_cast<VulkanRenderer*>(renderer);

		// Camera creation
		camera = new Camera(vRenderer);

		SDL_GetWindowSize(vRenderer->getWindow(), &width, &height);
		aspectRatio = static_cast<float>(width) / static_cast<float>(height);
		DualQuat initialOrientation(Vec3(0.0f, 0.0f, -5.0f));
		camera->OnCreate(aspectRatio, initialOrientation);

		lightsUBO = vRenderer->CreateUniformBuffers<LightsData>();

		// Lights setup
		lights.numLights = 3;
		lights.ambient = Vec4(0.1f, 0.1f, 0.1f, 0.0f);

		// R
		lights.diffuse[0] = redLight;
		lights.specular[0] = Vec4(0.0f, 0.5f, 0.0f, 0.0f);
		lights.pos[0] = Vec4(-12.0f, 0.0f, -5.0f, 0.0f);

		// G
		lights.diffuse[1] = greenLight;
		lights.specular[1] = Vec4(0.5f, 0.0f, 0.0f, 0.0f);
		lights.pos[1] = Vec4(0.0f, 5.0f, 0.0f, 0.0f);

		// B
		lights.diffuse[2] = blueLight;
		lights.specular[2] = Vec4(0.0f, 0.0f, 0.6f, 0.0f);
		lights.pos[2] = Vec4(15.0f, 0.0f, 0.0f, 0.0f);

		vRenderer->UpdateUniformBuffer<LightsData>(lights, lightsUBO);

		Quaternion orient = QMath::angleAxisRotation(90.0f, Vec3(0.0f, 1.0f, 0.0f));

		// Mario actor creation
		mario = actorPool.Acquire();

		mario->createTexture(vRenderer->Create2DTextureImage("./textures/mario_fire.png"));
		mario->createMesh(vRenderer->LoadModelIndexed("./meshes/Mario.obj"));

		DescriptorSetBuilder marioDescriptorSetBuilder(vRenderer->getDevice());
		marioDescriptorSetBuilder.add(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 1, camera->getCameraUBO());
		marioDescriptorSetBuilder.add(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 1, lightsUBO);
		marioDescriptorSetBuilder.add(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 1, &mario->GetSampler());
		DescriptorSetInfo marioDescSetInfo = marioDescriptorSetBuilder.BuildDescriptorSet(vRenderer->getNumSwapchains());
		PipelineInfo marioPipelineInfo = vRenderer->CreateGraphicsPipeline(marioDescSetInfo.descriptorSetLayout, "shaders/multiPhong.vert.spv", "shaders/multiPhong.frag.spv");

		mario->OnCreate(Vec3(0.0f, -1.0f, 0.0f), Vec3(0, 0, 0), orient, marioDescSetInfo, marioPipelineInfo, Vec3(0.5f, 0.5f, 0.5f));

		actorsLookup["mario"] = mario;
		actorLoop.push_back(mario);

		// BRZ actor creation
		brz = actorPool.Acquire();

		brz->createTexture(vRenderer->Create2DTextureImage("./textures/brz.png"));
		brz->createMesh(vRenderer->LoadModelIndexed("./meshes/brz.obj"));

		DescriptorSetBuilder brzDescriptorSetBuilder(vRenderer->getDevice());
		brzDescriptorSetBuilder.add(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 1, camera->getCameraUBO());
		brzDescriptorSetBuilder.add(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 1, lightsUBO);
		brzDescriptorSetBuilder.add(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 1, &brz->GetSampler());
		DescriptorSetInfo brzDescSetInfo = brzDescriptorSetBuilder.BuildDescriptorSet(vRenderer->getNumSwapchains());
		PipelineInfo brzPipelineInfo = vRenderer->CreateGraphicsPipeline(brzDescSetInfo.descriptorSetLayout, "shaders/multiPhong.vert.spv", "shaders/multiPhong.frag.spv");

		brz->OnCreate(Vec3(0, 0, 0), Vec3(0, 0, 0), orient, brzDescSetInfo, brzPipelineInfo);

		actorsLookup["brz"] = brz;
		actorLoop.push_back(brz);

		// Load mp3
		audioSystem.loadSoundFromFile("mp3File", "./AudioFiles/soundtrack1.mp3", FMOD_DEFAULT, nullptr);
		// Load flac
		audioSystem.loadSoundFromFile("flacFile", "./AudioFiles/soundtrack2.flac", FMOD_DEFAULT, nullptr);
		// Load m4a (should not support)
		audioSystem.loadSoundFromFile("m4a", "./AudioFiles/soundtrack3.m4a", FMOD_DEFAULT, nullptr);
		break;
	}
	case RendererType::OPENGL:
		break;
	}
	int gamepadCount = 0;
	SDL_JoystickID* gamepadIDs = SDL_GetGamepads(&gamepadCount);

	if (gamepadCount > 0) {
		gamepad = SDL_OpenGamepad(gamepadIDs[0]);
		if (!gamepad) {
			Debug::Error("Failed to open gamepad: ", __FILE__, __LINE__);
		}
	}
	SDL_free(gamepadIDs);
	
	return true;
}

void Scene0::HandleEvents(const SDL_Event& sdlEvent) {
	
	float aspectRatio = static_cast<float>(sdlEvent.window.data1) / static_cast<float>(sdlEvent.window.data2);
	static int colorState = 0;
	static bool highlightState = false;

		switch (sdlEvent.type) {
		case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
			printf("size changed %d %d\n", sdlEvent.window.data1, sdlEvent.window.data2);
			if(renderer->getRendererType() == RendererType::VULKAN){
				camera->setPerspective(Vec4(45.0f, aspectRatio, 0.5f, 100.0f));
				dynamic_cast<VulkanRenderer*>(renderer)->RecreateSwapChain();
			}
			break;

		// Keyboard
		case SDL_EVENT_KEY_UP:
			
			switch (sdlEvent.key.key)
			{
			case SDLK_1:
				// Set all lights to off
				for (uint32_t i = 0; i < lights.numLights; ++i)
				{
					lights.diffuse[i] *= 0;
					lights.specular[i] *= 0;
				}

				// Loop through all 3 states
				colorState = (colorState + 1) % 4;

				switch (colorState)
				{
				case 1:
					lights.diffuse[0] = redLight;
					break;
				case 2:
					lights.diffuse[1] = greenLight;
					break;
				case 3:
					lights.diffuse[2] = blueLight;
					break;
				case 0:
					break;
				}
				dynamic_cast<VulkanRenderer*>(renderer)->UpdateUniformBuffer<LightsData>(lights, lightsUBO); // updates buffer to apply changes
				break;

			case SDLK_F:
				if (!audioSystem.isSoundPlaying("flacFile"))
				{
					audioSystem.playSound("flacFile", false);
				}
				else {
					audioSystem.stopSound("flacFile");
				}
				break;
			case SDLK_M:
				if (!audioSystem.isSoundPlaying("mp3File"))
				{
					audioSystem.playSound("mp3File", false);
				}
				else
				{
					audioSystem.stopSound("mp3File");
				}
				break;
			case SDLK_4:
				if (!audioSystem.isSoundPlaying("m4aFile"))
				{
					audioSystem.playSound("m4aFile", false);
				}
				else
				{
					audioSystem.stopSound("m4aFile");
				}
				break;
			case SDLK_5:
				highlightState ? highlightState = false : highlightState = true;
				actorLoop[selectedActor]->setHighlighState(highlightState);
				break;
			case SDLK_6:
				selectedActor = (selectedActor + 1) % actorLoop.size();
				break;
			default:
				break;
			}
			break;

		// Gamepad buttons
		case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
			switch (sdlEvent.gbutton.button)
			{
			case SDL_GAMEPAD_BUTTON_DPAD_UP:
				highlightState ? highlightState = false : highlightState = true;
				actorLoop[selectedActor]->setHighlighState(highlightState);
				break;
			case SDL_GAMEPAD_BUTTON_DPAD_RIGHT:
				highlightState = false;
				actorLoop[selectedActor]->setHighlighState(highlightState);
				selectedActor = (selectedActor + 1) % actorLoop.size();
				highlightState = true;
				actorLoop[selectedActor]->setHighlighState(highlightState);
				break;
			case SDL_GAMEPAD_BUTTON_DPAD_LEFT:
				highlightState = false;
				actorLoop[selectedActor]->setHighlighState(highlightState);
				selectedActor = (selectedActor - 1 + static_cast<int>(actorLoop.size())) % static_cast<int>(actorLoop.size());
				highlightState = true;
				actorLoop[selectedActor]->setHighlighState(highlightState);
				break;
			default:
				break;
			}
			break;
		// Gamepad joysticks
		case SDL_EVENT_GAMEPAD_AXIS_MOTION:

			switch (sdlEvent.gaxis.axis) {
			case SDL_GAMEPAD_AXIS_LEFTX:
				stickLX = sdlEvent.gaxis.value;
				break;
			case SDL_GAMEPAD_AXIS_LEFTY:
				stickLY = sdlEvent.gaxis.value;
				break;
			case SDL_GAMEPAD_AXIS_RIGHTX:
				stickRX = sdlEvent.gaxis.value;
				break;
			case SDL_GAMEPAD_AXIS_RIGHTY:
				stickRY = sdlEvent.gaxis.value;
				break;
			default:
				break;
			}
			break;

		default:
			break;
		}

}

void Scene0::Update(const float deltaTime) {

	audioSystem.Update();

	// Rotate the selected actor using the left stick (X = yaw, Y = pitch)
	const float rotationSpeed = 90.0f; // degrees per second at full stick
	float yawInput = (std::abs(stickLX) > deadzone) ? stickLX / 32768.0f : 0.0f;
	float pitchInput = (std::abs(stickLY) > deadzone) ? stickLY / 32768.0f : 0.0f;

	if (yawInput != 0.0f || pitchInput != 0.0f)
	{
		Quaternion yaw = QMath::angleAxisRotation(yawInput * rotationSpeed * deltaTime, Vec3(0.0f, 1.0f, 0.0f));
		Quaternion pitch = QMath::angleAxisRotation(pitchInput * rotationSpeed * deltaTime, Vec3(1.0f, 0.0f, 0.0f));
		actorLoop[selectedActor]->rotate(yaw * pitch);
	}

	// Mario spins on his own unless the player has selected him to steer with the stick
	if (actorLoop[selectedActor] != mario)
	{
		mario->rotate(QMath::angleAxisRotation(rotationSpeed * deltaTime, Vec3(0.0f, 1.0f, 0.0f)));
	}

	// Move the selected actor left/right and up/down using the right stick
	const float moveSpeed = 3.0f; // units per second at full stick
	float moveXInput = (std::abs(stickRX) > deadzone) ? stickRX / 32768.0f : 0.0f;
	float moveYInput = (std::abs(stickRY) > deadzone) ? stickRY / 32768.0f : 0.0f;

	if (moveXInput != 0.0f || moveYInput != 0.0f)
	{
		// Stick up reports a negative value, so flip it to move up on screen
		actorLoop[selectedActor]->translate(Vec3(moveXInput * moveSpeed * deltaTime, -moveYInput * moveSpeed * deltaTime, 0.0f));
	}

	// Update all actors
	for (auto& actor : actorLoop)
	{
		actor->Update(deltaTime);
	}

	/*int profilerTest = 0;
	for (int i = 0; i < 10000000; ++i)
	{
		profilerTest = i;
	}*/
}

void Scene0::Render() const {
		switch (renderer->getRendererType()) {

		case RendererType::VULKAN:
			VulkanRenderer* vRenderer;
			vRenderer = dynamic_cast<VulkanRenderer*>(renderer);
		
			vRenderer->RecordCommandBuffers(Recording::START);

			// Render all actors
			for (auto& actor : actorLoop)
			{
				actor->Render(vRenderer);
			}
		
			ImGui::BeginChild("Scene Actor Count", ImVec2(300,0), ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_AutoResizeY);
			ImGui::Text("Total Actors Loaded: %d", actorLoop.size());
			ImGui::SetWindowFontScale(1.5f);

			ImGui::EndChild();
			ImGui::End();

			ImGui::Render();

			vRenderer->RecordCommandBuffers(Recording::STOP);
			vRenderer->Render();
			break;

		case RendererType::OPENGL:
			OpenGLRenderer* glRenderer;
			glRenderer = dynamic_cast<OpenGLRenderer*>(renderer);
			/// Clear the screen
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_CULL_FACE);
			/// Draw your scene here
			glUseProgram(0);
		
			break;
		}
}


void Scene0::OnDestroy() {
	VulkanRenderer* vRenderer;
	vRenderer = dynamic_cast<VulkanRenderer*>(renderer);
	if(vRenderer){
		vkDeviceWaitIdle(vRenderer->getDevice());
		vRenderer->DestroyCommandBuffers();

		size_t acquired = actorPool.FreeCount(); // Get pre release free space in pool (ie how many objects we used - pool size)

		// Destroy all actors
		for (auto& actor : actorLoop)
		{
			actor->OnDestroy(vRenderer);
			actorPool.Release(actor);			
		}
		actorLoop.clear();
		actorsLookup.clear();

		size_t released = actorPool.FreeCount(); // Get post release free space in pool (ideally pool size)

		std::cout << "Released: " << released - acquired << " actors\n"; // Subtract released (pool size) from acquired (objects used - pool size) to get total count

		vRenderer->DestroyUBO(lightsUBO);

		if (camera) {
			camera->OnDestroy();
			delete camera;
			camera = nullptr;
		}		
	}	
}